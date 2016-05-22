#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include "base.h"
#include "gb.h"
#include "machine.h"
#include "debugger.h"

int main(int argn, char *argv[])
{
	if (argn < 3)
	{
		cout << "Invalid number of arguments!" << endl;
		cout << "Syntax: gbe.exe bootable_rom rom [-debugger]";
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
	GB gb;
	gb.PowerUp(argv[1], argv[2]);

	Machine machine(gb);
	unique_ptr<Debugger> debugger {nullptr};

	if ((argn > 3) && (string(argv[3]) == "-debugger"))
	{
		debugger = unique_ptr<Debugger>(new Debugger(gb));
		gb.GetCpu().Break();
	}

	// ...
	SDL_Event event;
	bool done = false;
	Uint32 actTime = 0;
	Uint32 prevTime = 0;
	Uint32 delta = 0;
	Uint32 timeToRender = 0;

	while(!done)
	{
		actTime = SDL_GetTicks();
		delta = actTime - prevTime;
		prevTime = actTime;

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

		machine.Update();
        machine.Render();

		if (debugger != nullptr)
		{
			if (gb.GetCpu().IsOnDebugMode())
				debugger->Render();
			else
			{
				timeToRender += delta;
				if (timeToRender >= 300)
				{
					debugger->Render();
					timeToRender = 0;
				}
			}
		}
	}

	SDL_Quit();

    return 0;
}