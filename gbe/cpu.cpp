#include "base.h"
#include "mmu.h"
#include "cpu.h"

//--------------------------------------------
// --
//--------------------------------------------
void CPU::Step()
{
    u8 opcode = mMmu.ReadU8(mRegPC++);

    switch(opcode)
    {
        case 0x21: // LD HL, nn
			mRegL = mMmu.ReadU8(mRegPC++);
            mRegH = mMmu.ReadU8(mRegPC++);
            break;

        case 0x31: // LD SP, nn
            mRegSP = mMmu.ReadU16(mRegPC);
			mRegPC += 2;
            break;

        case 0x32: // LD (HL-), A
        {
            u16 aux = U8sToU16(mRegL, mRegH);
            mMmu.WriteU8(aux--, mRegA);
            U16ToU8s(aux, mRegL, mRegH);
        }
        break;

        case 0xAF: // XOR A
            mRegA = 0;
            mFlagZ = true;
            mFlagN = false;
            mFlagH = false;
            mFlagC = false;
            break;

        default:
            throw runtime_error("opcode unknown: " + Int2Hex(opcode));
            break;
    }
}