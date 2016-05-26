#include "base.h"
#include "defines.h"
#include "mmu.h"
#include "gpu.h"

static u32 mColors[4] {0x9BBC0FFF, 0x8BAC0FFF, 0x306230FF, 0x0F380FFF};

//--------------------------------------------
// --
//--------------------------------------------
GPU::GPU(MMU& _mmu)
{
    _mmu.AddListener(this);
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
            mDirty = true;
            break;
    }

    if ((_virtAddr >= Memory::VRamStartAddr) && (_virtAddr <= Memory::VRamEndAddr))
    {
        mDirty = true;
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
const u32* GPU::GetScreen()
{
    if (mDirty)
    {
        // calcular imagen final
        mDirty = false;
    }

    return mScreen;
}