#pragma once
#ifndef _DEBUGGER_H
#define _DEBUGGER_H

#include <SDL.h>
#include "memory_viewer.h"
#include "instructions_viewer.h"
#include "general_viewer.h"
#include "vram_viewer.h"
#include "cpu_runnable.h"

class GB;

class Debugger : public ICpuRunnable
{
public:
	     Debugger       (GB& _gb);
	    ~Debugger       ();

	void HandleEvent    (SDL_Event& _event);
    void Update         (int _numCycles);
	void Render         ();

    void Break          ();
    void Continue       ();
    void Step           ();

private:
	SDL_Window         *mWindow;
	SDL_GLContext      mContext;
	MemoryViewer       mMemoryViewer;
    InstructionsViewer mInstructionsViewer;
    GeneralViewer      mGeneralViewer;
    VRAMViewer         mVRamViewer;
    CPU&               mCpu;
    GB&                mGb;
    int                mNumCyclesToExecute { 0 };
};

#endif
