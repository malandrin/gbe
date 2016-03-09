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
void CPU::DecReg(u8 &_reg)
{
    u8 prevVal = _reg;
    --_reg;
    mFlagZ = (_reg == 0);
    mFlagN = true;
    mFlagH = (prevVal < 16) && (_reg >= 16);
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::SubRegA(u8 _reg)
{
    u8 prevVal = mRegA;
    mRegA -= _reg;
    mFlagZ = (mRegA == 0);
    mFlagN = true;
    mFlagH = false; // TODO: REVISAR COMO FUNCIONA EL FLAG H
    mFlagC = mRegA < _reg; // TODO: REVISAR ESTO TAMBIEN
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::RotateLeft(u8 &_reg)
{
    mFlagN = false;
    mFlagH = false;
    mFlagC = (_reg & 0b10000000) != 0;
    _reg = _reg << 1;
}


//--------------------------------------------
// --
//--------------------------------------------
void CPU::InternalStep()
{
    u8 opcode = mMmu.ReadU8(mRegPC++);

    switch(opcode)
    {
        case 0x04: // INC B
            IncReg(mRegBC.b);
            break;

        case 0x05: // DEC B
            DecReg(mRegBC.b);
            break;

        case 0x06: // LD B, n
            mRegBC.b = mMmu.ReadU8(mRegPC++);
            break;

        case 0x0C: // INC C
            IncReg(mRegBC.c);
            break;

        case 0x0D: // DEC C
            DecReg(mRegBC.c);
            break;

        case 0x0E: // LD C, n
            mRegBC.c = mMmu.ReadU8(mRegPC++);
            break;

        case 0x11: // LD DE, nn
            mRegDE.de = mMmu.ReadU16(mRegPC);
            mRegPC += 2;
            break;

        case 0x13: // INC DE
            ++mRegDE.de;
            break;

        case 0x15: // DEC D
            DecReg(mRegDE.d);
            break;

        case 0x16: // LD D, n
            mRegDE.d = mMmu.ReadU8(mRegPC++);
            break;

        case 0x17: // RLA
            RotateLeft(mRegA);
            break;

        case 0x18: // JR r8
        {
            i8 offset = (i8)mMmu.ReadU8(mRegPC++);
            mRegPC += offset;
        }
        break;

        case 0x1A: // LD A, (DE)
            mRegA = mMmu.ReadU8(mRegDE.de);
            break;

        case 0x1C: // INC E
            IncReg(mRegDE.e);
            break;

        case 0x1D: // DEC E
            DecReg(mRegDE.e);
            break;

        case 0x1E: // LD E, n
            mRegDE.e = mMmu.ReadU8(mRegPC++);
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

        case 0x22: // LD (HL+), A
            mMmu.WriteU8(mRegHL.hl++, mRegA);
            break;

        case 0x23: // INC HL
            ++mRegHL.hl;
            break;

        case 0x24: // INC H
            ++mRegHL.h;
            break;

        case 0x28: // JR Z, r8
        {
            i8 offset = (i8)mMmu.ReadU8(mRegPC++);

            if (mFlagZ)
                mRegPC += offset;
        }
        break;

        case 0x2E: // LD L, n
            mRegHL.l = mMmu.ReadU8(mRegPC++);
            break;

        case 0x31: // LD SP, nn
            mRegSP = mMmu.ReadU16(mRegPC);
			mRegPC += 2;
            break;

        case 0x32: // LD (HL-), A
			mMmu.WriteU8(mRegHL.hl--, mRegA);
	        break;

        case 0x3D: // DEC A
            DecReg(mRegA);
            break;

        case 0x3E: // LD A, n
            mRegA = mMmu.ReadU8(mRegPC++);
            break;

        case 0x4F: // LD C, A
            mRegBC.c = mRegA;
            break;

        case 0x57: // LD D, A
            mRegDE.d = mRegA;
            break;

        case 0x67: // LD H, A
            mRegHL.h = mRegA;
            break;

        case 0x77: // LD (HL), A
            mMmu.WriteU8(mRegHL.hl, mRegA);
            break;

        case 0x7B: // LD A, E
            mRegA = mRegDE.e;
            break;

        case 0x7C: // LD A, H
            mRegA = mRegHL.h;
            break;

        case 0x90: // SUB B
            SubRegA(mRegBC.b);
            break;

        case 0xAF: // XOR A
            mRegA = 0;
            mFlagZ = true;
            mFlagN = false;
            mFlagH = false;
            mFlagC = false;
            break;

        case 0xC1: // POP BC
            mRegBC.bc = Pop();
            break;

        case 0xC5: // PUSH BC
            Push(mRegBC.bc);
            break;

        case 0xC9: // RET
            mRegPC = Pop();
            break;

        case 0xCB: // CB
            ProcessCb(mMmu.ReadU8(mRegPC++));
            break;

        case 0xCD: // CALL nn
            Push(mRegPC + 2);
            mRegPC = mMmu.ReadU16(mRegPC);
            break;

        case 0xE0: // LD (0xFF00 + n), A
            mMmu.WriteU8(0xFF00 + mMmu.ReadU8(mRegPC++), mRegA);
            break;

        case 0xE2: // LD (0xFF00 + C), A
            mMmu.WriteU8(0xFF00 + mRegBC.c, mRegA);
            break;

        case 0xEA: // LD (nn), A
            mMmu.WriteU8(mMmu.ReadU16(mRegPC), mRegA);
            mRegPC += 2;
            break;

        case 0xF0: // LD A, (0xFF00 + n)
            mRegA = mMmu.ReadU8(0xFF00 + mMmu.ReadU8(mRegPC++));
            break;

        case 0xFE: // CP n
        {
            u8 n = mMmu.ReadU8(mRegPC++);
            u8 diff = mRegA - n;

            mFlagZ = (mRegA == n);
            mFlagN = true;
            mFlagH = (diff < 16);
            mFlagC = (mRegA < n);
        }
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
        case 0x11: // RL C
            RotateLeft(mRegBC.c);
            break;

        case 0x7C: // BIT 7, H
            if ((mRegHL.h & 0b10000000) != 0)
                mFlagZ = false;
            else
                mFlagZ = true;

            mFlagN = false;
            mFlagH = true;
            break;

        default:
            throw runtime_error("CB opcode unknown: " + Int2Hex(_opcode));
            break;
    }
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::Push(u16 _val)
{
    mRegSP -= 2;
    mMmu.WriteU16(mRegSP, _val);
}

//--------------------------------------------
// --
//--------------------------------------------
u16 CPU::Pop()
{
    u16 val = mMmu.ReadU16(mRegSP);
    mRegSP += 2;
    return val;
}