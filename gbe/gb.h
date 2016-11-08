#pragma once
#ifndef _GB_H
#define _GB_H

#include "mmu.h"
#include "cpu.h"
#include "gpu.h"
#include "joypad.h"

class GB
{
public:

                GB() : mCpu(mMmu), mGpu(mCpu, mMmu), mJoypad(mMmu) {}

    void        PowerUp    (const string& _cartridge, const string& _bootRom);
    int         Step       ();

    const MMU&  GetMmu     () const { return mMmu; }
    CPU&        GetCpu     () { return mCpu; }
    GPU&        GetGpu     () { return mGpu; }
    Joypad&     GetJoypad  () { return mJoypad; }

private:
	MMU mMmu;
    CPU mCpu;
    GPU mGpu;
    Joypad mJoypad;
};

#endif
