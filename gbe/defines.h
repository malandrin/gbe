#pragma once

namespace Screen
{
    static const u8 Width = 160;
    static const u8 Height = 144;
    static const u8 TotalHeight = Height + 10; // 10 = hidden lines
}

namespace Cycles
{
    static const int PerFrame = 70224;
    static const int PerHBlank = 204;
    static const int PerScanline = 456;
    static const int PerOAM = 80;
    static const int PerVRAM = 172;
}

namespace Memory
{
    static const u16 VBlankInterruptAddr = 0x0040;
    static const u16 LCDCStatusInterrupAddr = 0x0048;
    static const u16 TimerOverflowInterrupAddr = 0x0050;
    static const u16 SerialTransferInterrupAddr = 0x0058;
    static const u16 HiLoP10P13InterrupAddr = 0x0060;

    static const u16 BootRomStartAddr = 0x0000;
    static const u16 BootRomEndAddr = 0x0100;

    static const u16 RomStartAddr = 0x0000;
    static const u16 RomEndAddr = 0x3FFF;

    static const u16 RomBankNumberStartAddr = 0x2000;
    static const u16 RomBankNumberEndAddr = 0x3FFF;

    static const u16 RomBankNStartAddr = 0x4000;
    static const u16 RomBankNEndAddr = 0x7FFF;

    static const u16 RamBankNumberStartAddr = 0x4000;
    static const u16 RamBankNumberEndAddr = 0x5FFF;

    static const u16 RomRamModeSelectStartAddr = 0x6000;
    static const u16 RomRamModeSelectEndAddr = 0x7FFF;

    static const u16 VRamStartAddr = 0x8000;
    static const u16 VRamEndAddr = 0x9FFF;

    static const u16 VRamTileData1StartAddr = 0x8000;
    static const u16 VRamTileData2StartAddr = 0x8800;

    static const u16 VRamTileMap1StartAddr = 0x9800;
    static const u16 VRamTileMap2StartAddr = 0x9C00;

    static const u16 ExternalRamStartAddr = 0xA000;
    static const u16 ExternalRamEndAddr = 0xBFFF;

    static const u16 RamStartAddr = 0xC000;
    static const u16 RamEndAddr = 0xDFFF;

    static const u16 RamMirrorStartAddr = 0xE000;
    static const u16 RamMirrorEndAddr = 0xEFFF;

    static const u16 OAMStartAddr = 0xFE00;
    static const u16 OAMEndAddr = 0xFE9F;

    static const u16 NotUsableStartAddr = 0xFEA0;
    static const u16 NotUsableEndAddr = 0xFEFF;

    static const u16 IORegsStartAddr = 0xFF00;
    static const u16 IORegsEndAddr = 0xFF7F;

    static const u16 HighRamStartAddr = 0xFF80;
    static const u16 HighRamEndAddr = 0xFFFE;

    static const u16 InterruptsEnableRegister = 0xFFFF;
}

namespace IOReg
{
    static const u16 P1 = 0xFF00;   // Joypad Info
    static const u16 DIV = 0xFF04;  // Divider Register
    static const u16 TIMA = 0xFF05; // Timer Counter
    static const u16 TMA = 0xFF06;  // Timer Module
    static const u16 TAC = 0xFF07;  // Timer Control
    static const u16 IF = 0xFF0F;   // Interrupt Flag
    static const u16 LCDC = 0xFF40; // LCD Control
    static const u16 STAT = 0xFF41; // LCDC Status
    static const u16 SCY = 0xFF42;  // Scroll Y
    static const u16 SCX = 0xFF43;  // Scroll X
    static const u16 LY = 0xFF44;   // LCDC Y Coordinate
    static const u16 LYC = 0xFF45;  // LY Compare
    static const u16 DMA = 0xFF46;  // DMA
    static const u16 BGP = 0xFF47;  // BG & Window Palette Data
    static const u16 OBP0 = 0xFF48; // Object Palette 0 Data
    static const u16 OBP1 = 0xFF49; // Object Palette 1 Data
    static const u16 WX = 0xFF4B;   // Window X Position
    static const u16 WY = 0xFF4A;   // Window Y Position 
    static const u16 IE = 0xFFFF;   // Interrupt Enable
}

namespace Size
{
    static const int ExternalRam = 1024 * 8;
}