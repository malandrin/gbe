#pragma once

#include "defines.h"

class Cartridge
{
public:

                Cartridge   (const string &_filename);
               ~Cartridge   ();

    u8         *GetRom      (u16 _addr = 0) const { return &mRom[_addr]; }
    u8         *GetBankRom  (u16 _addr = 0) const;
    const u8   *GetRam      () const { return mExternalRam; }

    int         GetRomSize  () const { return mRomSize; }
    string      GetTitle    () const { return mTitle; }

    u8          GetRomBank  () const { return mRomBank; }
    u8          GetRamBank  () const { return mRamBank; }

    bool        WriteU8     (u16 _virtAddr, u8 _value);

private:

    enum MBC
    {
        MBC1 = 1
    };

    u8     *mRom { nullptr };
    u8      mRomBank { 1 };
    u8      mRamBank { 0 };
    int     mRomSize { 0 };
    bool    mRomBankingMode { true };
    u8      mType { 0 };
    u8      mExternalRam[Size::ExternalRam] { 0 };
    string  mTitle { "" };
};
