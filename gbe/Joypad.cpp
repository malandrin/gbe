#include "base.h"
#include "defines.h"

#include "joypad.h"

//--------------------------------------------
// --
//--------------------------------------------
Joypad::Joypad(MMU &_mmu) : mMmu(_mmu)
{
    _mmu.AddListener(this);
}

//--------------------------------------------
// --
//--------------------------------------------
void Joypad::OnMemoryWrittenU8(u16 _virtAdd, u8 _value)
{
    if (_virtAdd == IOReg::P1)
    {
        int offset = -1;

        if ((_value & 0x10) != 0) // P15
            offset = 4;
        else if ((_value & 0x20) != 0) // P14
            offset = 0;

        if (offset != -1)
        {
            _value = 0;

            for (int i = 0; i < 4; ++i)
                _value |= (mKeyPressed[offset + i] ? 0 : 1) << i;

            mMmu.WriteU8(IOReg::P1, _value, false);
        }
    }
}

//--------------------------------------------
// --
//--------------------------------------------
void Joypad::OnKeyDown(Key _key)
{
    mKeyPressed[_key] = true;
    mMmu.WriteU8(IOReg::IF, mMmu.ReadU8(IOReg::IF) | (1 << 4));
}