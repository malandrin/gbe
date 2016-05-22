#pragma once
#ifndef _MACHINE_H
#define _MACHINE_H

class GB;

class Machine
{
public:
	Machine(GB& _gb);
	~Machine();

	void Update();
    void Render();
	void HandleEvent(SDL_Event& _event);

private:
	GB& mGb;
	CPU& mCpu;
    SDL_Window*   mWindow {nullptr};
    SDL_Renderer* mRenderer {nullptr};
};

#endif 
