#include <fstream>
#include <memory.h>
#include "base.h"
#include "mmu.h"

//--------------------------------------------
// --
//--------------------------------------------
MMU::MMU()
{
    fill_n(mRam, RamSize, 0);
    fill_n(mVRam, VRamSize, 0);
    fill_n(mBootableRom, BootableRomSize, 0);
}

//--------------------------------------------
// --
//--------------------------------------------
MMU::~MMU()
{
    if (mRom != nullptr)
    {
        delete[] mRom;
    }
}

//--------------------------------------------
// --
//--------------------------------------------
bool MMU::LoadRoms(const string &_bootableRom, const string &_cartridge)
{
    ifstream bootable(_bootableRom, ios::binary);
    bootable.read((char*)mBootableRom, 256);
    bootable.close();

    // ...
    ifstream cartridge(_cartridge, ios::binary);

    cartridge.seekg(0, cartridge.end);
	mRomSize = (int)cartridge.tellg();
    cartridge.seekg(0, cartridge.beg);

    mRom = new u8[mRomSize];
    cartridge.read((char*)mRom, mRomSize);
    cartridge.close();

	return true;
}

//--------------------------------------------
// --
//--------------------------------------------
u8* MMU::VirtAddrToPhysAddr(u16 _virtAddr) const
{
    if (mBootableromEnabled && (_virtAddr >= 0x000) && (_virtAddr <= 0x00FF))
    {
        return (u8*)&mBootableRom[_virtAddr];
    }

    if ((_virtAddr >= 0x000) && (_virtAddr <= 0x3FFF))
    {
        return &mRom[_virtAddr];
    }

    if ((_virtAddr >= 0x8000) && (_virtAddr <= 0x9FFF))
    {
        return (u8*)&mVRam[_virtAddr - 0x8000];
    }

    if ((_virtAddr >= 0xC000) && (_virtAddr <= 0xDFFF))
    {
        return (u8*)&mRam[_virtAddr - 0xC000];
    }

    throw runtime_error("memory address unknown: " + Int2Hex(_virtAddr));
}