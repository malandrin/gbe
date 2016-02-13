#include "base.h"
#include "cpu.h"
#include "mmu.h"
#include "gb.h"

//--------------------------------------------
// --
//--------------------------------------------
void GB::PowerUp(const string &_bootableRom, const string &_cartridge)
{
    try
    {
        mMmu.LoadRoms(_bootableRom, _cartridge);
        mCpu.SetMmu(&mMmu);

        while(true)
        {
            mCpu.Step();
        }
    }
    catch(runtime_error &e)
    {
        cout << e.what() << '\n';
    }
}