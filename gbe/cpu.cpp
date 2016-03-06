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
    try
    {
        InternalStep();
    }
    catch(runtime_error &e)
    {
        cout << e.what() << '\n';
        exit(-1);
    }
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::IncReg(u8 &_reg)
{
    u8 prevVal = _reg;
    ++_reg;
    mFlagZ = (_reg == 0);
    mFlagN = false;
    mFlagH = (prevVal < 16) && (_reg >= 16);
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::InternalStep()
{
    u8 opcode = mMmu.ReadU8(mRegPC++);

    switch(opcode)
    {
        case 0x0C: // INC C
            IncReg(mRegC);
            break;

        case 0x0E: // LD C, n
            mRegC = mMmu.ReadU8(mRegPC++);
            break;

        case 0x11: // LD DE, nn
            mRegDE.de = mMmu.ReadU16(mRegPC);
            mRegPC += 2;
            break;

        case 0x1A: // LD A, (DE)
            mRegA = mMmu.ReadU8(mRegDE.de);
            break;

        case 0x1C: // INC E
            IncReg(mRegDE.e);
            break;

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

        case 0x3E: // LD A, n
            mRegA = mMmu.ReadU8(mRegPC++);
            break;

        case 0x77: // LD (HL), A
            mMmu.WriteU8(mRegHL.hl, mRegA);
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

        case 0xE0: // LD (0xFF00 + n), A
            mMmu.WriteU8(0xFF00 + mMmu.ReadU8(mRegPC++), mRegA);
            break;

        case 0xE2: // LD (0xFF00 + C), A
            mMmu.WriteU8(0xFF00 + mRegC, mRegA);
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