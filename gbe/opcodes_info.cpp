#include "base.h"
#include "gb.h"
#include "opcodes_info.h"

OpcodesInfo::InfoPrimary OpcodesInfo::primary[256] =
{
    // 0x00
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "NOP"; } },
    { 3, 12, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD BC, " + Int2Hex(_gb.GetMmu().ReadU16(_pc)); } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD (BC), A"; } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "INC BC"; } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "INC B"; } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "DEC B"; } },
    { 2, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD B, " + Int2Hex(_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "RLCA"; } },
    { 3, 20, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD (" + Int2Hex(_gb.GetMmu().ReadU16(_pc)) + "), SP"; } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADD HL, BC"; } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD A, (BC)"; } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "DEC BC"; } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "INC C"; } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "DEC C"; } },
    { 2, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD C, " + Int2Hex(_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "RRCA"; } },

    // 0x10
    { 2, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "STOP"; } },
    { 3, 12, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD DE, " + Int2Hex(_gb.GetMmu().ReadU16(_pc)); } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD (DE), A"; } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "INC DE"; } },
    { 1, 0,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "INC D"; } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "DEC D"; } },
    { 2, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD D, " + Int2Hex(_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "RLA"; } },
    { 2, 12, OpcodesInfo::JumpI8, [](GB &_gb, u16 _pc) { return "JR :" + Int2Hex(_pc + 1 + (i8)_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADD HL, DE"; } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD A, (DE)"; } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "DEC DE"; } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "INC E"; } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "DEC E"; } },
    { 2, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD E, " + Int2Hex(_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "RRA"; } },

    // 0x20
    { 2, 8,  OpcodesInfo::JumpCondI8, [](GB &_gb, u16 _pc) { return "JR NZ, :" + Int2Hex(_pc + 1 + (i8)_gb.GetMmu().ReadU8(_pc)); } },
    { 3, 12, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD HL, " + Int2Hex(_gb.GetMmu().ReadU16(_pc)); } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD (HL+), A"; } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "INC HL"; } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "INC H"; } },
    { 1, 0,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "DEC H"; } },
    { 2, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD H, " + Int2Hex(_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "DAA"; } },
    { 2, 8,  OpcodesInfo::JumpCondI8, [](GB &_gb, u16 _pc) { return "JR Z, :" + Int2Hex(_pc + 1 + (i8)_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADD HL, HL"; } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD A, (HL+)"; } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "DEC HL"; } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "INC L"; } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "DEC L"; } },
    { 2, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD L, " + Int2Hex(_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "CPL"; } },

    // 0x30
    { 1, 8,  OpcodesInfo::JumpCondI8, [](GB &_gb, u16 _pc) { return "JR NC, :" + Int2Hex(_pc + 1 + (i8)_gb.GetMmu().ReadU8(_pc)); } },
    { 3, 12, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD SP, " + Int2Hex(_gb.GetMmu().ReadU16(_pc)); } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD (HL-), A"; } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "INC SP"; } },
    { 1, 12, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "INC (HL)"; } },
    { 1, 12, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "DEC (HL)"; } },
    { 2, 12, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD (HL), " + Int2Hex(_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "SCF"; } },
    { 1, 8,  OpcodesInfo::JumpCondI8, [](GB &_gb, u16 _pc) { return "JR C, :" + Int2Hex(_pc + 1 + (i8)_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 0,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADD HL, SP"; } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD A, (HL-)"; } },
    { 1, 0,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "DEC SP"; } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "INC A"; } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "DEC A"; } },
    { 2, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD A, " + Int2Hex(_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "CCF"; } },

    // 0x40
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD B, B"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD B, C"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD B, D"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD B, E"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD B, H"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD B, L"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD B, (HL)"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD B, A"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD C, B"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD C, C"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD C, D"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD C, E"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD C, H"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD C, L"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD C, (HL)"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD C, A"; } },

    // 0x50
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD D, B"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD D, C"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD D, D"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD D, E"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD D, H"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD D, L"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD D, (HL)"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD D, A"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD E, B"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD E, C"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD E, D"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD E, E"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD E, H"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD E, L"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD E, (HL)"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD E, A"; } },

    // 0x60
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD H, B"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD H, C"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD H, D"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD H, E"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD H, H"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD H, L"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD H, (HL)"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD H, A"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD L, B"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD L, C"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD L, D"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD L, E"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD L, H"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD L, L"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD L, (HL)"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD L, A"; } },

    // 0x70
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD (HL), B"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD (HL), C"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD (HL), D"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD (HL), E"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD (HL), H"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD (HL), L"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "HALT"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD (HL), A"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD A, B"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD A, C"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD A, D"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD A, E"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD A, H"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD A, L"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD A, (HL)"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD A, A"; } },

    // 0x80
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADD A, B"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADD A, C"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADD A, D"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADD A, E"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADD A, H"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADD A, L"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADD A, (HL)"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADD A, A"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADC A, B"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADC A, C"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADC A, D"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADC A, E"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADC A, H"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADC A, L"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADC A, (HL)"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADC A, A"; } },

    // 0x90
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "SUB B"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "SUB C"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "SUB D"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "SUB E"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "SUB H"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "SUB L"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "SUB (HL)"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "SUB A"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "SBC A, B"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "SBC A, C"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "SBC A, D"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "SBC A, E"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "SBC A, H"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "SBC A, L"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "SBC A, (HL)"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "SBC A, A"; } },

    // 0xA0
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "AND B"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "AND C"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "AND D"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "AND E"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "AND H"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "AND L"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "AND (HL)"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "AND A"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "XOR B"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "XOR C"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "XOR D"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "XOR E"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "XOR H"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "XOR L"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "XOR (HL)"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "XOR A"; } },

    // 0xB0
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "OR B"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "OR C"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "OR D"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "OR E"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "OR H"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "OR L"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "OR (HL)"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "OR A"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "CP B"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "CP C"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "CP D"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "CP E"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "CP H"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "CP L"; } },
    { 1, 8, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "CP (HL)"; } },
    { 1, 4, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "CP A"; } },

    // 0xC0
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "RET NZ"; } },
    { 1, 12, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "POP BC"; } },
    { 3, 12, OpcodesInfo::JumpCondU16, [](GB &_gb, u16 _pc) { return "JP NZ, " + Int2Hex(_gb.GetMmu().ReadU16(_pc)); } },
    { 3, 16, OpcodesInfo::JumpU16, [](GB &_gb, u16 _pc) { return "JP " + Int2Hex(_gb.GetMmu().ReadU16(_pc)); } },
    { 3, 12, OpcodesInfo::CallCond, [](GB &_gb, u16 _pc) { return "CALL NZ, " + Int2Hex(_gb.GetMmu().ReadU16(_pc)); } },
    { 1, 16, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "PUSH BC"; } },
    { 2, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADD A, " + Int2Hex(_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 16, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "RST 00H"; } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "RET Z"; } },
    { 1, 16, OpcodesInfo::Ret,  [](GB &_gb, u16 _pc) { return "RET"; } },
    { 3, 12, OpcodesInfo::JumpCondU16, [](GB &_gb, u16 _pc) { return "JP Z, " + Int2Hex(_gb.GetMmu().ReadU16(_pc)); } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "CB"; } },
    { 3, 12, OpcodesInfo::CallCond, [](GB &_gb, u16 _pc) { return "CALL Z, " + Int2Hex(_gb.GetMmu().ReadU16(_pc)); } },
    { 3, 14, OpcodesInfo::Call, [](GB &_gb, u16 _pc) { return "CALL " + Int2Hex(_gb.GetMmu().ReadU16(_pc)); } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADC A, " + Int2Hex(_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 16, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "RST 08H"; } },

    // 0xD0
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "RET NC"; } },
    { 1, 12, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "POP DE"; } },
    { 3, 12, OpcodesInfo::JumpCondU16, [](GB &_gb, u16 _pc) { return "JP NC, " + Int2Hex(_gb.GetMmu().ReadU16(_pc)); } },
    { 1, 0,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "??"; } },
    { 3, 12, OpcodesInfo::CallCond, [](GB &_gb, u16 _pc) { return "CALL NC, " + Int2Hex(_gb.GetMmu().ReadU16(_pc)); } },
    { 1, 16, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "PUSH DE"; } },
    { 2, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "SUB " + Int2Hex(_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 16, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "RST 10H"; } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "RET C"; } },
    { 1, 16, OpcodesInfo::Ret,  [](GB &_gb, u16 _pc) { return "RETI"; } },
    { 3, 12, OpcodesInfo::JumpCondU16, [](GB &_gb, u16 _pc) { return "JP C, " + Int2Hex(_gb.GetMmu().ReadU16(_pc)); } },
    { 1, 0,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "??"; } },
    { 3, 12, OpcodesInfo::CallCond, [](GB &_gb, u16 _pc) { return "CALL C, " + Int2Hex(_gb.GetMmu().ReadU16(_pc)); } },
    { 1, 0,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "??"; } },
    { 2, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "SBC A, " + Int2Hex(_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 16, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "RST 18H"; } },

    // 0xE0
    { 2, 12, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD (FF00 + " + Int2Hex(_gb.GetMmu().ReadU8(_pc)) + "), A"; } },
    { 1, 12, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "POP HL"; } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD (FF00 + C), A"; } },
    { 1, 0,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "??"; } },
    { 1, 0,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "??"; } },
    { 1, 16, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "PUSH HL"; } },
    { 2, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "AND " + Int2Hex(_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 16, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "RST 20H"; } },
    { 2, 16, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "ADD SP, " + Int2Hex(_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "JP (HL)"; } },
    { 3, 16, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD (" + Int2Hex(_gb.GetMmu().ReadU16(_pc)) + "), A"; } },
    { 1, 0,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "??"; } },
    { 1, 0,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "??"; } },
    { 1, 0,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "??"; } },
    { 2, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "XOR " + Int2Hex(_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 16, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "RST 28H"; } },

    // 0xF0
    { 2, 12, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD A, (FF00 + " + Int2Hex(_gb.GetMmu().ReadU8(_pc)) + ")"; } },
    { 1, 12, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "POP AF"; } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD A, (C)"; } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "DI"; } },
    { 1, 0,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "??"; } },
    { 1, 16, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "PUSH AF"; } },
    { 2, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "OR " + Int2Hex(_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 16, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "RST 30H"; } },
    { 2, 12, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD HL, SP + " + Int2Hex(_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD SP, HL"; } },
    { 3, 16, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "LD A, " + Int2Hex(_gb.GetMmu().ReadU16(_pc)); } },
    { 1, 4,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "EI"; } },
    { 1, 0,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "??"; } },
    { 1, 0,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "??"; } },
    { 2, 8,  OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "CP " + Int2Hex(_gb.GetMmu().ReadU8(_pc)); } },
    { 1, 16, OpcodesInfo::None, [](GB &_gb, u16 _pc) { return "RST 38H"; } }
};

