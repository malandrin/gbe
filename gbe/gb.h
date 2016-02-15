#pragma once
#ifndef _GB_H
#define _GB_H

#include "mmu.h"
#include "cpu.h"

class GB
{
public:
    GB() : mCpu(mMmu) {}
    void        PowerUp    (const string& _bootableRom, const string& _cartridge);
    const MMU&  GetMmu     () const { return mMmu; }

private:
	MMU mMmu;
    CPU mCpu;
};

#endif
