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
Debugger::Debugger(GB& gb) : mMemoryViewer(gb.GetMmu()), mInstructionsViewer(gb, *this), mGeneralViewer(gb.GetCpu(), gb.GetMmu()), mVRamViewer(gb.GetMmu()), mCpu(gb.GetCpu())
{
	mWindow = SDL_CreateWindow("GBE Debugger", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_OPENGL);
	mContext = SDL_GL_CreateContext(mWindow);

	ImGui_ImplSdl_Init(mWindow);

    mVRamViewer.Init();
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
	if (_event.window.windowID != SDL_GetWindowID(mWindow))
		return;

	ImGui_ImplSdl_ProcessEvent(&_event);
}

//--------------------------------------------
// --
//--------------------------------------------
void Debugger::Update(int _numCycles)
{
    if (mCpu.IsOnDebugMode())
        return;

    if (mNumCyclesToExecute <= 0)
        mNumCyclesToExecute = _numCycles;

    while (mNumCyclesToExecute > 0)
    {
        mNumCyclesToExecute -= mCpu.Step();

        if (mInstructionsViewer.OnStep())
        {
            Break();
            break;
        }
    }
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
	mGeneralViewer.Render();
    mVRamViewer.Render();

	// ...
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	glClearColor(1, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui::Render();
	SDL_GL_SwapWindow(mWindow);
}

//--------------------------------------------
// --
//--------------------------------------------
void Debugger::Break()
{
    mCpu.Break();
}

//--------------------------------------------
// --
//--------------------------------------------
void Debugger::Continue()
{
    mCpu.Continue();
}

//--------------------------------------------
// --
//--------------------------------------------
void Debugger::Step()
{
    mNumCyclesToExecute -= mCpu.Step();
    mInstructionsViewer.OnStep();
}
