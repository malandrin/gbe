#include "base.h"
#include "mmu.h"
#include "cpu.h"

//--------------------------------------------
// --
//--------------------------------------------
void CPU::Step()
{
    u8 opcode = mMmu->ReadU8(mRegPC++);

    switch(opcode)
    {
        case 0x31: // LD SP, nn
            mRegSP = mMmu->ReadU16(mRegPC);
			mRegPC += 2;
            cout << "LD SP, " << Int2Hex(mRegSP) << "\n";
            break;

        default:
            throw runtime_error("opcode unknown: " + Int2Hex(opcode));
            break;
    }
}