#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include "base.h"
#include "gb.h"
#include "memory_viewer.h"
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
	GB gb;
	gb.PowerUp(argv[1], argv[2]);

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

	Debugger debugger(gb);
	SDL_Event event;
	bool done = false;

	while(!done)
	{
		while(SDL_PollEvent(&event))
		{
			// TODO: comprobar para que ventana es el evento
			debugger.HandleEvent(event);

			if (event.type == SDL_QUIT)
				done = true;
		}

		debugger.Render();
	}

	SDL_Quit();

    return 0;
}