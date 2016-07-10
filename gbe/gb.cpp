#include "base.h"
#include "cpu.h"
#include "mmu.h"
#include "gb.h"

//--------------------------------------------
// --
//--------------------------------------------
void GB::PowerUp(const string &_cartridge, const string &_bootRom)
{
    try
    {
        mMmu.LoadRoms(_cartridge, _bootRom);

        if (_bootRom == "")
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
