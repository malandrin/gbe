#include <fstream>
#include <algorithm>
#include "base.h"

#include "cartridge.h"

//--------------------------------------------
// --
//--------------------------------------------
Cartridge::Cartridge(const string &_filename)
{
    ifstream cartridge(_filename, ios::binary);

    cartridge.seekg(0, cartridge.end);
    mRomSize = (int)cartridge.tellg();
    cartridge.seekg(0, cartridge.beg);

    mRom = new u8[mRomSize];
    cartridge.read((char*)mRom, mRomSize);
    cartridge.close();

    // ...
    mType = mRom[0x147];

    switch(mType)
    {
        case MBC1:
        case MBC3:
        {
            mRamSize = 32 * 1024;
            mRam = new u8[32 * 1024] { 0 };

            // sav filename
            mSavFilename = _filename;

            int dpos = mSavFilename.find_last_of('.');

            if (dpos != -1)
                mSavFilename = mSavFilename.substr(0, dpos);

            mSavFilename += ".sav";

            // ...
            if (FileExists(mSavFilename))
            {
                ifstream fileram(mSavFilename, ios::binary);
                fileram.read((char*)mRam, mRamSize);
                fileram.close();
            }
        }
        break;
    }

    // ...
    char title[17] { 0 };
    memcpy_s(title, 16, &mRom[0x134], 16);
    mTitle = title;
}

//--------------------------------------------
// --
//--------------------------------------------
Cartridge::~Cartridge()
{
    if (mRom != nullptr)
        delete[] mRom;

    if (mRam != nullptr)
    {
        ofstream output(mSavFilename, ofstream::binary);
        output.write((char*)mRam, mRamSize);
        output.close();

        delete[] mRam;
    }
}

//--------------------------------------------
// --
//--------------------------------------------
bool Cartridge::WriteU8(u16 _virtAddr, u8 _value)
{
    switch(mType)
    {
        case MBC0:
            break;

        case MBC1:
        case MBC3:
            if (_virtAddr >= Memory::RomRamModeSelectStartAddr && _virtAddr <= Memory::RomRamModeSelectEndAddr)
            {
                mRomBankingMode = _value == 0;
                return true;
            }

            if (_virtAddr >= Memory::RomBankNumberStartAddr && _virtAddr <= Memory::RomBankNumberEndAddr)
            {
                mRomBank = max(_value & 0x1F, 1);
                return true;
            }

            if (_virtAddr >= Memory::RamBankNumberStartAddr && _virtAddr <= Memory::RamBankNumberEndAddr)
            {
                if (mRomBankingMode)
                    mRomBank |= _value & 0x60;
                else
                    mRamBank = _value & 0x3;
                return true;
            }
            break;

        default:
            cout << "unknown MBC: " << mType << endl;
            break;
    }

    return false;
}

//--------------------------------------------
// --
//--------------------------------------------
u8 *Cartridge::GetBankRom(u16 _addr) const
{
    switch(mType)
    {
        case MBC0:
            return GetRom(_addr);
            break;

        case MBC1:
        case MBC3:
            return &mRom[Memory::RomStartAddr + (mRomBank * 0x4000) + (_addr - Memory::RomBankNStartAddr)];
            break;

        default:
            cout << "opcode MBC: " << mType << endl;
            break;
    }

    return nullptr;
}

//--------------------------------------------
// --
//--------------------------------------------
u8 *Cartridge::GetRam(u16 _addr)
{
    return &mRam[(mRamBank * 0x1FFF) + (_addr - Memory::ExternalRamStartAddr)];
}