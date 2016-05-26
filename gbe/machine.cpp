#include <SDL.h>
#include <SDL_opengl.h>
#include "base.h"
#include "defines.h"
#include "gb.h"
#include "cpu.h"
#include "gpu.h"
#include "machine.h"

//--------------------------------------------
// --
//--------------------------------------------
Machine::Machine(GB& _gb) : mGb(_gb), mCpu(_gb.GetCpu())
{
	mWindow = SDL_CreateWindow("GBE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Screen::Width << 1, Screen::Height << 1, SDL_WINDOW_OPENGL);
    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
}

//--------------------------------------------
// --
//--------------------------------------------
Machine::~Machine()
{
	SDL_DestroyRenderer(mRenderer);
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
void Machine::Render()
{
    SDL_RenderClear(mRenderer); // TODO: quitar cuando se pinte la textura

    // TODO: copiar textura a pantalla

    SDL_RenderPresent(mRenderer);
}

//--------------------------------------------
// --
//--------------------------------------------
void Machine::HandleEvent(SDL_Event& _event)
{
	if (_event.window.windowID != SDL_GetWindowID(mWindow))
		return;
}