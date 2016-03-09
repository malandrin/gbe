#pragma once
#ifndef _MMU_H
#define _MMU_H

class MMU
{
public:
	MMU();
    ~MMU();

	static const int RamSize = 1024 * 8;
	static const int VRamSize = 1024 * 8;
	static const int BootableRomSize = 256;
    static const int IORegistersSize = 128;
    static const int HighRamSize = 127;

    inline void WriteU8(u16 _virtAdd, u8 _value)
    {
        *VirtAddrToPhysAddr(_virtAdd) = _value;
    }

    inline u8 ReadU8(u16 _virtAdd) const
    {
        return *VirtAddrToPhysAddr(_virtAdd);
    }

    inline void WriteU16(u16 _virtAdd, u16 _value)
    {
        (*(u16*)VirtAddrToPhysAddr(_virtAdd)) = _value;
    }

    inline u16 ReadU16(u16 _virtAdd) const
    {
        return *(u16*)VirtAddrToPhysAddr(_virtAdd);
    }

	const u8* GetBootableRom() const {return mBootableRom;}
    const u8* GetRom() const {return mRom;}
	const u8* GetRam() const {return mRam;}
	const u8* GetVRam() const { return mVRam; }
    const u8* GetIORegisters() const { return mIORegisters; }
    const u8* GetHighRam() const { return mHighRam; }

	bool LoadRoms   (const string& _bootableRom, const string& _cartridge);

	int  GetRomSize () const {return mRomSize;}

    bool IsInBootableRom     () const {return mBootableRomEnabled;}
    void DisableBootableRom  () {mBootableRomEnabled = false;}

private:

    u8* VirtAddrToPhysAddr   (u16 _virtAddr) const;

	u8 mRam[RamSize];
	u8 mVRam[VRamSize];
	u8 mBootableRom[BootableRomSize];
    u8 mIORegisters[IORegistersSize];
    u8 mHighRam[HighRamSize];
	u8 *mRom {nullptr};
	int mRomSize{0};
    bool mBootableRomEnabled {true};
};

#endif
