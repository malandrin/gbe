#pragma once
#ifndef _INSTRUCTIONS_VIEWER_H
#define _INSTRUCTIONS_VIEWER_H

#include "rom_walker.h"

class Debugger;

class InstructionsViewer
{
public:

     InstructionsViewer(GB &_gb, Debugger &_debugger);
    ~InstructionsViewer();

	void Render();

    bool OnStep();

private:

    struct InstructionLine
    {
        InstructionLine(int _a, int _nb, string& _ac)
        {
            addr = _a;
            numBytes = _nb;
            asmCode = _ac;
        }

        int addr;
        int numBytes;
        string asmCode;
    };

    GB                      &mGb;
    ROMWalker               mROMWalker;
    vector<InstructionLine> mInstructionLines;
    int                     mSelectedLineIdx {-1};
    int                     mActiveLineIdx {0};
    int                     mPrevActiveLineIdx {0};
    int                     mAddrDigitCount;
    int                    *mPCLineInfo {nullptr};
    int                     mRomSize {0};
    bool                    mHighRamWalked {false};
    Debugger               &mDebugger;

	void CalculateInstructionLines  ();
    void ToggleBreakpoint           (u16 _addr);
    void ParseMemory                (int _addr, int _size);
};

#endif
