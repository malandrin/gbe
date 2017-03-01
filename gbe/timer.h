#pragma once

#include "mmu_listener.h"
#include "mmu.h"
#include "cpu.h"

class Timer : public IMmuListener
{
public:

         Timer              (CPU &_cpu, MMU &_mmu);

    void Update             (u8 _numCycles);

    void OnMemoryWrittenU8  (u16 _virtAdd, u8 _value);
    void OnMemoryWrittenU16 (u16 _virtAdd, u16 _value) {}

    void Reset              ();

private:

    MMU  &mMmu;
    CPU  &mCpu;
    int  mDivTimer { 0 };
    int  mTimaTimer { 0 };
    bool mTimerEnabled { true };
    int  mTimerFreq { 0 };
};
