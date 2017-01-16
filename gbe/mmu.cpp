#include <assert.h>
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
    fill_n(mOAM, OAMSize, 0);

    mDummyListener = new MMUDummyListener();
    mListeners[0] = mDummyListener;
    mListeners[1] = mDummyListener;
    mListeners[2] = mDummyListener;
}

//--------------------------------------------
// --
//--------------------------------------------
MMU::~MMU()
{
    delete mDummyListener;
}

//--------------------------------------------
// --
//--------------------------------------------
void MMU::AddListener(IMmuListener* _listener)
{
    assert(mListeners[2] == mDummyListener);

    if (mListeners[0] == mDummyListener)
        mListeners[0] = _listener;
    else if (mListeners[1] == mDummyListener)
        mListeners[1] = _listener;
    else
        mListeners[2] = _listener;
}

/*
//--------------------------------------------
// --
//--------------------------------------------
bool MMU::LoadRoms(const string &_cartridge, const string &_bootRom )
{
    if (_bootRom != "")
    {
        ifstream bootable(_bootRom, ios::binary);
        bootable.read((char*)mBootableRom, 256);
        bootable.close();
    }

	return true;
}
*/

//--------------------------------------------
// --
//--------------------------------------------
void MMU::SetStateAfterBoot()
{
    WriteU8(0xFF05, 0x00);
    WriteU8(0xFF06, 0x00);
    WriteU8(0xFF07, 0x00);
    WriteU8(0xFF10, 0x80);
    WriteU8(0xFF11, 0xBF);
    WriteU8(0xFF12, 0xF3);
    WriteU8(0xFF14, 0xBF);
    WriteU8(0xFF16, 0x3F);
    WriteU8(0xFF17, 0x00);
    WriteU8(0xFF19, 0xBF);
    WriteU8(0xFF1A, 0x7F);
    WriteU8(0xFF1B, 0xFF);
    WriteU8(0xFF1C, 0x9F);
    WriteU8(0xFF1E, 0xBF);
    WriteU8(0xFF20, 0xFF);
    WriteU8(0xFF21, 0x00);
    WriteU8(0xFF22, 0x00);
    WriteU8(0xFF23, 0xBF);
    WriteU8(0xFF24, 0x77);
    WriteU8(0xFF25, 0xF3);
    WriteU8(0xFF26, 0xF1);
    WriteU8(0xFF40, 0x91);
    WriteU8(0xFF42, 0x00);
    WriteU8(0xFF43, 0x00);
    WriteU8(0xFF45, 0x00);
    WriteU8(0xFF47, 0xFC);
    WriteU8(0xFF48, 0xFF);
    WriteU8(0xFF49, 0xFF);
    WriteU8(0xFF4A, 0x00);
    WriteU8(0xFF4B, 0x00);
    WriteU8(0xFFFF, 0x00);

    mBootableRomEnabled = false;
}

//--------------------------------------------
// --
//--------------------------------------------
u8* MMU::VirtAddrToPhysAddr(u16 _virtAddr) const
{
    if (mBootableRomEnabled && (_virtAddr >= Memory::BootRomStartAddr) && (_virtAddr <= Memory::BootRomEndAddr))
        return (u8*)&mBootableRom[_virtAddr];

    if ((_virtAddr >= Memory::RomStartAddr) && (_virtAddr <= Memory::RomEndAddr))
        return mCartridge->GetRom(_virtAddr);

    if ((_virtAddr >= Memory::RomBankNStartAddr) && (_virtAddr <= Memory::RomBankNEndAddr))
        return mCartridge->GetBankRom(_virtAddr);

    if ((_virtAddr >= Memory::VRamStartAddr) && (_virtAddr <= Memory::VRamEndAddr))
        return (u8*)&mVRam[_virtAddr - Memory::VRamStartAddr];

    if (((_virtAddr >= Memory::RamStartAddr) && (_virtAddr <= Memory::RamEndAddr)) ||
        ((_virtAddr >= Memory::RamMirrorStartAddr) && (_virtAddr <= Memory::RamMirrorEndAddr)))
        return (u8*)&mRam[_virtAddr - Memory::RamStartAddr];

    if ((_virtAddr >= Memory::ExternalRamStartAddr) && (_virtAddr <= Memory::ExternalRamEndAddr))
        return (u8*)&mCartridge->GetRam()[_virtAddr - Memory::ExternalRamStartAddr];

    if ((_virtAddr >= Memory::OAMStartAddr) && (_virtAddr <= Memory::OAMEndAddr))
        return (u8*)&mOAM[_virtAddr - Memory::OAMStartAddr];

    if ((_virtAddr >= Memory::IORegsStartAddr) && (_virtAddr <= Memory::IORegsEndAddr))
        return (u8*)&mIORegisters[_virtAddr - Memory::IORegsStartAddr];

    if ((_virtAddr >= Memory::HighRamStartAddr) && (_virtAddr <= Memory::HighRamEndAddr))
        return (u8*)&mHighRam[_virtAddr - Memory::HighRamStartAddr];

    if (_virtAddr == Memory::InterruptsEnableRegister)
        return (u8*)&mIER;

    //todo: temp
    if (_virtAddr >= 0xF000 && _virtAddr <= 0xFDFF)
        return (u8*)&mRam[_virtAddr - Memory::RamStartAddr];

    throw runtime_error("memory address unknown: " + Int2Hex(_virtAddr));
}

//--------------------------------------------
// --
//--------------------------------------------
bool MMU::IsValidAddr(u16 _virtAddr, bool _read) const
{
    if (_read)
        return !((_virtAddr >= Memory::NotUsableStartAddr) && (_virtAddr <= Memory::NotUsableEndAddr));

    return !(
        ((_virtAddr >= Memory::NotUsableStartAddr) && (_virtAddr <= Memory::NotUsableEndAddr)) || 
        ((_virtAddr >= Memory::RomStartAddr) && (_virtAddr <= Memory::RomBankNEndAddr))
    );
}

//--------------------------------------------
// --
//--------------------------------------------
void MMU::CopyMem(u16 _startAddr, u16 _destAddr, u16 _size)
{
    memcpy(VirtAddrToPhysAddr(_destAddr), VirtAddrToPhysAddr(_startAddr), _size);
}

//--------------------------------------------
// --
//--------------------------------------------
void MMU::WriteU8(u16 _virtAddr, u8 _value, bool _warnLinesteners)
{
    // MBC
    if (!mCartridge->WriteU8(_virtAddr, _value))
    {
        if (IsValidAddr(_virtAddr, false))
        {
            *VirtAddrToPhysAddr(_virtAddr) = _value;

            if (_warnLinesteners)
            {
                mListeners[0]->OnMemoryWrittenU8(_virtAddr, _value);
                mListeners[1]->OnMemoryWrittenU8(_virtAddr, _value);
                mListeners[2]->OnMemoryWrittenU8(_virtAddr, _value);
            }
        }
    }
}