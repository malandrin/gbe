#include <SDL.h>
#include <SDL_opengl.h>
#include "base.h"
#include "gb.h"
#include "cpu.h"
#include "machine.h"

//--------------------------------------------
// --
//--------------------------------------------
Machine::Machine(GB& _gb) : mGb(_gb), mCpu(_gb.GetCpu())
{
	mWindow = SDL_CreateWindow("GBE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160, 144, SDL_WINDOW_OPENGL);
	mContext = SDL_GL_CreateContext(mWindow);
}

//--------------------------------------------
// --
//--------------------------------------------
Machine::~Machine()
{
	SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
}

//--------------------------------------------
// --
//--------------------------------------------
void Machine::Update()
{
	if (!mCpu.IsOnDebugMode())
	    mCpu.Step();
}

//--------------------------------------------
// --
//--------------------------------------------
void Machine::HandleEvent(SDL_Event& _event)
{
	if (_event.window.windowID != SDL_GetWindowID(mWindow))
		return;
}