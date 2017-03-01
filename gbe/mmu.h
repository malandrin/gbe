#pragma once
#ifndef _MMU_H
#define _MMU_H

#include "mmu_listener.h"
#include "defines.h"
#include "cartridge.h"

class MMUDummyListener : public IMmuListener
{
    virtual void OnMemoryWrittenU8  (u16 _virtAdd, u8 _value) {}
    virtual void OnMemoryWrittenU16 (u16 _virtAdd, u16 _value) {}
};

class MMU
{
public:
	            MMU                     ();
               ~MMU                     ();

    void        CopyMem                 (u16 _startAddr, u16 _destAddr, u16 _size);

    const u8    *GetBootRom             () const;
    const u8    *GetRom                 () const { return (mCartridge != nullptr) ? mCartridge->GetRom() : nullptr; }
    const u8    *GetRam                 () const { return mRam; }
    const u8    *GetVRam                () const { return mVRam; }
    const u8    *GetIORegisters         () const { return mIORegisters; }
    const u8    *GetHighRam             () const { return mHighRam; }
    const u8    *GetOAM                 () const { return mOAM; }

    void        OnCartridgeInserted     (Cartridge *_cartridge) { mCartridge = _cartridge; }

    int         GetRomSize              () const { return (mCartridge != nullptr) ? mCartridge->GetRomSize() : 0; }
    const u8*   GetActiveRom            () const { return mBootRomEnabled ? GetBootRom() : GetRom(); }
    int         GetActiveRomSize        () const { return mBootRomEnabled ? Size::BootRom : GetRomSize(); }
    u16         GetActiveRomEntryAddr   () const { return mBootRomEnabled ? 0 : 0x100; }

    u8          GetRomBank              () const { return (mCartridge != nullptr) ? mCartridge->GetRomBank() : 0; }
    u8          GetRamBank              () const { return (mCartridge != nullptr) ? mCartridge->GetRamBank() : 0; }

    bool        IsInBootRom             () const { return mBootRomEnabled; }
    void        SetStateAfterBoot       ();

    void        AddListener             (IMmuListener* _listener);

    void        WriteU8                 (u16 _virtAdd, u8 _value, bool _warnLinesteners = true);

    inline u8 ReadU8(u16 _virtAdd) const
    {
        return IsValidAddr(_virtAdd, true) ? *VirtAddrToPhysAddr(_virtAdd) : 0;
    }

    inline void WriteU16(u16 _virtAdd, u16 _value)
    {
        if (IsValidAddr(_virtAdd, false))
        {
            (*(u16*)VirtAddrToPhysAddr(_virtAdd)) = _value;

            mListeners[0]->OnMemoryWrittenU16(_virtAdd, _value);
            mListeners[1]->OnMemoryWrittenU16(_virtAdd, _value);
            mListeners[2]->OnMemoryWrittenU16(_virtAdd, _value);
        }
    }

    inline u16 ReadU16(u16 _virtAdd) const
    {
        return IsValidAddr(_virtAdd, true) ? *(u16*)VirtAddrToPhysAddr(_virtAdd) : 0;
    }

private:

    Cartridge  *mCartridge { nullptr };
	u8          mRam[Size::Ram];
	u8          mVRam[Size::VRam];
    u8          mIORegisters[Size::IORegisters];
    u8          mHighRam[Size::HighRam];
    u8          mOAM[Size::OAM];
    u8          mIER { 0 }; // Interrups Enable Register
    bool        mBootRomEnabled { true };
    IMmuListener     *mListeners[3] { nullptr };
    MMUDummyListener *mDummyListener { nullptr };

    u8*  VirtAddrToPhysAddr   (u16 _virtAddr) const;
    bool IsValidAddr          (u16 _virtAddr, bool _read) const;
};

#endif
