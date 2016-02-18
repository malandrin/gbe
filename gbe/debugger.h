#pragma once
#ifndef _DEBUGGER_H
#define _DEBUGGER_H

#include "memory_viewer.h"
#include "instructions_viewer.h"
#include "registers_viewer.h"

class GB;

class Debugger
{
public:
	Debugger(GB& _gb);
	~Debugger();

	void HandleEvent(SDL_Event& _event);
	void Render();

private:
	SDL_Window         *mWindow;
	SDL_GLContext      mContext;
	MemoryViewer       mMemoryViewer;
    InstructionsViewer mInstructionsViewer;
    RegistersViewer    mRegistersViewer;
};

#endif
