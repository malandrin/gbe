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
	Debugger *debugger {nullptr};

	if ((argn > 3) && (string(argv[3]) == "-debugger"))
	{
		debugger = new Debugger(gb);
		gb.GetCpu().Break();
	}

	// ...
	SDL_Event event;
	bool done = false;

	while(!done)
	{
		while(SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				done = true;
			else
			{
				if (debugger != nullptr)
					debugger->HandleEvent(event);

				machine.HandleEvent(event);
			}
		}

		machine.Update();

		if (debugger != nullptr)
			debugger->Render();
	}

	if (debugger != nullptr)
		delete debugger;

	SDL_Quit();

    return 0;
}