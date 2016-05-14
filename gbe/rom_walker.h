#pragma once

class ROMWalker
{
public:

                ROMWalker   (const u8* _rom, int _romSize, u16 _addr);

        void    Analyze     (u16 _addr, bool _clear=false);
        bool    IsCode      (u16 _addr);

private:
    unordered_set<u16> mCode;
    vector<u16>        mPaths;
    const u8*          mRom {nullptr};
    int                mRomSize {0};

    void        AnalyzePath         (u16 _addr);
    u8          GetInstructionSize  (u8 _opcode);
};
