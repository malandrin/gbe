#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include "base.h"
#include "gb.h"
#include "machine.h"
#include "defines.h"
#include "debugger.h"
#include "cpu_runnable.h"
#include "cartridge.h"

int main(int argn, char *argv[])
{
	if (argn < 2)
	{
		cout << "Invalid number of arguments!" << endl;
		cout << "Syntax: gbe.exe rom [-debugger] [-disable_boot_rom]";
		return -1;
	}

    // check file existence
    if (!FileExists(argv[1]))
    {
        cout << "File " << argv[1] << " does not exist";
        return -1;
    }

	// ...
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
        cout << "Error: " << SDL_GetError() << endl;
        return -1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    // ...
    bool runBootRom = true;
    bool debuggerActive = false;

    for (int i = 2; i < argn; ++i)
    {
        string sa = string(argv[i]);

        // todo: add error checking
        if (sa == "-debugger")
            debuggerActive = true;
        else if (sa == "-disable_boot_rom")
            runBootRom = false;
    }

	// ...
	GB gb;
    Cartridge cartridge(argv[1]);

	gb.PowerUp(&cartridge, runBootRom);

	unique_ptr<Debugger> debugger {nullptr};

	if (debuggerActive)
	{
		debugger = unique_ptr<Debugger>(new Debugger(gb));
		gb.GetCpu().Break();
	}

    Machine machine(gb);

	// ...
	SDL_Event event;
	bool done = false;
	Uint32 preTime = 0;
    Uint32 emuTime = 0;
    Uint32 msPerFrame = 17;

    ICpuRunnable *cpuRunnable;
    
    if (debugger != nullptr) 
        cpuRunnable = debugger.get();
    else
        cpuRunnable = &machine;

	while(!done)
	{
		while(SDL_PollEvent(&event))
		{
			if ((((event.type == SDL_WINDOWEVENT) && (event.window.event == SDL_WINDOWEVENT_CLOSE))) || (event.type == SDL_QUIT))
				done = true;
			else
			{
				if (debugger != nullptr)
					debugger->HandleEvent(event);

				machine.HandleEvent(event);
			}
		}

        preTime = SDL_GetTicks();

        cpuRunnable->Update(Cycles::PerFrame);
        machine.Render();

        emuTime = SDL_GetTicks() - preTime;

		if (debugger != nullptr)
			debugger->Render();

        if (machine.GetPpu().IsLCDOn() && emuTime < msPerFrame)
            SDL_Delay(msPerFrame - emuTime);
	}

	SDL_Quit();

    return 0;
}