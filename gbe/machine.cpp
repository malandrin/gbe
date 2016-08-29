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
Machine::Machine(GB& _gb) : mGb(_gb), mCpu(_gb.GetCpu()), mGpu(_gb.GetGpu())
{
	mWindow = SDL_CreateWindow("GBE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Screen::Width << 1, Screen::Height << 1, SDL_WINDOW_OPENGL);
    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);

    mTexture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, Screen::Width, Screen::Height);
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
void Machine::Update(int _numCycles)
{
    int nc = 0;

    while (nc < _numCycles)
	    nc += mGb.Step();
}

//--------------------------------------------
// --
//--------------------------------------------
void Machine::Render()
{
    SDL_RenderClear(mRenderer);

    if (mGpu.IsLCDOn())
    {
        SDL_UpdateTexture(mTexture, nullptr, mGpu.GetFrameBuffer(), Screen::Width * 4);
        SDL_RenderCopy(mRenderer, mTexture, nullptr, nullptr);
    }

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