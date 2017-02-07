#include <fstream>
#include <algorithm>
#include "base.h"

#include "cartridge.h"

//--------------------------------------------
// --
//--------------------------------------------
Cartridge::Cartridge(const string &_filename)
{
    fill_n(mExternalRam, Size::ExternalRam, 0);

    // ...
    ifstream cartridge(_filename, ios::binary);

    cartridge.seekg(0, cartridge.end);
    mRomSize = (int)cartridge.tellg();
    cartridge.seekg(0, cartridge.beg);

    mRom = new u8[mRomSize];
    cartridge.read((char*)mRom, mRomSize);
    cartridge.close();

    // ...
    mType = mRom[0x147];

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
}

//--------------------------------------------
// --
//--------------------------------------------
bool Cartridge::WriteU8(u16 _virtAddr, u8 _value)
{
    switch(mType)
    {
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
            cout << "opcode MBC: " << mType << endl;
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