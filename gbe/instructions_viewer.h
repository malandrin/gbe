#pragma once
#ifndef _INSTRUCTIONS_VIEWER_H
#define _INSTRUCTIONS_VIEWER_H

#include "rom_walker.h"

class MMU;
class CPU;
class Debugger;

class InstructionsViewer
{
public:

     InstructionsViewer(const MMU &_mmu, CPU &_cpu, Debugger &_debugger);
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

    const MMU               &mMmu;
    CPU                     &mCpu;
    ROMWalker               mROMWalker;
    vector<InstructionLine> mInstructionLines;
    int                     mSelectedLineIdx {-1};
    int                     mActiveLineIdx {0};
    int                     mPrevActiveLineIdx {0};
    int                     mAddrDigitCount;
    int                    *mPCLineInfo {nullptr};
    Debugger               &mDebugger;

	void CalculateInstructionLines ();
    void ToggleBreakpoint          (u16 _addr);
    void ProcessCb                 (u8 _opcode, u16 _addr);
};

#endif
