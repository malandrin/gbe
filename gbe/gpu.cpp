#include "base.h"
#include "defines.h"
#include "mmu.h"
#include "cpu.h"
#include "defines.h"
#include "gpu.h"

static u32 mColors[4] {0x9BBC0F, 0x8BAC0F, 0x306230, 0x0F380F};

//--------------------------------------------
// --
//--------------------------------------------
GPU::GPU(CPU &_cpu, MMU &_mmu) : mMmu(_mmu)
{
    _mmu.AddListener(this);
    _cpu.AddListener(this);
}

//--------------------------------------------
// --
//--------------------------------------------
void GPU::OnMemoryWrittenU8(u16 _virtAddr, u8 _value)
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

        case IOReg::LCDC:
            mBGWindowDisplayOn = (_value & 1) != 0;
            mObjDisplayOn = (_value & (1 << 1)) != 0;
            mObjSize = (_value & (1 << 2)) != 0 ? 1 : 0;
            mBGTileMap = (_value & (1 << 3)) != 0 ? Memory::VRamTileMap2StartAddr : Memory::VRamTileMap1StartAddr;
            mBGTileData = (_value & (1 << 4)) != 0 ? Memory::VRamTileData1StartAddr : Memory::VRamTileData2StartAddr;
            mWindowDisplayOn = (_value & (1 << 5)) != 0;
            mWindowTileMap = (_value & (1 << 6)) != 0 ? Memory::VRamTileMap1StartAddr : Memory::VRamTileMap2StartAddr;
            mLCDOn = (_value & (1 << 7)) != 0;
            break;
    }
}

//--------------------------------------------
// --
//--------------------------------------------
void GPU::OnMemoryWrittenU16(u16 _virtAdd, u16 _value)
{
}

//--------------------------------------------
// --
//--------------------------------------------
void GPU::OnStep(int _numCycles)
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
                    UpdateFrameBuffer();
                }
                else
                    SetMode(OAM);

                mMmu.WriteU8(IOReg::LY, cl);
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
            }
            break;

        case OAM:
            if (mCycles >= Cycles::PerOAM)
                SetMode(OAM_VRAM);
            break;

        case OAM_VRAM:
            if (mCycles >= Cycles::PerVRAM)
                SetMode(HBLANK);
            break;
    }
}

//--------------------------------------------
// --
//--------------------------------------------
void GPU::UpdateFrameBuffer()
{
    if (mBGWindowDisplayOn)
    {
        int r = 0;
        int c = 0;

        for (int t = 0; t < 1024; ++t)
        {
            RenderTile(mBGTileData, mMmu.ReadU8(mBGTileMap + t), c * 8, r * 8);

            ++c;
            if (c == 32)
            {
                c = 0;
                ++r;
            }
        }

        // ...
        int sx = mMmu.ReadU8(IOReg::SCX);
        int sy = mMmu.ReadU8(IOReg::SCY);

        for (int y = 0; y < Screen::Height; ++y)
        {
            for (int x = 0; x < Screen::Width; ++x)
            {
                int bx = (sx + x) % 256;
                int by = (sy + y) % 256;

                if (mBuffer[(by * 256) + bx] != 0)
                    int a = 0;

                mScreen[(y * Screen::Width) + x] = mColors[mBuffer[(by * 256) + bx]];
            }
        }
    }
}

//--------------------------------------------
// --
//--------------------------------------------
void GPU::RenderTile(u16 _tileDataAddr, u8 _numTile, u8 _x, u8 _y)
{
    u16 tileAddr = _tileDataAddr + (_numTile * 16);

    for (int r = 0; r < 8; ++r)
    {
        u8 b1 = mMmu.ReadU8(tileAddr++);
        u8 b2 = mMmu.ReadU8(tileAddr++);

        for (int b = 7; b >= 0; --b)
        {
            u8 cp = (((b1 >> b) & 1) << 1) | ((b2 >> b) & 1);
            mBuffer[((_y + r) * 256) + _x + (7 - b)] = cp;
        }
    }
}

//--------------------------------------------
// --
//--------------------------------------------
void GPU::SetMode(int _newMode)
{
    mMode = _newMode;
    mCycles = 0;
}