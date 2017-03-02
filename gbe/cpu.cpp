#include <assert.h>
#include "base.h"
#include "mmu.h"
#include "opcodes_info.h"
#include "defines.h"
#include "timer.h"

#include "cpu.h"

static u16 sInterrupAddresses[] = { Memory::VBlankInterruptAddr, Memory::LCDCStatusInterrupAddr, Memory::TimerOverflowInterrupAddr,
                                    Memory::SerialTransferInterrupAddr, Memory::HiLoP10P13InterrupAddr };


//--------------------------------------------
// --
//--------------------------------------------
CPU::CPU(MMU &_mmu, Timer &_timer) : mMmu(_mmu), mTimer(_timer)
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
void CPU::Bit(u8 _bit, u8 _reg)
{
    mFlagZ = ((_reg & (1 << _bit)) == 0);
    mFlagN = false;
    mFlagH = true;
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::Res(u8 _bit, u8 &_reg)
{
    _reg &= ~(1 << _bit);
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::Set(u8 _bit, u8 &_reg)
{
    _reg |= (1 << _bit);
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::Sla(u8 &_reg)
{
    mFlagC = (_reg & 0b10000000) != 0;
    _reg = _reg << 1;

    mFlagN = false;
    mFlagH = false;
    mFlagZ = (_reg == 0);
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::Srl(u8 &_reg)
{
    mFlagC = (_reg & 1) != 0;
    _reg = _reg >> 1;

    mFlagN = false;
    mFlagH = false;
    mFlagZ = (_reg == 0);
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::Sra(u8 &_reg)
{
    mFlagC = (_reg & 1) != 0;
    u8 bit7 = _reg & 0b10000000;

    _reg = (_reg >> 1) | bit7;
 
    mFlagN = false;
    mFlagH = false;
    mFlagZ = (_reg == 0);
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::Swap(u8 &_reg)
{
    u8 lower = _reg & 0b00001111;
    u8 upper = _reg & 0b11110000;
    _reg = (lower << 4) | (upper >> 4);

    mFlagZ = _reg == 0;
    mFlagN = false;
    mFlagH = false;
    mFlagC = false;
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::IncReg(u8 &_reg)
{
    mFlagH = ((_reg & 0x0F) + 1) > 15;
    ++_reg;
    mFlagZ = (_reg == 0);
    mFlagN = false;
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::DecReg(u8 &_reg)
{
    mFlagH = ((_reg & 0x0F) - 1) < 0;
    --_reg;
    mFlagZ = (_reg == 0);
    mFlagN = true;
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::SubRegA(u8 _val)
{
    mFlagC = (mRegA - _val) < 0;
    mFlagH = ((mRegA & 0x0F) - (_val & 0x0F)) < 0;
    mRegA -= _val;
    mFlagZ = (mRegA == 0);
    mFlagN = true;
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::AddRegA(u8 _val)
{
    mFlagC = (mRegA + _val) > 255;
    mFlagH = ((mRegA & 0x0F) + (_val & 0x0F)) > 15;
    mRegA += _val;
    mFlagZ = (mRegA == 0);
    mFlagN = false;
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::CpRegA(u8 _val)
{
    mFlagZ = (mRegA == _val);
    mFlagN = true;
    mFlagH = (mRegA & 0x0F) < (_val & 0x0F);
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
void CPU::XorRegA(u8 _val)
{
    mRegA = mRegA ^ _val;
    mFlagZ = mRegA == 0;
    mFlagN = false;
    mFlagH = false;
    mFlagC = false;
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::OrRegA(u8 _val)
{
    mRegA = mRegA | _val;
    mFlagZ = mRegA == 0;
    mFlagN = false;
    mFlagH = false;
    mFlagC = false;
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::AddReg(u16 &_dest, u16 _orig)
{
    mFlagC = (_dest + _orig) > 0xFFFF;
    mFlagH = (((_dest + _orig) & 0xFF00) >> 8) > 15;
    _dest += _orig;
    mFlagN = false;
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
    mFlagZ = _reg == 0;
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::RotateLeftC(u8 &_reg)
{
    mFlagC = (_reg & 0b10000000) != 0;
    _reg = (_reg << 1) | (mFlagC ? 1 : 0);
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
    u8 cb = mFlagC ? 1 : 0;
    mFlagC = (_reg & 1) != 0;
    _reg = (_reg >> 1) | (cb << 7);
    mFlagZ = _reg == 0;
}

//--------------------------------------------
// --
//--------------------------------------------
void CPU::RotateRightC(u8 &_reg)
{
    mFlagC = (_reg & 1) != 0;
    _reg = (_reg >> 1) | ((mFlagC ? 1 : 0) << 7);
    mFlagZ = _reg == 0;
    mFlagN = false;
    mFlagH = false;
}

//--------------------------------------------
// --
//--------------------------------------------
int CPU::Step()
{
    if (mStopped)
        return 0;

    int numCycles = 4;

    // ...
    if (!mHalted)
    {
        u8 opcode = mMmu.ReadU8(mRegPC++);
        numCycles = OpcodesInfo::primary[opcode].cyclesDuration;

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
                mStopped = true;
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
                IncReg(mRegHL.h);
                break;

            case 0x25: // DEC H
                DecReg(mRegHL.h);
                break;

            case 0x26: // LD H, n
                mRegHL.h = mMmu.ReadU8(mRegPC++);
                break;

            case 0x27: // DAA
            {
                u8 lower = mRegA & 0x0F;
                u8 upper = (mRegA & 0xF0) >> 4;

                if (!mFlagN)
                {
                    if (!mFlagC)
                    {
                        if (!mFlagH)
                        {
                            if ((upper <= 8) && (lower >= 0xA))
                                mRegA += 0x06;
                            else if ((upper >= 0xA) && (lower <= 9))
                            {
                                mRegA += 0x06;
                                mFlagC = true;
                            }
                            else if ((upper >= 9) && (lower >= 0xA))
                            {
                                mRegA += 0x66;
                                mFlagC = true;
                            }
                        }
                        else 
                        {
                            if ((upper <= 9) && (lower <= 3))
                                mRegA += 0x06;
                            else if ((upper >= 0xA) && (lower <= 3))
                            {
                                mRegA += 0x66;
                                mFlagC = true;
                            }
                        }
                    }
                    else
                    {
                        if (!mFlagH && (upper <= 2) && (lower <= 9))
                            mRegA += 0x06;
                        else if (!mFlagH && (upper <= 2) && (lower >= 0xA))
                            mRegA += 0x66;
                        else if (mFlagH && (upper <= 3) && (lower <= 3))
                            mRegA += 0x66;
                    }
                }
                else 
                {
                    if (!mFlagC)
                    {
                        if (mFlagH && (upper <= 8) && (lower >= 6))
                            mRegA += 0xFA;
                    }
                    else 
                    {
                        if (!mFlagH && (upper >= 7) && (lower <= 9))
                        {
                            mRegA += 0xA0;
                            mFlagC = true;
                        }
                        else if (mFlagH && (upper >= 6) && (lower >= 6))
                        {
                            mRegA += 0x9A;
                            mFlagC = true;
                        }
                    }
                }
            }
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

            case 0x41: // LD B, C
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
                mHalted = true;
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

            case 0x80: // ADD A, B
                AddRegA(mRegBC.b);
                break;

            case 0x81: // ADD A, C
                AddRegA(mRegBC.c);
                break;

            case 0x82: // ADD A, D
                AddRegA(mRegDE.d);
                break;

            case 0x83: // ADD A, E
                AddRegA(mRegDE.e);
                break;

            case 0x84: // ADD A, H
                AddRegA(mRegHL.h);
                break;

            case 0x85: // ADD A, L
                AddRegA(mRegHL.l);
                break;

            case 0x86: // ADD A, (HL)
                AddRegA(mMmu.ReadU8(mRegHL.hl));
                break;

            case 0x87: // ADD A, A
                AddRegA(mRegA);
                break;

            case 0x88: // ADC A, B
                AddRegA(mRegBC.b + (mFlagC ? 1 : 0));
                break;

            case 0x89: // ADC A, C
                AddRegA(mRegBC.c + (mFlagC ? 1 : 0));
                break;

            case 0x8A: // ADC A, D
                AddRegA(mRegDE.d + (mFlagC ? 1 : 0));
                break;

            case 0x8B: // ADC A, E
                AddRegA(mRegDE.e + (mFlagC ? 1 : 0));
                break;

            case 0x8C: // ADC A, H
                AddRegA(mRegHL.h + (mFlagC ? 1 : 0));
                break;

            case 0x8D: // ADC A, L
                AddRegA(mRegHL.l + (mFlagC ? 1 : 0));
                break;

            case 0x8E: // ADC A, (HL)
                AddRegA(mMmu.ReadU8(mRegHL.hl) + (mFlagC ? 1 : 0));
                break;

            case 0x8F: // ADC A, A
                AddRegA(mRegA + (mFlagC ? 1 : 0));
                break;

            case 0x90: // SUB B
                SubRegA(mRegBC.b);
                break;

            case 0x91: // SUB C
                SubRegA(mRegBC.c);
                break;

            case 0x92: // SUB D
                SubRegA(mRegDE.d);
                break;

            case 0x93: // SUB E
                SubRegA(mRegDE.e);
                break;

            case 0x94: // SUB H
                SubRegA(mRegHL.h);
                break;

            case 0x95: // SUB L
                SubRegA(mRegHL.l);
                break;

            case 0x96: // SUB (HL)
                SubRegA(mMmu.ReadU8(mRegHL.hl));
                break;

            case 0x97: // SUB A
                SubRegA(mRegA);
                break;

            case 0x98: // SBC A, B
                SubRegA(mRegBC.b + (mFlagC ? 1 : 0));
                break;

            case 0x99: // SBC A, C
                SubRegA(mRegBC.c + (mFlagC ? 1 : 0));
                break;

            case 0x9A: // SBC A, D
                SubRegA(mRegDE.d + (mFlagC ? 1 : 0));
                break;

            case 0x9B: // SBC A, E
                SubRegA(mRegDE.e + (mFlagC ? 1 : 0));
                break;

            case 0x9C: // SBC A, H
                SubRegA(mRegHL.h + (mFlagC ? 1 : 0));
                break;

            case 0x9D: // SBC A, L
                SubRegA(mRegHL.l + (mFlagC ? 1 : 0));
                break;

            case 0x9E: // SBC A, (HL)
                SubRegA(mMmu.ReadU8(mRegHL.hl) + (mFlagC ? 1 : 0));
                break;

            case 0x9F: // SBC A, A
                SubRegA(mRegA + (mFlagC ? 1 : 0));
                break;

            case 0xA0: // AND B
                AndRegA(mRegBC.b);
                break;

            case 0xA1: // AND C
                AndRegA(mRegBC.c);
                break;

            case 0xA2: // AND D
                AndRegA(mRegDE.d);
                break;

            case 0xA3: // AND E
                AndRegA(mRegDE.e);
                break;

            case 0xA4: // AND H
                AndRegA(mRegHL.h);
                break;

            case 0xA5: // AND L
                AndRegA(mRegHL.l);
                break;

            case 0xA6: // AND (HL)
                AndRegA(mMmu.ReadU8(mRegHL.hl));
                break;

            case 0xA7: // AND A
                AndRegA(mRegA);
                break;

            case 0xA8: // XOR B
                XorRegA(mRegBC.b);
                break;

            case 0xA9: // XOR C
                XorRegA(mRegBC.c);
                break;

            case 0xAA: // XOR D
                XorRegA(mRegDE.d);
                break;

            case 0xAB: // XOR E
                XorRegA(mRegDE.e);
                break;

            case 0xAC: // XOR H
                XorRegA(mRegHL.h);
                break;

            case 0xAD: // XOR L
                XorRegA(mRegHL.l);
                break;

            case 0xAE: // XOR (HL)
                XorRegA(mMmu.ReadU8(mRegHL.hl));
                break;

            case 0xAF: // XOR A
                XorRegA(mRegA);
                break;

            case 0xB0: // OR B
                OrRegA(mRegBC.b);
                break;

            case 0xB1: // OR C
                OrRegA(mRegBC.c);
                break;

            case 0xB2: // OR D
                OrRegA(mRegDE.d);
                break;

            case 0xB3: // OR E
                OrRegA(mRegDE.e);
                break;

            case 0xB4: // OR H
                OrRegA(mRegHL.h);
                break;

            case 0xB5: // OR L
                OrRegA(mRegHL.l);
                break;

            case 0xB6: // OR (HL)
                OrRegA(mMmu.ReadU8(mRegHL.hl));
                break;

            case 0xB7: // OR A
                OrRegA(mRegA);
                break;

            case 0xB8: // CP B
                CpRegA(mRegBC.b);
                break;

            case 0xB9: // CP C
                CpRegA(mRegBC.c);
                break;

            case 0xBA: // CP D
                CpRegA(mRegDE.d);
                break;

            case 0xBB: // CP E
                CpRegA(mRegDE.e);
                break;

            case 0xBC: // CP H
                CpRegA(mRegHL.h);
                break;

            case 0xBD: // CP L
                CpRegA(mRegHL.l);
                break;

            case 0xBE: // CP (HL)
                CpRegA(mMmu.ReadU8(mRegHL.hl));
                break;

            case 0xBF: // CP A
                CpRegA(mRegA);
                break;

            case 0xC0: // RET NZ
                if (!mFlagZ)
                    mRegPC = Pop();
                break;

            case 0xC1: // POP BC
                mRegBC.bc = Pop();
                break;

            case 0xC2: // JP NZ, nn
            {
                u16 dst = mMmu.ReadU16(mRegPC);
                mRegPC += 2;

                if (!mFlagZ)
                    mRegPC = dst;
            }
            break;

            case 0xC3: // JP nn
                mRegPC = mMmu.ReadU16(mRegPC);
                break;

            case 0xC4: // CALL NZ, nn
            {
                u16 dst = mMmu.ReadU16(mRegPC);
                mRegPC += 2;
                if (!mFlagZ)
                {
                    Push(mRegPC);
                    mRegPC = dst;
                }
            }
            break;

            case 0xC5: // PUSH BC
                Push(mRegBC.bc);
                break;

            case 0xC6: // ADD A, n
                AddRegA(mMmu.ReadU8(mRegPC++));
                break;

            case 0xC7: // RST 00H
                Push(mRegPC);
                mRegPC = 0;
                break;

            case 0xC8: // RET Z
                if (mFlagZ)
                    mRegPC = Pop();
                break;

            case 0xC9: // RET
                mRegPC = Pop();
                break;

            case 0xCA: // JP Z, nn
            {
                u16 dst = mMmu.ReadU16(mRegPC);
                mRegPC += 2;

                if (mFlagZ)
                    mRegPC = dst;
            }
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

            case 0xCF: // RST 08H
                Push(mRegPC);
                mRegPC = 0x8;
                break;

            case 0xD0: // RET NC
                if (!mFlagC)
                    mRegPC = Pop();
                break;

            case 0xD1: // POP DE
                mRegDE.de = Pop();
                break;

            case 0xD2: // JP NC, nn
            {
                u16 dst = mMmu.ReadU16(mRegPC);
                mRegPC += 2;

                if (!mFlagC)
                    mRegPC = dst;
            }
            break;

            case 0xD4: // CALL NC, nn
            {
                u16 dst = mMmu.ReadU16(mRegPC);
                mRegPC += 2;
                if (!mFlagC)
                {
                    Push(mRegPC);
                    mRegPC = dst;
                }
            }
            break;

            case 0xD5: // PUSH DE
                Push(mRegDE.de);
                break;

            case 0xD6: // SUB n
                SubRegA(mMmu.ReadU8(mRegPC++));
                break;

            case 0xD7: // RST 10H
                Push(mRegPC);
                mRegPC = 0x10;
                break;

            case 0xD8: // RET C
                if (mFlagC)
                    mRegPC = Pop();
                break;

            case 0xD9: // RETI
                mRegPC = Pop();
                mIME = true;
                break;

            case 0xDA: // JP C, nn
            {
                u16 dst = mMmu.ReadU16(mRegPC);
                mRegPC += 2;

                if (mFlagC)
                    mRegPC = dst;
            }
            break;

            case 0xDC: // CALL C, nn
            {
                u16 dst = mMmu.ReadU16(mRegPC);
                mRegPC += 2;
                if (mFlagC)
                {
                    Push(mRegPC);
                    mRegPC = dst;
                }
            }
            break;

            case 0xDE: // SBC A, n
                SubRegA(mMmu.ReadU8(mRegPC++) + (mFlagC ? 1 : 0));
                break;

            case 0xDF: // RST 18H
                Push(mRegPC);
                mRegPC = 0x18;
                break;

            case 0xE0: // LD (0xFF00 + n), A
                mMmu.WriteU8(0xFF00 + mMmu.ReadU8(mRegPC++), mRegA);
                break;

            case 0xE1: // POP HL
                mRegHL.hl = Pop();
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

            case 0xE7: // RST 20H
                Push(mRegPC);
                mRegPC = 0x20;
                break;

            case 0xE8: // ADD SP, n
            {
                i8 val = (i8)mMmu.ReadU8(mRegPC++);
                mFlagC = (mRegSP + val) > 255;
                mFlagH = ((mRegSP & 0x0F) + (val & 0x0F)) > 15;
                mRegSP += val;
                mFlagZ = false;
                mFlagN = false;                
            }
            break;

            case 0xE9: // JP (HL)
                mRegPC = mRegHL.hl;
                break;

            case 0xEA: // LD (nn), A
                mMmu.WriteU8(mMmu.ReadU16(mRegPC), mRegA);
                mRegPC += 2;
                break;

            case 0xEE: // XOR n
                XorRegA(mMmu.ReadU8(mRegPC++));
                break;

            case 0xEF: // RST 28H
                Push(mRegPC);
                mRegPC = 0x28;
                break;

            case 0xF0: // LD A, (0xFF00 + n)
                mRegA = mMmu.ReadU8(0xFF00 + mMmu.ReadU8(mRegPC++));
                break;

            case 0xF1: // POP AF
            {
                u16 value = Pop();
                mRegA = (value & 0xFF00) >> 8;
                SetFlagsFromU8(value & 0x00FF);
            }
            break;

            case 0xF2: // LD A, (C)
                mRegA = mMmu.ReadU8(0xFF00 + mRegBC.c);
                break;

            case 0xF3: // DI
                mDI = true;
                break;

            case 0xF5: // PUSH AF
                Push((mRegA << 8) | GetFlagsAsU8());
                break;

            case 0xF6: // OR n
                OrRegA(mMmu.ReadU8(mRegPC++));
                break;

            case 0xF7: // RST 30H
                Push(mRegPC);
                mRegPC = 0x30;
                break;

            case 0xF8: // LD HL, SP + n
            {
                u8 val = mMmu.ReadU8(mRegPC++);
                mFlagC = (mRegSP + val) > 255;
                mFlagH = ((mRegSP & 0x0F) + (val & 0x0F)) > 15;
                mRegHL.hl = mRegSP + val;
                mFlagZ = false;
                mFlagN = false;                
            }
            break;

            case 0xF9: // LD SP, HL
                mRegSP = mRegHL.hl;
                break;

            case 0xFA: // LD A, nn
                mRegA = mMmu.ReadU8(mMmu.ReadU16(mRegPC));
                mRegPC += 2;
                break;

            case 0xFB: // EI
                mEI = true;
                break;

            case 0xFE: // CP n
                CpRegA(mMmu.ReadU8(mRegPC++));
                break;

            case 0xFF: // RST 38H
                Push(mRegPC);
                mRegPC = 0x38;
                break;

            default:
                cout << "opcode unknown: " << Int2Hex(opcode) << endl;
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
    }

    mTimer.Update(numCycles);

    if (mMmu.IsInBootRom() && (mRegPC >= 0x100))
    {
        mMmu.SetStateAfterBoot();
        SetStateAfterBoot();
        mTimer.Reset();
    }

    // manage interrupts
    if (mIME)
    {
        u8 iflags = mMmu.ReadU8(IOReg::IF);

        if (iflags > 0)
        {
            u8 ienable = mMmu.ReadU8(IOReg::IE);

            for (int i = 0; i < 5; ++i)
            {
                if (ManageInterrupt(i, sInterrupAddresses[i], iflags, ienable))
                {
                    numCycles += 24;
                    mHalted = false;
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
bool CPU::ManageInterrupt(int _interruptBit, u16 _interruptAddr, u8 _iflags, u8 _ienable)
{
    int interrupt = (1 << _interruptBit);

    if ((_iflags & interrupt) && (_ienable & interrupt))
    {
        mIME = false;
        Push(mRegPC);
        mRegPC = _interruptAddr;
        mMmu.WriteU8(IOReg::IF, _iflags & ~interrupt);
        return true;
    }

    return false;
}

//--------------------------------------------
// --
//--------------------------------------------
int CPU::ProcessCb(u8 _opcode)
{
    switch(_opcode)
    {
        case 0x00: // RLC B
            RotateLeftC(mRegBC.b);
            break;

        case 0x01: // RLC C
            RotateLeftC(mRegBC.c);
            break;

        case 0x02: // RLC D
            RotateLeftC(mRegDE.d);
            break;

        case 0x03: // RLC E
            RotateLeftC(mRegDE.e);
            break;

        case 0x04: // RLC H
            RotateLeftC(mRegHL.h);
            break;

        case 0x05: // RLC L
            RotateLeftC(mRegHL.l);
            break;

        case 0x06: // RLC (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            RotateLeftC(temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0x07: // RLC A
            RotateLeftC(mRegA);
            break;

        case 0x08: // RRC B
            RotateRightC(mRegBC.b);
            break;

        case 0x09: // RRC C
            RotateRightC(mRegBC.c);
            break;

        case 0x0A: // RRC D
            RotateRightC(mRegDE.d);
            break;

        case 0x0B: // RRC E
            RotateRightC(mRegDE.e);
            break;

        case 0x0C: // RRC H
            RotateRightC(mRegHL.h);
            break;

        case 0x0D: // RRC L
            RotateRightC(mRegHL.l);
            break;

        case 0x0E: // RRC (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            RotateRightC(temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0x0F: // RRC A
            RotateRightC(mRegA);
            break;

        case 0x10: // RL B
            RotateLeft(mRegBC.b);
            break;

        case 0x11: // RL C
            RotateLeft(mRegBC.c);
            break;

        case 0x12: // RL D
            RotateLeft(mRegDE.d);
            break;

        case 0x13: // RL E
            RotateLeft(mRegDE.e);
            break;

        case 0x14: // RL H
            RotateLeft(mRegHL.h);
            break;

        case 0x15: // RL L
            RotateLeft(mRegHL.l);
            break;

        case 0x16: // RL (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            RotateLeft(temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0x17: // RL A
            RotateLeft(mRegA);
            break;

        case 0x18: // RR B
            RotateRight(mRegBC.b);
            break;

        case 0x19: // RR C
            RotateRight(mRegBC.c);
            break;

        case 0x1A: // RR D
            RotateRight(mRegDE.d);
            break;

        case 0x1B: // RR E
            RotateRight(mRegDE.e);
            break;

        case 0x1C: // RR H
            RotateRight(mRegHL.h);
            break;

        case 0x1D: // RR L
            RotateRight(mRegHL.l);
            break;

        case 0x1E: // RR (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            RotateRight(temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0x1F: // RR A
            RotateRight(mRegA);
            break;

        case 0x20: // SLA B
            Sla(mRegBC.b);
            break;

        case 0x21: // SLA C
            Sla(mRegBC.c);
            break;

        case 0x22: // SLA D
            Sla(mRegDE.d);
            break;

        case 0x23: // SLA E
            Sla(mRegDE.e);
            break;

        case 0x24: // SLA H
            Sla(mRegHL.h);
            break;

        case 0x25: // SLA L
            Sla(mRegHL.l);
            break;

        case 0x26: // SLA (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Sla(temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0x27: // SLA A
            Sla(mRegA);
            break;

        case 0x28: // SRA B
            Sra(mRegBC.b);
            break;

        case 0x29: // SRA C
            Sra(mRegBC.c);
            break;

        case 0x2A: // SRA D
            Sra(mRegDE.d);
            break;

        case 0x2B: // SRA E
            Sra(mRegDE.e);
            break;

        case 0x2C: // SRA H
            Sra(mRegHL.h);
            break;

        case 0x2D: // SRA L
            Sra(mRegHL.l);
            break;

        case 0x2E: // SRA (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Sra(temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0x2F: // SRA A
            Sra(mRegA);
            break;

        case 0x30: // SWAP B
            Swap(mRegBC.b);
            break;

        case 0x31: // SWAP C
            Swap(mRegBC.c);
            break;

        case 0x32: // SWAP D
            Swap(mRegDE.d);
            break;

        case 0x33: // SWAP E
            Swap(mRegDE.e);
            break;

        case 0x34: // SWAP H
            Swap(mRegHL.h);
            break;

        case 0x35: // SWAP L
            Swap(mRegHL.l);
            break;

        case 0x36: // SWAP (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Swap(temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0x37: // SWAP A
            Swap(mRegA);
            break;

        case 0x38: // SRL B
            Srl(mRegBC.b);
            break;

        case 0x39: // SRL C
            Srl(mRegBC.c);
            break;

        case 0x3A: // SRL D
            Srl(mRegDE.d);
            break;

        case 0x3B: // SRL E
            Srl(mRegDE.e);
            break;

        case 0x3C: // SRL H
            Srl(mRegHL.h);
            break;

        case 0x3D: // SRL L
            Srl(mRegHL.l);
            break;

        case 0x3E: // SRL (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Srl(temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0x3F: // SRL A
            Srl(mRegA);
            break;

        case 0x40: // BIT 0, B
            Bit(0, mRegBC.b);
            break;

        case 0x41: // BIT 0, C
            Bit(0, mRegBC.c);
            break;

        case 0x42: // BIT 0, D
            Bit(0, mRegDE.d);
            break;

        case 0x43: // BIT 0, E
            Bit(0, mRegDE.e);
            break;

        case 0x44: // BIT 0, H
            Bit(0, mRegHL.h);
            break;

        case 0x45: // BIT 0, L
            Bit(0, mRegHL.l);
            break;

        case 0x46: // BIT 0, (HL)
            Bit(0, mMmu.ReadU8(mRegHL.hl));
            break;

        case 0x47: // BIT 0, A
            Bit(0, mRegA);
            break;

        case 0x48: // BIT 1, B
            Bit(1, mRegBC.b);
            break;

        case 0x49: // BIT 1, C
            Bit(1, mRegBC.c);
            break;

        case 0x4A: // BIT 1, D
            Bit(1, mRegDE.d);
            break;

        case 0x4B: // BIT 1, E
            Bit(1, mRegDE.e);
            break;

        case 0x4C: // BIT 1, H
            Bit(1, mRegHL.h);
            break;

        case 0x4D: // BIT 1, L
            Bit(1, mRegHL.l);
            break;

        case 0x4E: // BIT 1, (HL)
            Bit(1, mMmu.ReadU8(mRegHL.hl));
            break;

        case 0x4F: // BIT 1, A
            Bit(1, mRegA);
            break;

        case 0x50: // BIT 2, B
            Bit(2, mRegBC.b);
            break;

        case 0x51: // BIT 2, C
            Bit(2, mRegBC.c);
            break;

        case 0x52: // BIT 2, D
            Bit(2, mRegDE.d);
            break;

        case 0x53: // BIT 2, E
            Bit(2, mRegDE.e);
            break;

        case 0x54: // BIT 2, H
            Bit(2, mRegHL.h);
            break;

        case 0x55: // BIT 2, L
            Bit(2, mRegHL.l);
            break;

        case 0x56: // BIT 2, (HL)
            Bit(2, mMmu.ReadU8(mRegHL.hl));
            break;

        case 0x57: // BIT 2, A
            Bit(2, mRegA);
            break;

        case 0x58: // BIT 3, B
            Bit(3, mRegBC.b);
            break;

        case 0x59: // BIT 3, C
            Bit(3, mRegBC.c);
            break;

        case 0x5A: // BIT 3, D
            Bit(3, mRegDE.d);
            break;

        case 0x5B: // BIT 3, E
            Bit(3, mRegDE.e);
            break;

        case 0x5C: // BIT 3, H
            Bit(3, mRegHL.h);
            break;

        case 0x5D: // BIT 3, L
            Bit(3, mRegHL.l);
            break;

        case 0x5E: // BIT 3, (HL)
            Bit(3, mMmu.ReadU8(mRegHL.hl));
            break;

        case 0x5F: // BIT 3, A
            Bit(3, mRegA);
            break;

        case 0x60: // BIT 4, B
            Bit(4, mRegBC.b);
            break;

        case 0x61: // BIT 4, C
            Bit(4, mRegBC.c);
            break;

        case 0x62: // BIT 4, D
            Bit(4, mRegDE.d);
            break;

        case 0x63: // BIT 4, E
            Bit(4, mRegDE.e);
            break;

        case 0x64: // BIT 4, H
            Bit(4, mRegHL.h);
            break;

        case 0x65: // BIT 4, L
            Bit(4, mRegHL.l);
            break;

        case 0x66: // BIT 4, (HL)
            Bit(4, mMmu.ReadU8(mRegHL.hl));
            break;

        case 0x67: // BIT 4, A
            Bit(4, mRegA);
            break;

        case 0x68: // BIT 5, B
            Bit(5, mRegBC.b);
            break;

        case 0x69: // BIT 5, C
            Bit(5, mRegBC.c);
            break;

        case 0x6A: // BIT 5, D
            Bit(5, mRegDE.d);
            break;

        case 0x6B: // BIT 5, E
            Bit(5, mRegDE.e);
            break;

        case 0x6C: // BIT 5, H
            Bit(5, mRegHL.h);
            break;

        case 0x6D: // BIT 5, L
            Bit(5, mRegHL.l);
            break;

        case 0x6E: // BIT 5, (HL)
            Bit(5, mMmu.ReadU8(mRegHL.hl));
            break;

        case 0x6F: // BIT 5, A
            Bit(5, mRegA);
            break;

        case 0x70: // BIT 6, B
            Bit(6, mRegBC.b);
            break;

        case 0x71: // BIT 6, C
            Bit(6, mRegBC.c);
            break;

        case 0x72: // BIT 6, D
            Bit(6, mRegDE.d);
            break;

        case 0x73: // BIT 6, E
            Bit(6, mRegDE.e);
            break;

        case 0x74: // BIT 6, H
            Bit(6, mRegHL.h);
            break;

        case 0x75: // BIT 6, L
            Bit(6, mRegHL.l);
            break;

        case 0x76: // BIT 6, (HL)
            Bit(6, mMmu.ReadU8(mRegHL.hl));
            break;

        case 0x77: // BIT 6, A
            Bit(6, mRegA);
            break;

        case 0x78: // BIT 7, B
            Bit(7, mRegBC.b);
            break;

        case 0x79: // BIT 7, C
            Bit(7, mRegBC.c);
            break;

        case 0x7A: // BIT 7, D
            Bit(7, mRegDE.d);
            break;

        case 0x7B: // BIT 7, E
            Bit(7, mRegDE.e);
            break;

        case 0x7C: // BIT 7, H
            Bit(7, mRegHL.h);
            break;

        case 0x7D: // BIT 7, L
            Bit(7, mRegHL.l);
            break;

        case 0x7E: // BIT 7, (HL)
            Bit(7, mMmu.ReadU8(mRegHL.hl));
            break;

        case 0x7F: // BIT 7, A
            Bit(7, mRegA);
            break;

        case 0x80: // RES 0, B
            Res(0, mRegBC.b);
            break;

        case 0x81: // RES 0, C
            Res(0, mRegBC.c);
            break;

        case 0x82: // RES 0, D
            Res(0, mRegDE.d);
            break;

        case 0x83: // RES 0, E
            Res(0, mRegDE.e);
            break;

        case 0x84: // RES 0, H
            Res(0, mRegHL.h);
            break;

        case 0x85: // RES 0, L
            Res(0, mRegHL.l);
            break;

        case 0x86: // RES 0, (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Res(0, temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0x87: // RES 0, A
            Res(0, mRegA);
            break;

        case 0x88: // RES 1, B
            Res(1, mRegBC.b);
            break;

        case 0x89: // RES 1, C
            Res(1, mRegBC.c);
            break;

        case 0x8A: // RES 1, D
            Res(1, mRegDE.d);
            break;

        case 0x8B: // RES 1, E
            Res(1, mRegDE.e);
            break;

        case 0x8C: // RES 1, H
            Res(1, mRegHL.h);
            break;

        case 0x8D: // RES 1, L
            Res(1, mRegHL.l);
            break;

        case 0x8E: // RES 1, (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Res(1, temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0x8F: // RES 1, A
            Res(1, mRegA);
            break;

        case 0x90: // RES 2, B
            Res(2, mRegBC.b);
            break;

        case 0x91: // RES 2, C
            Res(2, mRegBC.c);
            break;

        case 0x92: // RES 2, D
            Res(2, mRegDE.d);
            break;

        case 0x93: // RES 2, E
            Res(2, mRegDE.e);
            break;

        case 0x94: // RES 2, H
            Res(2, mRegHL.h);
            break;

        case 0x95: // RES 2, L
            Res(2, mRegHL.l);
            break;

        case 0x96: // RES 2, (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Res(2, temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0x97: // RES 2, A
            Res(2, mRegA);
            break;

        case 0x98: // RES 3, B
            Res(3, mRegBC.b);
            break;

        case 0x99: // RES 3, C
            Res(3, mRegBC.c);
            break;

        case 0x9A: // RES 3, D
            Res(3, mRegDE.d);
            break;

        case 0x9B: // RES 3, E
            Res(3, mRegDE.e);
            break;

        case 0x9C: // RES 3, H
            Res(3, mRegHL.h);
            break;

        case 0x9D: // RES 3, L
            Res(3, mRegHL.l);
            break;

        case 0x9E: // RES 3, (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Res(3, temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0x9F: // RES 3, A
            Res(3, mRegA);
            break;

        case 0xA0: // RES 4, B
            Res(4, mRegBC.b);
            break;

        case 0xA1: // RES 4, C
            Res(4, mRegBC.c);
            break;

        case 0xA2: // RES 4, D
            Res(4, mRegDE.d);
            break;

        case 0xA3: // RES 4, E
            Res(4, mRegDE.e);
            break;

        case 0xA4: // RES 4, H
            Res(4, mRegHL.h);
            break;

        case 0xA5: // RES 4, L
            Res(4, mRegHL.l);
            break;

        case 0xA6: // RES 4, (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Res(4, temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0xA7: // RES 4, A
            Res(4, mRegA);
            break;

        case 0xA8: // RES 5, B
            Res(5, mRegBC.b);
            break;

        case 0xA9: // RES 5, C
            Res(5, mRegBC.c);
            break;

        case 0xAA: // RES 5, D
            Res(5, mRegDE.d);
            break;

        case 0xAB: // RES 5, E
            Res(5, mRegDE.e);
            break;

        case 0xAC: // RES 5, H
            Res(5, mRegHL.h);
            break;

        case 0xAD: // RES 5, L
            Res(5, mRegHL.l);
            break;

        case 0xAE: // RES 5, (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Res(5, temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0xAF: // RES 5, A
            Res(5, mRegA);
            break;

        case 0xB0: // RES 6, B
            Res(6, mRegBC.b);
            break;

        case 0xB1: // RES 6, C
            Res(6, mRegBC.c);
            break;

        case 0xB2: // RES 6, D
            Res(6, mRegDE.d);
            break;

        case 0xB3: // RES 6, E
            Res(6, mRegDE.e);
            break;

        case 0xB4: // RES 6, H
            Res(6, mRegHL.h);
            break;

        case 0xB5: // RES 6, L
            Res(6, mRegHL.l);
            break;

        case 0xB6: // RES 6, (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Res(6, temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0xB7: // RES 6, A
            Res(6, mRegA);
            break;

        case 0xB8: // RES 7, B
            Res(7, mRegBC.b);
            break;

        case 0xB9: // RES 7, C
            Res(7, mRegBC.c);
            break;

        case 0xBA: // RES 7, D
            Res(7, mRegDE.d);
            break;

        case 0xBB: // RES 7, E
            Res(7, mRegDE.e);
            break;

        case 0xBC: // RES 7, H
            Res(7, mRegHL.h);
            break;

        case 0xBD: // RES 7, L
            Res(7, mRegHL.l);
            break;

        case 0xBE: // RES 7, (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Res(7, temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0xBF: // RES 7, A
            Res(7, mRegA);
            break;

        case 0xC0: // SET 0, B
            Set(0, mRegBC.b);
            break;

        case 0xC1: // SET 0, C
            Set(0, mRegBC.c);
            break;

        case 0xC2: // SET 0, D
            Set(0, mRegDE.d);
            break;

        case 0xC3: // SET 0, E
            Set(0, mRegDE.e);
            break;

        case 0xC4: // SET 0, H
            Set(0, mRegHL.h);
            break;

        case 0xC5: // SET 0, L
            Set(0, mRegHL.l);
            break;

        case 0xC6: // SET 0, (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Set(0, temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0xC7: // SET 0, A
            Set(0, mRegA);
            break;

        case 0xC8: // SET 1, B
            Set(1, mRegBC.b);
            break;

        case 0xC9: // SET 1, C
            Set(1, mRegBC.c);
            break;

        case 0xCA: // SET 1, D
            Set(1, mRegDE.d);
            break;

        case 0xCB: // SET 1, E
            Set(1, mRegDE.e);
            break;

        case 0xCC: // SET 1, H
            Set(1, mRegHL.h);
            break;

        case 0xCD: // SET 1, L
            Set(1, mRegHL.l);
            break;

        case 0xCE: // SET 1, (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Set(1, temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0xCF: // SET 1, A
            Set(1, mRegA);
            break;

        case 0xD0: // SET 2, B
            Set(2, mRegBC.b);
            break;

        case 0xD1: // SET 2, C
            Set(2, mRegBC.c);
            break;

        case 0xD2: // SET 2, D
            Set(2, mRegDE.d);
            break;

        case 0xD3: // SET 2, E
            Set(2, mRegDE.e);
            break;

        case 0xD4: // SET 2, H
            Set(2, mRegHL.h);
            break;

        case 0xD5: // SET 2, L
            Set(2, mRegHL.l);
            break;

        case 0xD6: // SET 2, (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Set(2, temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0xD7: // SET 2, A
            Set(2, mRegA);
            break;

        case 0xD8: // SET 3, B
            Set(3, mRegBC.b);
            break;

        case 0xD9: // SET 3, C
            Set(3, mRegBC.c);
            break;

        case 0xDA: // SET 3, D
            Set(3, mRegDE.d);
            break;

        case 0xDB: // SET 3, E
            Set(3, mRegDE.e);
            break;

        case 0xDC: // SET 3, H
            Set(3, mRegHL.h);
            break;

        case 0xDD: // SET 3, L
            Set(3, mRegHL.l);
            break;

        case 0xDE: // SET 3, (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Set(3, temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0xDF: // SET 3, A
            Set(3, mRegA);
            break;

        case 0xE0: // SET 4, B
            Set(4, mRegBC.b);
            break;

        case 0xE1: // SET 4, C
            Set(4, mRegBC.c);
            break;

        case 0xE2: // SET 4, D
            Set(4, mRegDE.d);
            break;

        case 0xE3: // SET 4, E
            Set(4, mRegDE.e);
            break;

        case 0xE4: // SET 4, H
            Set(4, mRegHL.h);
            break;

        case 0xE5: // SET 4, L
            Set(4, mRegHL.l);
            break;

        case 0xE6: // SET 4, (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Set(4, temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0xE7: // SET 4, A
            Set(4, mRegA);
            break;

        case 0xE8: // SET 5, B
            Set(5, mRegBC.b);
            break;

        case 0xE9: // SET 5, C
            Set(5, mRegBC.c);
            break;

        case 0xEA: // SET 5, D
            Set(5, mRegDE.d);
            break;

        case 0xEB: // SET 5, E
            Set(5, mRegDE.e);
            break;

        case 0xEC: // SET 5, H
            Set(5, mRegHL.h);
            break;

        case 0xED: // SET 5, L
            Set(5, mRegHL.l);
            break;

        case 0xEE: // SET 5, (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Set(5, temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0xEF: // SET 5, A
            Set(5, mRegA);
            break;

        case 0xF0: // SET 6, B
            Set(6, mRegBC.b);
            break;

        case 0xF1: // SET 6, C
            Set(6, mRegBC.c);
            break;

        case 0xF2: // SET 6, D
            Set(6, mRegDE.d);
            break;

        case 0xF3: // SET 6, E
            Set(6, mRegDE.e);
            break;

        case 0xF4: // SET 6, H
            Set(6, mRegHL.h);
            break;

        case 0xF5: // SET 6, L
            Set(6, mRegHL.l);
            break;

        case 0xF6: // SET 6, (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Set(6, temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0xF7: // SET 6, A
            Set(4, mRegA);
            break;

        case 0xF8: // SET 7, B
            Set(7, mRegBC.b);
            break;

        case 0xF9: // SET 7, C
            Set(7, mRegBC.c);
            break;

        case 0xFA: // SET 7, D
            Set(7, mRegDE.d);
            break;

        case 0xFB: // SET 7, E
            Set(7, mRegDE.e);
            break;

        case 0xFC: // SET 7, H
            Set(7, mRegHL.h);
            break;

        case 0xFD: // SET 7, L
            Set(7, mRegHL.l);
            break;

        case 0xFE: // SET 7, (HL)
        {
            u8 temp = mMmu.ReadU8(mRegHL.hl);
            Set(7, temp);
            mMmu.WriteU8(mRegHL.hl, temp);
        }
        break;

        case 0xFF: // SET 7, A
            Set(7, mRegA);
            break;

        default:
            cout << "opcode unknown: " << Int2Hex(_opcode) << endl;
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

//--------------------------------------------
// --
//--------------------------------------------
void CPU::RequestInterrupt(int _interrupt)
{
    mMmu.WriteU8(IOReg::IF, mMmu.ReadU8(IOReg::IF) | _interrupt);
}