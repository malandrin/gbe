#include <assert.h>
#include "base.h"
#include "mmu.h"
#include "opcodes_info.h"
#include "defines.h"
#include "cpu.h"

//--------------------------------------------
// --
//--------------------------------------------
CPU::CPU(MMU &_mmu) : mMmu(_mmu)
{
}

//--------------------------------------------
// --
//--------------------------------------------
CPU::~CPU()
{
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::SetStateAfterBoot()
{
    mRegPC = 0x100;
    mRegA = 0x01;
    mRegBC.b = 0x00;
    mRegBC.c = 0x13;
    mRegDE.d = 0x00;
    mRegDE.e = 0xD8;
    mRegHL.h = 0x01;
    mRegHL.l = 0x4D;
    mRegSP = 0xFFFE;
    mSPStartAddr = mRegSP;
    mFlagC = true;
    mFlagH = false;
    mFlagN = false;
    mFlagZ = true;
}

//--------------------------------------------
// --
//--------------------------------------------
int CPU::Step()
{
    try
    {
        return InternalStep();
    }
    catch(runtime_error &e)
    {
        cout << e.what() << '\n';
        exit(-1);
    }

    return -1;
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
void CPU::SubRegA(u8 _val)
{
    u8 prevVal = mRegA;
    mRegA -= _val;
    mFlagZ = (mRegA == 0);
    mFlagN = true;
    mFlagH = false; // TODO: REVISAR COMO FUNCIONA EL FLAG H
    mFlagC = mRegA < _val; // TODO: REVISAR ESTO TAMBIEN
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::AddRegA(u8 _val)
{
    bool b3 = (mRegA & 0b00001000) != 0;
    bool b7 = (mRegA & 0b10000000) != 0;

    mRegA += _val;
    mFlagZ = (mRegA == 0);
    mFlagN = false;
    mFlagH = b3 && ((mRegA & 0b00001000) == 0);
    mFlagC = b7 && ((mRegA & 0b10000000) == 0);
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::CpRegA(u8 _val)
{
    mFlagZ = (mRegA == _val);
    mFlagN = true;
    mFlagH = false; // TODO: REVISAR ESTO
    mFlagC = (mRegA < _val);
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::AndRegA(u8 _val)
{
    mRegA = mRegA & _val;
    mFlagZ = mRegA == 0;
    mFlagN = false;
    mFlagH = true;
    mFlagC = false;
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::AddReg(u16 &_dest, u16 _orig)
{
    _dest += _orig;
    mFlagN = false;
    
    // TODO: implementar FlagH y FlagC
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::RotateLeft(u8 &_reg)
{
    mFlagN = false;
    mFlagH = false;
    u8 cb = mFlagC ? 1 : 0;
    mFlagC = (_reg & 0b10000000) != 0;
    _reg = (_reg << 1) | cb;
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::RotateLeftC(u8 &_reg)
{
    mFlagC = (_reg & 0b10000000) != 0;
    _reg = _reg << 1;
    mFlagZ = _reg == 0;
    mFlagN = false;
    mFlagH = false;
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::RotateRight(u8 &_reg)
{
    mFlagN = false;
    mFlagH = false;
    mFlagC = (_reg & 1) != 0;
    _reg = (_reg >> 1) | ((mFlagC ? 1 : 0) << 7);
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::RotateRightC(u8 &_reg)
{
    mFlagC = (_reg & 1) != 0;
    _reg = _reg >> 1;
    mFlagZ = _reg == 0;
    mFlagN = false;
    mFlagH = false;
}

//--------------------------------------------
// --
//--------------------------------------------
int CPU::InternalStep()
{
    if (mHalted)
        return 0;

    // ...
    u8 opcode = mMmu.ReadU8(mRegPC++);
    int numCycles = OpcodesInfo::primary[opcode].cyclesDuration;

    switch(opcode)
    {
        case 0x00: // NOP
            break;

        case 0x01: // LD BC, nn
            mRegBC.bc = mMmu.ReadU16(mRegPC);
            mRegPC += 2;
            break;

        case 0x02: // LD (BC), A
            mMmu.WriteU8(mRegBC.bc, mRegA);
            break;

        case 0x03: // INC BC
            ++mRegBC.bc;
            break;

        case 0x04: // INC B
            IncReg(mRegBC.b);
            break;

        case 0x05: // DEC B
            DecReg(mRegBC.b);
            break;

        case 0x06: // LD B, n
            mRegBC.b = mMmu.ReadU8(mRegPC++);
            break;

        case 0x07: // RLCA  
            RotateLeftC(mRegA);
            break;

        case 0x08: // LD (nn), SP
            mMmu.WriteU16(mMmu.ReadU16(mRegPC), mRegSP);
            mRegPC += 2;
            break;

        case 0x09: // ADD HL, BC
            AddReg(mRegHL.hl, mRegBC.bc);
            break;

        case 0x0A: // LD A, (BC)
            mRegA = mMmu.ReadU8(mRegBC.bc);
            break;

        case 0x0B: // DEC BC
            --mRegBC.bc;
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

        case 0x0F: // RRCA
            RotateRightC(mRegA);
            break;

        case 0x10: // STOP
            mHalted = true; // TODO: hacer que cuando se presione un boton se salga de este estado
            break;

        case 0x11: // LD DE, nn
            mRegDE.de = mMmu.ReadU16(mRegPC);
            mRegPC += 2;
            break;

        case 0x12: // LD (DE), A
            mMmu.WriteU8(mRegDE.de, mRegA);
            break;

        case 0x13: // INC DE
            ++mRegDE.de;
            break;

        case 0x14: // INC D
            IncReg(mRegDE.d);
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

        case 0x19: // ADD HL, DE
            AddReg(mRegHL.hl, mRegDE.de);
            break;

        case 0x1A: // LD A, (DE)
            mRegA = mMmu.ReadU8(mRegDE.de);
            break;

        case 0x1B: // DEC DE
            --mRegDE.de;
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

        case 0x1F: // RRA
            RotateRight(mRegA);
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

        case 0x25: // DEC H
            DecReg(mRegHL.h);
            break;

        case 0x26: // LD H, n
            mRegHL.h = mMmu.ReadU8(mRegPC++);
            break;

        case 0x27: // DAA
            //TODO: IMPLEMENTAR!!
            break;

        case 0x28: // JR Z, r8
        {
            i8 offset = (i8)mMmu.ReadU8(mRegPC++);

            if (mFlagZ)
                mRegPC += offset;

        }
        break;

        case 0x29: // ADD HL, HL
            AddReg(mRegHL.hl, mRegHL.hl);
            break;

        case 0x2A: // LD A, (HL+)
            mRegA = mMmu.ReadU8(mRegHL.hl);
            ++mRegHL.hl;
            break;

        case 0x2B: // DEC HL
            --mRegHL.hl;
            break;

        case 0x2C: // INC L
            IncReg(mRegHL.l);
            break;

        case 0x2D: // DEC L
            DecReg(mRegHL.l);
            break;

        case 0x2E: // LD L, n
            mRegHL.l = mMmu.ReadU8(mRegPC++);
            break;

        case 0x2F: // CPL
            mRegA = ~mRegA;
            mFlagN = true;
            mFlagH = true;
            break;

        case 0x30: // JR NC, n
        {
            i8 offset = (i8)mMmu.ReadU8(mRegPC++);

            if (!mFlagC)
                mRegPC += offset;
        }
        break;

        case 0x31: // LD SP, nn
            mRegSP = mMmu.ReadU16(mRegPC);
            mSPStartAddr = mRegSP;
			mRegPC += 2;
            break;

        case 0x32: // LD (HL-), A
			mMmu.WriteU8(mRegHL.hl--, mRegA);
	        break;

        case 0x33: // INC SP
            ++mRegSP;
            break;

        case 0x34: // INC (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            IncReg(temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0x35: // DEC (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            DecReg(temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0x36: // LD (HL), n
            mMmu.WriteU8(mRegHL.hl, mMmu.ReadU8(mRegPC++));
            break;

        case 0x37: // SCF
            mFlagC = true;
            mFlagN = false;
            mFlagH = false;
            break;

        case 0x38: // JR C, n
        {
            i8 offset = (i8)mMmu.ReadU8(mRegPC++);

            if (mFlagC)
                mRegPC += offset;
        }
        break;

        case 0x39: // ADD HL, SP
            AddReg(mRegHL.hl, mRegSP);
            break;

        case 0x3A: // LD A, (HL-)
            mRegA = mMmu.ReadU8(mRegHL.hl--);
            break;

        case 0x3B: // DEC SP
            --mRegSP;
            break;

        case 0x3C: // INC A
            IncReg(mRegA);
            break;

        case 0x3D: // DEC A
            DecReg(mRegA);
            break;

        case 0x3E: // LD A, n
            mRegA = mMmu.ReadU8(mRegPC++);
            break;

        case 0x3F: // CCF
            mFlagC = !mFlagC;
            mFlagN = false;
            mFlagH = false;
            break;

        case 0x40: // LD B, B
            break;

        case 0x41:
            mRegBC.b = mRegBC.c;
            break;

        case 0x42: // LD B, D
            mRegBC.b = mRegDE.d;
            break;

        case 0x43: // LD B, E
            mRegBC.b = mRegDE.e;
            break;

        case 0x44: // LD B, H
            mRegBC.b = mRegHL.h;
            break;

        case 0x45: // LD B, L
            mRegBC.b = mRegHL.l;
            break;

        case 0x46: // LD B, (HL)
            mRegBC.b = mMmu.ReadU8(mRegHL.hl);
            break;

        case 0x47: // LD B, A
            mRegBC.b = mRegA;
            break;

        case 0x48: // LD C, B
            mRegBC.c = mRegBC.b;
            break;

        case 0x49: // LD C, C
            break;

        case 0x4A: // LD C, D
            mRegBC.c = mRegDE.d;
            break;

        case 0x4B: // LD C, E
            mRegBC.c = mRegDE.e;
            break;

        case 0x4C: // LD C, H
            mRegBC.c = mRegHL.h;
            break;

        case 0x4D: // LD C, L
            mRegBC.c = mRegHL.l;
            break;

        case 0x4E: // LD C, (HL)
            mRegBC.c = mMmu.ReadU8(mRegHL.hl);
            break;

        case 0x4F: // LD C, A
            mRegBC.c = mRegA;
            break;

        case 0x50: // LD D, B
            mRegDE.d = mRegBC.b;
            break;

        case 0x51: // LD D, C
            mRegDE.d = mRegBC.c;
            break;

        case 0x52: // LD D, D
            break;

        case 0x53: // LD D, E
            mRegDE.d = mRegDE.e;
            break;

        case 0x54: // LD D, H
            mRegDE.d = mRegHL.h;
            break;

        case 0x55: // LD D, L
            mRegDE.d = mRegHL.l;
            break;

        case 0x56: // LD D, (HL)
            mRegDE.d = mMmu.ReadU8(mRegHL.hl);
            break;

        case 0x57: // LD D, A
            mRegDE.d = mRegA;
            break;

        case 0x58: // LD E, B
            mRegDE.e = mRegBC.b;
            break;

        case 0x59: // LD E, C
            mRegDE.e = mRegBC.c;
            break;

        case 0x5A: // LD E, D
            mRegDE.e = mRegDE.d;
            break;

        case 0x5B: // LD E, E
            break;

        case 0x5C: // LD E, H
            mRegDE.e = mRegHL.h;
            break;

        case 0x5D: // LD E, L
            mRegDE.e = mRegHL.l;
            break;

        case 0x5E: // LD E, (HL)
            mRegDE.e = mMmu.ReadU8(mRegHL.hl);
            break;

        case 0x5F: // LD E, A
            mRegDE.e = mRegA;
            break;

        case 0x60: // LD H, B
            mRegHL.h = mRegBC.b;
            break;

        case 0x61: // LD H, C
            mRegHL.h = mRegBC.c;
            break;

        case 0x62: // LD H, D
            mRegHL.h = mRegDE.d;
            break;

        case 0x63: // LD H, E
            mRegHL.h = mRegDE.e;
            break;

        case 0x64: // LD H, H
            break;

        case 0x65: // LD H, L
            mRegHL.h = mRegHL.l;
            break;

        case 0x66: // LD H, (HL)
            mRegHL.h = mMmu.ReadU8(mRegHL.hl);
            break;

        case 0x67: // LD H, A
            mRegHL.h = mRegA;
            break;

        case 0x68: // LD L, B
            mRegHL.l = mRegBC.b;
            break;

        case 0x69: // LD L, C
            mRegHL.l = mRegBC.c;
            break;

        case 0x6A: // LD L, D
            mRegHL.l = mRegDE.d;
            break;

        case 0x6B: // LD L, E
            mRegHL.l = mRegDE.e;
            break;

        case 0x6C: // LD L, H
            mRegHL.l = mRegHL.h;
            break;

        case 0x6D: // LD L, L
            break;

        case 0x6E: // LD L, (HL)
            mRegHL.l = mMmu.ReadU8(mRegHL.hl);
            break;

        case 0x6F: // LD L, A
            mRegHL.l = mRegA;
            break;

        case 0x70: // LD (HL), B
            mMmu.WriteU8(mRegHL.hl, mRegBC.b);
            break;

        case 0x71: // LD (HL), C
            mMmu.WriteU8(mRegHL.hl, mRegBC.c);
            break;

        case 0x72: // LD (HL), D
            mMmu.WriteU8(mRegHL.hl, mRegDE.d);
            break;

        case 0x73: // LD (HL), E
            mMmu.WriteU8(mRegHL.hl, mRegDE.e);
            break;

        case 0x74: // LD (HL), H
            mMmu.WriteU8(mRegHL.hl, mRegHL.h);
            break;

        case 0x75: // LD (HL), L
            mMmu.WriteU8(mRegHL.hl, mRegHL.l);
            break;

        case 0x76: // HALT
            //TODO: implementar
            break;

        case 0x77: // LD (HL), A
            mMmu.WriteU8(mRegHL.hl, mRegA);
            break;

        case 0x78: // LD A, B
            mRegA = mRegBC.b;
            break;

        case 0x79: // LD A, C
            mRegA = mRegBC.c;
            break;

        case 0x7A: // LD A, D
            mRegA = mRegDE.d;
            break;

        case 0x7B: // LD A, E
            mRegA = mRegDE.e;
            break;

        case 0x7C: // LD A, H
            mRegA = mRegHL.h;
            break;

        case 0x7D: // LD A, L
            mRegA = mRegHL.l;
            break;

        case 0x7E: // LD A, (HL)
            mRegA = mMmu.ReadU8(mRegHL.hl);
            break;

        case 0x7F: // LD A, A
            break;

        case 0x83: // ADD A, E
            AddRegA(mRegDE.e);
            break;

        case 0x86: // ADD A, (HL)
            AddRegA(mMmu.ReadU8(mRegHL.hl));
            break;

        case 0x88: // ADC A, B
            AddRegA(mRegBC.b + (mFlagC ? 1 : 0));
            break;

        case 0x89: // ADC A, C
            AddRegA(mRegBC.c + (mFlagC ? 1 : 0));
            break;

        case 0x8E: // ADC A, (HL)
            AddRegA(mMmu.ReadU8(mRegHL.hl) + (mFlagC ? 1 : 0));
            break;

        case 0x90: // SUB B
            SubRegA(mRegBC.b);
            break;

        case 0x99: // SBC A, C
            SubRegA(mRegBC.c + (mFlagC ? 1 : 0));
            break;

        case 0x9F: // SBC A, A
            SubRegA(mRegA + (mFlagC ? 1 : 0));
            break;

        case 0xA0: // AND B
            AndRegA(mRegBC.b);
            break;

        case 0xA5: // AND L
            AndRegA(mRegHL.l);
            break;

        case 0xAF: // XOR A
            mRegA = 0;
            mFlagZ = true;
            mFlagN = false;
            mFlagH = false;
            mFlagC = false;
            break;

        case 0xB1: // OR C
            mRegA = mRegBC.c | mRegA;
            mFlagZ = mRegA == 0;
            mFlagN = false;
            mFlagH = false;
            mFlagC = false;
            break;

        case 0xB9: // CP C
            CpRegA(mRegBC.c);
            break;

        case 0xBB: // CP E
            CpRegA(mRegDE.e);
            break;

        case 0xBE: // CP (HL)
            CpRegA(mMmu.ReadU8(mRegHL.hl));
            break;

        case 0xC1: // POP BC
            mRegBC.bc = Pop();
            break;

        case 0xC3: // JP nn
            mRegPC = mMmu.ReadU16(mRegPC);
            break;

        case 0xC5: // PUSH BC
            Push(mRegBC.bc);
            break;

        case 0xC9: // RET
            mRegPC = Pop();
            ManageEndInterrupt();
            break;

        case 0xCB: // CB
            numCycles = ProcessCb(mMmu.ReadU8(mRegPC++));
            break;

        case 0xCC: // CALL Z, nn
        {
            u16 dst = mMmu.ReadU16(mRegPC);
            mRegPC += 2;
            if (mFlagZ)
            {
                Push(mRegPC);
                mRegPC = dst;
            }
        }
        break;

        case 0xCD: // CALL nn
            Push(mRegPC + 2);
            mRegPC = mMmu.ReadU16(mRegPC);
            break;

        case 0xCE: // ADC A, n
            AddRegA(mMmu.ReadU8(mRegPC++) + (mFlagC ? 1 : 0));
            break;

        case 0xD5: // PUSH DE
            Push(mRegDE.de);
            break;

        case 0xD9: // RETI
            mRegPC = Pop();
            ManageEndInterrupt();
            mIME = true;
            break;

        case 0xE0: // LD (0xFF00 + n), A
            mMmu.WriteU8(0xFF00 + mMmu.ReadU8(mRegPC++), mRegA);
            break;

        case 0xE2: // LD (0xFF00 + C), A
            mMmu.WriteU8(0xFF00 + mRegBC.c, mRegA);
            break;

        case 0xE5: // PUSH HL
            Push(mRegHL.hl);
            break;

        case 0xE6: // AND n
            AndRegA(mMmu.ReadU8(mRegPC++));
            break;

        case 0xEA: // LD (nn), A
            mMmu.WriteU8(mMmu.ReadU16(mRegPC), mRegA);
            mRegPC += 2;
            break;

        case 0xF0: // LD A, (0xFF00 + n)
            mRegA = mMmu.ReadU8(0xFF00 + mMmu.ReadU8(mRegPC++));
            break;

        case 0xF3: // DI
            mDI = true;
            break;

        case 0xF5: // PUSH AF
            Push((mRegA << 8) | GetFlagsAsU8());
            break;

        case 0xFB: // EI
            mEI = true;
            break;

        case 0xFE: // CP n
            CpRegA(mMmu.ReadU8(mRegPC++));
            break;

        default:
            throw runtime_error("opcode unknown: " + Int2Hex(opcode));
            break;
    }

    if (mDI && (opcode != 0xF3))
    {
        mIME = false;
        mDI = false;
    }

    if (mEI && (opcode != 0xFB))
    {
        mIME = true;
        mEI = false;
    }

    // manage interrupts
    if (mIME)
    {
        u8 iflags = mMmu.ReadU8(IOReg::IF);

        if (iflags > 0)
        {
            u8 ienable = mMmu.ReadU8(IOReg::IE);
            u16 addresses[] = { Memory::VBlankInterruptAddr, Memory::LCDCStatusInterrupAddr, Memory::TimerOverflowInterrupAddr,
                Memory::SerialTransferInterrupAddr, Memory::HiLoP10P13InterrupAddr };

            for (int i = 0; i < 5; ++i)
            {
                ManageInterrupt(i, addresses[i], iflags, ienable);

                if (mMI)
                {
                    numCycles += 24;
                    break;
                }
            }
        }
    }

    return numCycles;
}

//--------------------------------------------
// --
//--------------------------------------------
u8 CPU::GetFlagsAsU8()
{
    u8 val = 0;

    if (mFlagZ)
        val |= 1 << 7;

    if (mFlagN)
        val |= 1 << 6;

    if (mFlagH)
        val |= 1 << 5;

    if (mFlagC)
        val |= 1 << 4;

    return val;
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::SetFlagsFromU8(u8 _val)
{
    mFlagZ = (_val & (1 << 7)) != 0;
    mFlagN = (_val & (1 << 6)) != 0;
    mFlagH = (_val & (1 << 5)) != 0;
    mFlagC = (_val & (1 << 4)) != 0;
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::ManageInterrupt(int _interruptBit, u16 _interruptAddr, u8 _iflags, u8 _ienable)
{
    int interrupt = (1 << _interruptBit);

    if ((_iflags & interrupt) && (_ienable & interrupt))
    {
        mIME = false;
        mMI = true;
        Push(mRegPC);
        mRegPC = _interruptAddr;
        mMmu.WriteU8(IOReg::IF, _iflags & ~interrupt);
        SaveRegisters();
    }
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::ManageEndInterrupt()
{
    if (mMI)
    {
        mMI = false;
        RestoreRegisters();
    }
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::SaveRegisters()
{
    mRegASaved = mRegA;
    mRegBCSaved = mRegBC;
    mRegDESaved = mRegDE;
    mRegHLSaved = mRegHL;
    mFlagZSaved = mFlagZ;
    mFlagNSaved = mFlagN;
    mFlagHSaved = mFlagH;
    mFlagCSaved = mFlagC;
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::RestoreRegisters()
{
    mRegA  = mRegASaved;
    mRegBC = mRegBCSaved;
    mRegDE = mRegDESaved;
    mRegHL = mRegHLSaved;
    mFlagZ = mFlagZSaved;
    mFlagN = mFlagNSaved;
    mFlagH = mFlagHSaved;
    mFlagC = mFlagCSaved;
}

//--------------------------------------------
// --
//--------------------------------------------
int CPU::ProcessCb(u8 _opcode)
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

    return OpcodesInfo::cb[_opcode].cyclesDuration;
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