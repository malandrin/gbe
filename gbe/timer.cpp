#include "base.h"
#include "defines.h"

#include "timer.h"

//--------------------------------------------
// --
//--------------------------------------------
Timer::Timer(MMU &_mmu) : mMmu(_mmu)
{
    _mmu.AddListener(this);
}

//--------------------------------------------
// --
//--------------------------------------------
void Timer::Update(u8 _numCycles)
{
    mDivTimer += _numCycles;

    if (mDivTimer >= 4)
    {
        mMmu.WriteU8(IOReg::DIV, mMmu.ReadU8(IOReg::DIV) + 1, false);
        mDivTimer -= 4;
    }

    if (mTimerEnabled)
    {
        mTimaTimer += _numCycles;

        if (mTimaTimer >= mTimerFreq)
        {
            u16 value = mMmu.ReadU8(IOReg::TIMA);
            value += 1;
            mTimaTimer -= mTimerFreq;

            if (value > 255)
            {
                mMmu.WriteU8(IOReg::IF, mMmu.ReadU8(IOReg::IF) | (1 << 2));
                value = mMmu.ReadU8(IOReg::TAC);
            }

            mMmu.WriteU8(IOReg::TIMA, (u8)value);
        }
    }
}

//--------------------------------------------
// --
//--------------------------------------------
void Timer::OnMemoryWrittenU8(u16 _virtAddr, u8 _value)
{
    switch(_virtAddr)
    {
        case IOReg::DIV:
            mMmu.WriteU8(IOReg::DIV, 0, false);
            break;

        case IOReg::TAC:
            mTimerEnabled = (_value & 0x4) != 0;
            switch(_value & 0x3)
            {
                case 0: mTimerFreq = 1024;  break;
                case 1: mTimerFreq = 16;    break;
                case 2: mTimerFreq = 64;    break;
                case 3: mTimerFreq = 256;   break;
            }
            break;
    }
}

