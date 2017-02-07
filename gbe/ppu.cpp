#include "base.h"
#include "defines.h"
#include "mmu.h"
#include "cpu.h"
#include "defines.h"
#include "ppu.h"

static u32 mColors[4] {0x9BBC0F, 0x8BAC0F, 0x306230, 0x0F380F};

//--------------------------------------------
// --
//--------------------------------------------
PPU::PPU(CPU &_cpu, MMU &_mmu) : mMmu(_mmu)
{
    _mmu.AddListener(this);
}

//--------------------------------------------
// --
//--------------------------------------------
void PPU::OnMemoryWrittenU8(u16 _virtAddr, u8 _value)
{
    // IO registers
    switch(_virtAddr)
    {
        case IOReg::BGP:
            mPalette[0] = (_value & 0b11);
            mPalette[1] = (_value >> 2) & 0b11;
            mPalette[2] = (_value >> 4) & 0b11;
            mPalette[3] = (_value >> 6) & 0b11;
            break;

        case IOReg::OBP0:
            mSprPalette0[0] = 0;
            mSprPalette0[1] = (_value >> 2) & 0b11;
            mSprPalette0[2] = (_value >> 4) & 0b11;
            mSprPalette0[3] = (_value >> 6) & 0b11;
            break;

        case IOReg::OBP1:
            mSprPalette1[0] = 0;
            mSprPalette1[1] = (_value >> 2) & 0b11;
            mSprPalette1[2] = (_value >> 4) & 0b11;
            mSprPalette1[3] = (_value >> 6) & 0b11;
            break;

        case IOReg::LCDC:
            mBGWindowDisplayOn = (_value & 1) != 0;
            mObjDisplayOn = (_value & (1 << 1)) != 0;
            mObjSize = (_value & (1 << 2)) != 0 ? 16 : 8;
            mBGTileMap = (_value & (1 << 3)) != 0 ? Memory::VRamTileMap2StartAddr : Memory::VRamTileMap1StartAddr;
            mBGTileData = (_value & (1 << 4)) != 0 ? Memory::VRamTileData1StartAddr : Memory::VRamTileData2StartAddr;
            mWindowDisplayOn = (_value & (1 << 5)) != 0;
            mWindowTileMap = (_value & (1 << 6)) != 0 ? Memory::VRamTileMap2StartAddr : Memory::VRamTileMap1StartAddr;
            mLCDOn = (_value & (1 << 7)) != 0;
            break;

        case IOReg::DMA:
            mMmu.CopyMem(_value * 0x100, Memory::OAMStartAddr, 40 * 4);
            break;
    }
}

//--------------------------------------------
// --
//--------------------------------------------
void PPU::OnMemoryWrittenU16(u16 _virtAddr, u16 _value)
{
}

//--------------------------------------------
// --
//--------------------------------------------
void PPU::OnStep(int _numCycles)
{
    mCycles += _numCycles;

    switch(mMode)
    {
        case HBLANK:
            if (mCycles >= Cycles::PerHBlank)
            {
                u8 cl = mMmu.ReadU8(IOReg::LY) + 1;

                if (cl >= Screen::Height)
                {
                    SetMode(VBLANK);                    
                    
                    // buffer to screen
                    for (int y = 0; y < Screen::Height; ++y)
                    {
                        for (int x = 0; x < Screen::Width; ++x)
                            mScreen[(y * Screen::Width) + x] = mColors[mBuffer[(y * 256) + x]];
                    }
                }
                else
                    SetMode(OAM);

                mMmu.WriteU8(IOReg::LY, cl);
                OnHLine();
            }
            break;

        case VBLANK:
            // 4560 = (204 + 80 + 127) * 10 lines
            if (mCycles >= Cycles::PerScanline) 
            {
                u8 cl = mMmu.ReadU8(IOReg::LY) + 1;

                if (cl > Screen::TotalHeight)
                {
                    SetMode(OAM);
                    mMmu.WriteU8(IOReg::LY, 0);
                }
                else
                {
                    mMmu.WriteU8(IOReg::LY, cl);
                    mCycles = 0;
                }

                OnHLine();
            }
            break;

        case OAM:
            if (mCycles >= Cycles::PerOAM)
                SetMode(OAM_VRAM);
            break;

        case OAM_VRAM:
            if (mCycles >= Cycles::PerVRAM)
            {
                SetMode(HBLANK);
                RenderScanline();
            }
            break;
    }
}

//--------------------------------------------
// --
//--------------------------------------------
void PPU::RenderScanline()
{
    if (!mLCDOn)
        return;

    int line = mMmu.ReadU8(IOReg::LY);
    int mapX = mMmu.ReadU8(IOReg::SCX);
    int mapY = (line + mMmu.ReadU8(IOReg::SCY) % 256);
    int wndX = mMmu.ReadU8(IOReg::WX) - 7;
    int wndY = mMmu.ReadU8(IOReg::WY);
    bool signedTiles = (mBGTileData == Memory::VRamTileData2StartAddr);

    // draw tile map & window
    if (mBGWindowDisplayOn)
    {
        for (int x = 0; x < Screen::Width; ++x)
        {
            if (mWindowDisplayOn && (line >= wndY) && (x >= wndX))
            {
                int rx = x - wndX;
                int tileIdx = (((line - wndY) / 8) * 32) + (rx / 8);
                u8 tileNum = mMmu.ReadU8(mWindowTileMap + tileIdx);
                u16 tileAddr = signedTiles ? ((0x9000 + (i8)tileNum * 16)) : (mWindowTileMap + tileNum * 16);
                int tx = rx % 8;
                int ty = (line - wndY) % 8;

                tileAddr += (ty * 2);

                u8 b1 = mMmu.ReadU8(tileAddr);
                u8 b2 = mMmu.ReadU8(tileAddr + 1);
                u8 cp = ((b1 >> (7 - tx) & 1) << 1) | ((b2 >> (7 - tx) & 1));
                mBuffer[(line * 256) + x] = mPalette[cp];
            }
            else
            {
                int rx = (mapX + x) % 256;
                int tileIdx = ((mapY / 8) * 32) + (rx / 8);
                u8 tileNum = mMmu.ReadU8(mBGTileMap + tileIdx);
                u16 tileAddr = signedTiles ? ((0x9000 + (i8)tileNum * 16)) : (mBGTileMap + tileNum * 16);
                int tx = rx % 8;
                int ty = mapY % 8;

                tileAddr += (ty * 2);

                u8 b1 = mMmu.ReadU8(tileAddr);
                u8 b2 = mMmu.ReadU8(tileAddr + 1);
                u8 cp = ((b1 >> (7 - tx) & 1) << 1) | ((b2 >> (7 - tx) & 1));
                mBuffer[(line * 256) + x] = mPalette[cp];
            }
        }
    }

    // sprites
    if (mObjDisplayOn)
    {
        u16 addr = Memory::OAMStartAddr;

        for (int s = 0; s < 40; ++s)
        {
            u8 sprPosY = mMmu.ReadU8(addr++) - 16;
            u8 sprPosX = mMmu.ReadU8(addr++) - 8;

            if ((sprPosY > line) || ((sprPosY + mObjSize) <= line) || (sprPosX <= 0) || (sprPosX >= Screen::Width))
            {
                addr += 2;
                continue;
            }

            u8 sprTile = mMmu.ReadU8(addr++);
            u8 sprAttr = mMmu.ReadU8(addr++);
            u16 tileAddr = Memory::VRamTileData1StartAddr + (sprTile * 16);
            bool flipY = (sprAttr & (1 << 6)) != 0;
            int sprLine = flipY ? (mObjSize - 1 - (line - sprPosY)) : (line - sprPosY); // todo: comprobar que funciona el flipY
            tileAddr += sprLine * 2;

            u8 b1 = mMmu.ReadU8(tileAddr++);
            u8 b2 = mMmu.ReadU8(tileAddr++);

            bool priority = !((sprAttr & (1 << 7)) != 0);
            bool flipX = (sprAttr & (1 << 5)) != 0;
            bool palette1 = (sprAttr & (1 << 4)) != 0;

            for (int x = 0; x < 8; ++x)
            {
                if (sprPosX + x >= Screen::Width)
                    break;

                int rx = flipX ? (7 - x) : x;
                u8 cp = (((b1 >> (7 - rx)) & 1) << 1) | ((b2 >> (7 - rx)) & 1);
                int coord = (line * 256) + sprPosX + x;

                if ((cp == 0) || (!priority && mBuffer[coord] != 0))
                    continue;

                mBuffer[coord] = palette1 ? mSprPalette1[cp] : mSprPalette0[cp];
            }
        }
    }
}

//--------------------------------------------
// --
//--------------------------------------------
void PPU::OnHLine()
{
    u8 line = mMmu.ReadU8(IOReg::LY);
    u8 stat = mMmu.ReadU8(IOReg::STAT);
    bool cline = line == mMmu.ReadU8(IOReg::LYC);

    if (cline)
        stat |= (1 << 2);
    else
        stat &= ~(1 << 2);

    mMmu.WriteU8(IOReg::STAT, stat, false);

    if (cline && (stat & (1 << 6)) != 0)
        mMmu.WriteU8(IOReg::IF, mMmu.ReadU8(IOReg::IF) | (1 << 1));
}

//--------------------------------------------
// --
//--------------------------------------------
void PPU::SetMode(int _newMode)
{
    mMode = _newMode;
    mCycles = 0;

    u8 stat = (mMmu.ReadU8(IOReg::STAT) & 0b11111100) | mMode;
    mMmu.WriteU8(IOReg::STAT, stat, false);
    
    // ...
    switch(mMode)
    {
        case HBLANK:
            if ((stat & (1 << 3)) != 0)
                mMmu.WriteU8(IOReg::IF, mMmu.ReadU8(IOReg::IF) | (1 << 1));
            break;

        case VBLANK:
            mMmu.WriteU8(IOReg::IF, mMmu.ReadU8(IOReg::IF) | 1);

            if ((stat & (1 << 4)) != 0)
                mMmu.WriteU8(IOReg::IF, mMmu.ReadU8(IOReg::IF) | (1 << 1));
            break;

        case OAM_VRAM:
            if ((stat & (1 << 5)) != 0)
                mMmu.WriteU8(IOReg::IF, mMmu.ReadU8(IOReg::IF) | (1 << 1));
            break;
    }
}