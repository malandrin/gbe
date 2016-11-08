#include <SDL.h>
#include <SDL_opengl.h>
#include "base.h"
#include "defines.h"
#include "gb.h"
#include "cpu.h"
#include "gpu.h"
#include "joypad.h"
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
    int cs = -1;

    while ((nc < _numCycles) && (cs != 0))
    {
        cs = mGb.Step();
	    nc += cs;
    }
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

    switch(_event.type)
    {
        case SDL_KEYDOWN:
        {
            Joypad& joypad = mGb.GetJoypad();

            switch(_event.key.keysym.sym)
            {
                case SDLK_UP:       joypad.OnKeyDown(Joypad::Key::Up);      break;
                case SDLK_DOWN:     joypad.OnKeyDown(Joypad::Key::Down);    break;
                case SDLK_LEFT:     joypad.OnKeyDown(Joypad::Key::Left);    break;
                case SDLK_RIGHT:    joypad.OnKeyDown(Joypad::Key::Right);   break;
                case SDLK_a:        joypad.OnKeyDown(Joypad::Key::A);       break;
                case SDLK_s:        joypad.OnKeyDown(Joypad::Key::B);       break;
                case SDLK_LSHIFT:   joypad.OnKeyDown(Joypad::Key::Select);  break;
                case SDLK_RETURN:   joypad.OnKeyDown(Joypad::Key::Start);   break;
            }
        }
        break;

        case SDL_KEYUP:
        {
            Joypad& joypad = mGb.GetJoypad();

            switch (_event.key.keysym.sym)
            {
            case SDLK_UP:       joypad.OnKeyUp(Joypad::Key::Up);      break;
            case SDLK_DOWN:     joypad.OnKeyUp(Joypad::Key::Down);    break;
            case SDLK_LEFT:     joypad.OnKeyUp(Joypad::Key::Left);    break;
            case SDLK_RIGHT:    joypad.OnKeyUp(Joypad::Key::Right);   break;
            case SDLK_a:        joypad.OnKeyUp(Joypad::Key::A);       break;
            case SDLK_s:        joypad.OnKeyUp(Joypad::Key::B);       break;
            case SDLK_LSHIFT:   joypad.OnKeyUp(Joypad::Key::Select);  break;
            case SDLK_RETURN:   joypad.OnKeyUp(Joypad::Key::Start);   break;
            }
        }
        break;
    }
}