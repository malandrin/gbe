#pragma once
#ifndef _INSTRUCTIONS_VIEWER_H
#define _INSTRUCTIONS_VIEWER_H

#include "rom_walker.h"

class Debugger;

class InstructionsViewer
{
public:

            InstructionsViewer (GB &_gb, Debugger &_debugger);
           ~InstructionsViewer ();

	void    Render             ();
    bool    OnStep             ();

private:

    struct MemBlockInfo
    {
        struct Line
        {
            Line (u16 _a, u8 _nb, string &_ac) : addr(_a), numBytes(_nb), asmCode(_ac) { }

            u16 addr { 0 };
            u8  numBytes { 0 };
            string asmCode { "" };
        };

        u16 addr { 0 };
        string name { "" };
        vector<Line> lines;
        int *addr2Line { nullptr };
    };

    GB                      &mGb;
    ROMWalker               mROMWalker;
    int                     mSelectedLineIdx {-1};
    int                     mActiveLineIdx {0};
    int                     mPrevActiveLineIdx {0};
    int                     mTotalNumLines{ 0 };
    bool                    mHighRamWalked {false};
    MemBlockInfo           *mMemBlocksInfo { nullptr };
    int                     mNumBlocksInfo { 0 };
    u8                      mPrevRomBank { 0 };
    Debugger               &mDebugger;
    bool                    mShowingBootRom { false };

    void            ToggleBreakpoint           (MemBlockInfo *_info, u16 _addr);
    void            CreateMemBlockInfo         ();
    void            CreateBlockInfo            (MemBlockInfo *_info, u16 _virtAddr, int _size, const string &_name, const u8 *_mem, u16 _realAddr);
    MemBlockInfo   *GetMemBlockInfoByLine      (int _line, int &_blockInitialLine);
};

#endif
