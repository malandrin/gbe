#pragma once
#ifndef _MACHINE_H
#define _MACHINE_H

#include "cpu_runnable.h"
class GB;

class Machine : public ICpuRunnable
{
public:
            Machine     (GB& _gb);
	       ~Machine     ();

	void    Update      (int _numCycles);
    void    Render      ();
	void    HandleEvent (SDL_Event& _event);

    PPU    &GetPpu      () { return mPpu; }

private:
	GB  &mGb;
	CPU &mCpu;
    PPU &mPpu;
    SDL_Window*   mWindow { nullptr };
    SDL_Renderer* mRenderer { nullptr };
    SDL_Texture*  mTexture { nullptr };
};

#endif 
