#pragma once

#include "mmu_listener.h"
#include "mmu.h"

class Timer : public IMmuListener
{
public:

         Timer              (MMU &_mmu);

    void Update             (u8 _numCycles);

    void OnMemoryWrittenU8  (u16 _virtAdd, u8 _value);
    void OnMemoryWrittenU16 (u16 _virtAdd, u16 _value) {}

private:

    MMU  &mMmu;
    int  mDivTimer { 0 };
    int  mTimaTimer { 0 };
    bool mTimerEnabled { true };
    int  mTimerFreq { 0 };
};
