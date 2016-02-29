#include "base.h"
#include "mmu.h"
#include "cpu_listener.h"
#include "cpu.h"

//--------------------------------------------
// --
//--------------------------------------------
void CPU::AddListener(ICpuListener *_listener)
{
    mListeners.push_back(_listener);
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::Step()
{
    u8 opcode = mMmu.ReadU8(mRegPC++);

    switch(opcode)
    {
        case 0x20: // JR NZ, r8
		{
			i8 offset = (i8)mMmu.ReadU8(mRegPC++);

			if (!mFlagZ)
				mRegPC += offset;
		}
        break;

        case 0x21: // LD HL, nn
			mRegHL.hl = mMmu.ReadU16(mRegPC);
			mRegPC += 2;
            break;

        case 0x31: // LD SP, nn
            mRegSP = mMmu.ReadU16(mRegPC);
			mRegPC += 2;
            break;

        case 0x32: // LD (HL-), A
			mMmu.WriteU8(mRegHL.hl--, mRegA);
	        break;

        case 0xAF: // XOR A
            mRegA = 0;
            mFlagZ = true;
            mFlagN = false;
            mFlagH = false;
            mFlagC = false;
            break;

        case 0xCB: // CB
            ProcessCb(mMmu.ReadU8(mRegPC++));
            break;

        default:
            throw runtime_error("opcode unknown: " + Int2Hex(opcode));
            break;
    }

    // ...
    for each(auto l in mListeners)
        l->OnStep();
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::ProcessCb(u8 _opcode)
{
    switch(_opcode)
    {
        case 0x7C: // BIT 7, H
            if ((mRegHL.h & 0b10000000) != 0)
                mFlagZ = false;
            else
                mFlagZ = true;

            mFlagN = false;
            mFlagH = true;
            break;
    }
}