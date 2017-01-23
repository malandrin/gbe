#include "base.h"
#include "cpu.h"
#include "mmu.h"
#include "gb.h"

//--------------------------------------------
// --
//--------------------------------------------
void GB::PowerUp(Cartridge *_cartridge)
{
    
    try
    {
        //mMmu.LoadRoms(_cartridge, _bootRom);
        mCartridge = _cartridge;
        mMmu.OnCartridgeInserted(_cartridge);

        //if (_bootRom == "")
        {
            mCpu.SetStateAfterBoot();
            mMmu.SetStateAfterBoot();
        }
    }
    catch(runtime_error &e)
    {
        cout << e.what() << '\n';
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