#pragma once

#include "defines.h"

class Cartridge
{
public:

                Cartridge   (const string &_filename);
               ~Cartridge   ();

    u8         *GetRom      (u16 _addr = 0) const { return &mRom[_addr]; }
    u8         *GetBankRom  (u16 _addr = 0) const;
    u8         *GetRam      (u16 _addr = 0);

    int         GetRomSize  () const { return mRomSize; }
    string      GetTitle    () const { return mTitle; }

    u8          GetRomBank  () const { return mRomBank; }
    u8          GetRamBank  () const { return mRamBank; }

    bool        WriteU8     (u16 _virtAddr, u8 _value);

private:

    enum MBC
    {
        MBC0 = 0,
        MBC1 = 1,
        MBC3 = 3
    };

    u8     *mRom { nullptr };
    u8     *mRam { nullptr };
    u8      mRomBank { 1 };
    u8      mRamBank { 0 };
    int     mRomSize { 0 };
    int     mRamSize { 0 };
    bool    mRomBankingMode { true };
    u8      mType { 0 };
    string  mTitle { "" };
    string  mSavFilename { "" };
};
