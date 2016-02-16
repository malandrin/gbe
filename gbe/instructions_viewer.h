#pragma once
#ifndef _INSTRUCTIONS_VIEWER_H
#define _INSTRUCTIONS_VIEWER_H

class MMU;

class InstructionsViewer
{
public:

    InstructionsViewer(const MMU &_mmu);

	void Render();

private:

    struct InstructionLine
    {
        InstructionLine(int _p, int _nb, string& _ac)
        {
            pos = _p;
            numBytes = _nb;
            asmCode = _ac;
        }

        int pos;
        int numBytes;
        string asmCode;
    };

    const MMU               &mMmu;
    vector<InstructionLine> mInstructionLines;
    int                     mSelectedLineIdx {-1};
    int                     mActiveLineIdx {-1};
    int                     mAddrDigitCount;
    unordered_set<int>      mBreakpoints;

	void CalculateInstructionLines ();
    void ToggleBreakpoint          (int _line);
};

#endif
