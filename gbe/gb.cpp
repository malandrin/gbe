#include "base.h"
#include "cpu.h"
#include "mmu.h"
#include "gb.h"

//--------------------------------------------
// --
//--------------------------------------------
void GB::PowerUp(Cartridge *_cartridge, bool _runBootRom)
{    
    mCartridge = _cartridge;
    mMmu.OnCartridgeInserted(_cartridge);

    if (!_runBootRom)
    {
        mCpu.SetStateAfterBoot();
        mMmu.SetStateAfterBoot();
    }
}

//--------------------------------------------
// --
//--------------------------------------------
int GB::Step()
{
    int numCycles = mCpu.Step();
    mPpu.OnStep(numCycles);

    return numCycles;
}