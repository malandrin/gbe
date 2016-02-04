#pragma once
#ifndef _MMU_H
#define _MMU_H

class MMU
{
public:
	MMU();

    inline void WriteU8(u16 _virtAdd, u8 _value)
    {
        u8 *memory;
        u16 physAddr;

        VirtAddrToPhysAddr(_virtAdd, memory, physAddr);

        memory[physAddr] = _value;
    }

    inline u8 ReadU8(u16 _virtAdd)
    {
        u8 *memory;
        u16 physAddr;

        VirtAddrToPhysAddr(_virtAdd, memory, physAddr);

        return memory[physAddr];
    }

    inline void WriteU16(u16 _virtAdd, u16 _value)
    {
        u8 *memory;
        u16 physAddr;

        VirtAddrToPhysAddr(_virtAdd, memory, physAddr);

        memory[physAddr] = _value & 0x00FF;
        memory[physAddr + 1] = (_value >> 8) & 0x00FF;
    }

    inline u16 ReadU16(u16 _virtAdd)
    {
        u8 *memory;
        u16 physAddr;

        VirtAddrToPhysAddr(_virtAdd, memory, physAddr);

        return (memory[physAddr + 1] << 8) | memory[physAddr];
    }

	bool LoadRoms   (const string& _bootableRom, const string& _cartridge);

private:

    void VirtAddrToPhysAddr   (u16 _virtAddr, u8*& _memory, u16& _physAddr);

	u8 mRam[S8Kb];
	u8 mVRam[S8Kb];
	u8 mBootableRom[256];
	u8 *mRom;
    bool mBootableromEnabled = true;
};

#endif
