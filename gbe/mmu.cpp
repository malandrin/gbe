#include <fstream>
#include <memory.h>
#include "base.h"
#include "defines.h"
#include "mmu.h"

//--------------------------------------------
// --
//--------------------------------------------
MMU::MMU()
{
    fill_n(mRam, RamSize, 0);
    fill_n(mVRam, VRamSize, 0);
    fill_n(mBootableRom, BootableRomSize, 0);
    fill_n(mIORegisters, IORegistersSize, 0);
    fill_n(mHighRam, HighRamSize, 0);
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
void MMU::AddListener(IMmuListener* _listener)
{
    mListeners.push_back(_listener);
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
    if (mBootableRomEnabled && (_virtAddr >= Memory::BootRomStartAddr) && (_virtAddr <= Memory::BootRomEndAddr))
    {
        return (u8*)&mBootableRom[_virtAddr];
    }

    if ((_virtAddr >= Memory::RomStartAddr) && (_virtAddr <= Memory::RomEndAddr))
    {
        return &mRom[_virtAddr];
    }

    if ((_virtAddr >= Memory::VRamStartAddr) && (_virtAddr <= Memory::VRamEndAddr))
    {
        return (u8*)&mVRam[_virtAddr - Memory::VRamStartAddr];
    }

    if ((_virtAddr >= Memory::RamStartAddr) && (_virtAddr <= Memory::RamEndAddr))
    {
        return (u8*)&mRam[_virtAddr - Memory::RamStartAddr];
    }

    if ((_virtAddr >= Memory::IORegsStartAddr) && (_virtAddr <= Memory::IORegsEndAddr))
    {
        return (u8*)&mIORegisters[_virtAddr - Memory::IORegsStartAddr];
    }

    if ((_virtAddr >= Memory::HighRamStartAddr) && (_virtAddr <= Memory::HighRamEndAddr))
    {
        return (u8*)&mHighRam[_virtAddr - Memory::HighRamStartAddr];
    }

    throw runtime_error("memory address unknown: " + Int2Hex(_virtAddr));
}