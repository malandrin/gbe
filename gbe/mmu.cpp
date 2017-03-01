#include <assert.h>
#include <fstream>
#include <memory.h>
#include "base.h"
#include "defines.h"
#include "boot_rom.h"
#include "mmu.h"

//--------------------------------------------
// --
//--------------------------------------------
MMU::MMU()
{
    fill_n(mRam, Size::Ram, 0);
    fill_n(mVRam, Size::VRam, 0);
    fill_n(mIORegisters, Size::IORegisters, 0);
    fill_n(mHighRam, Size::HighRam, 0);
    fill_n(mOAM, Size::OAM, 0);

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

    mBootRomEnabled = false;
}

//--------------------------------------------
// --
//--------------------------------------------
u8* MMU::VirtAddrToPhysAddr(u16 _virtAddr) const
{
    if (mBootRomEnabled && (_virtAddr >= Memory::BootRomStartAddr) && (_virtAddr <= Memory::BootRomEndAddr))
        return (u8*)&BootROM[_virtAddr];

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
        return mCartridge->GetRam(_virtAddr);

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

    cout << "memory address unknown: " << Int2Hex(_virtAddr);
    return 0;
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
    if ((_virtAddr >= Memory::ExternalRamStartAddr) && (_virtAddr <= Memory::ExternalRamEndAddr))
        int a = 0;

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

//--------------------------------------------
// --
//--------------------------------------------
const u8 *MMU::GetBootRom() const
{
    return BootROM;
}