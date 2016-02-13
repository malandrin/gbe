#pragma once
#ifndef _DEBUGGER_H
#define _DEBUGGER_H

class GB;

class Debugger
{
public:
	Debugger(GB& _gb);
	~Debugger();

	void HandleEvent(SDL_Event& _event);
	void Render();

private:
	SDL_Window *mWindow;
	SDL_GLContext mContext;
	MemoryViewer mMemoryViewer;
};

#endif
