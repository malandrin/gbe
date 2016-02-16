#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "base.h"
#include "gb.h"
#include "debugger.h"

//--------------------------------------------
// --
//--------------------------------------------
Debugger::Debugger(GB& gb) : mMemoryViewer(gb.GetMmu()), mInstructionsViewer(gb.GetMmu())
{
	mWindow = SDL_CreateWindow("GBE Debugger", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_OPENGL);
	mContext = SDL_GL_CreateContext(mWindow);

	ImGui_ImplSdl_Init(mWindow);
}

//--------------------------------------------
// --
//--------------------------------------------
Debugger::~Debugger()
{
	ImGui_ImplSdl_Shutdown();
	SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
}

//--------------------------------------------
// --
//--------------------------------------------
void Debugger::HandleEvent(SDL_Event& _event)
{
	ImGui_ImplSdl_ProcessEvent(&_event);
}

//--------------------------------------------
// --
//--------------------------------------------
void Debugger::Render()
{
	ImGui_ImplSdl_NewFrame(mWindow);

	// ...
	mInstructionsViewer.Render();
	mMemoryViewer.Render();

	// ...
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	glClearColor(1, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui::Render();
	SDL_GL_SwapWindow(mWindow);
}