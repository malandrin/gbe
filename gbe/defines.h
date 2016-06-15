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
    static const u16 BootRomStartAddr = 0x0000;
    static const u16 BootRomEndAddr = 0x0100;

    static const u16 RomStartAddr = 0x0000;
    static const u16 RomEndAddr = 0x3FFF;

    static const u16 VRamStartAddr = 0x8000;
    static const u16 VRamEndAddr = 0x9FFF;

    static const u16 VRamTileData1StartAddr = 0x8000;
    static const u16 VRamTileData2StartAddr = 0x8800;

    static const u16 VRamTileMap1StartAddr = 0x9800;
    static const u16 VRamTileMap2StartAddr = 0x9C00;

    static const u16 RamStartAddr = 0xC000;
    static const u16 RamEndAddr = 0xDFFF;

    static const u16 IORegsStartAddr = 0xFF00;
    static const u16 IORegsEndAddr = 0xFF7F;

    static const u16 HighRamStartAddr = 0xFF80;
    static const u16 HighRamEndAddr = 0xFFFE;
}

namespace IOReg
{
    static const u16 LCDC = 0xFF40; // LCD Control
    static const u16 SCY = 0xFF42;  // Scroll Y
    static const u16 SCX = 0xFF43;  // Scroll X
    static const u16 LY = 0xFF44;   // LCDC Y Coordinate
    static const u16 BGP = 0xFF47;  // BG & Window Palette Data
}