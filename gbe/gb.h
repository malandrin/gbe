#pragma once
#ifndef _GB_H
#define _GB_H

#include "mmu.h"
#include "cpu.h"
#include "ppu.h"
#include "joypad.h"
#include "timer.h"

class Cartridge;

class GB
{
public:

                GB              () : mCpu(mMmu, mTimer), mPpu(mCpu, mMmu), mJoypad(mMmu), mTimer(mMmu) {}

    void        PowerUp         (Cartridge *_cartridge);
    int         Step            ();

    const MMU&  GetMmu          () const { return mMmu; }
    CPU&        GetCpu          () { return mCpu; }
    PPU&        GetPpu          () { return mPpu; }
    Joypad&     GetJoypad       () { return mJoypad; }
    Cartridge*  GetCartridge    () { return mCartridge; }

private:
	MMU     mMmu;
    CPU     mCpu;
    PPU     mPpu;
    Joypad  mJoypad;
    Timer   mTimer;
    Cartridge *mCartridge { nullptr };
};

#endif
