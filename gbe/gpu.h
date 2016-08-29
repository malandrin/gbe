#pragma once

#include "mmu_listener.h"
#include "defines.h"

class CPU;
class MMU;

class GPU : public IMmuListener
{
public:

         GPU                (CPU &_cpu, MMU &_mmu);

    // MMU listener
    void OnMemoryWrittenU8  (u16 _virtAddr, u8 _value);
    void OnMemoryWrittenU16 (u16 _virtAddr, u16 _value);

    void OnStep             (int _numCycles);

    bool IsLCDOn            () const { return mLCDOn; }

    const u32* GetFrameBuffer () const { return mScreen; }

private:

    enum Mode
    {
        HBLANK = 0,
        VBLANK,
        OAM,
        OAM_VRAM
    };

    MMU &mMmu;
    u8  mPalette[4] {0};
    u8  mSprPalette0[4]{ 0 };
    u8  mSprPalette1[4]{ 0 };
    u8  mBuffer[256 * 256];
    u32 mScreen[Screen::Width * Screen::Height] {0};
    int mMode {OAM};
    int mCycles {0};

    bool mBGWindowDisplayOn {false};
    bool mObjDisplayOn {false};
    bool mWindowDisplayOn {false};
    bool mLCDOn {false};
    u8   mObjSize {0};
    u16  mBGTileMap {0};
    u16  mBGTileData {0};
    u16  mWindowTileMap {0};

    void RenderTile         (u16 _tileDataAddr, u8 _numTile, u8 _x, u8 _y);
    void SetMode            (int _newMode);
    void UpdateFrameBuffer  ();
};
