#include <fstream>
#include <memory.h>
#include "base.h"
#include "mmu.h"

//--------------------------------------------
// --
//--------------------------------------------
MMU::MMU()
{
}

//--------------------------------------------
// --
//--------------------------------------------
bool MMU::LoadRoms(const string &_bootableRom, const string &_cartridge)
{
    std::ifstream bootable(_bootableRom, std::ios::binary);

    bootable.read((char*)mBootableRom, 256);

    bootable.close();

	return true;
}

//--------------------------------------------
// --
//--------------------------------------------
void MMU::VirtAddrToPhysAddr(u16 _virtAddr, u8 *&_memory, u16 &_physAddr)
{
    if (mBootableromEnabled && (_virtAddr >= 0x000) && (_virtAddr <= 0x0100))
    {
        _memory = &mBootableRom[0];
        _physAddr = _virtAddr;
        return;
    }

    if ((_virtAddr >= 0x000) && (_virtAddr <= 0x3FFF))
    {
        _memory = mRom;
        _physAddr = _virtAddr;
        return;
    }

    if ((_virtAddr >= 0x8000) && (_virtAddr <= 0x9FFF))
    {
        _memory = &mVRam[0];
        _physAddr = _virtAddr - 0x8000;
        return;
    }

    if ((_virtAddr >= 0xC000) && (_virtAddr <= 0xDFFF))
    {
        _memory = &mRam[0];
        _physAddr = _virtAddr - 0xC000;
        return;
    }

    throw std::runtime_error("memory address unknown: " + _virtAddr);
}