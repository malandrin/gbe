#pragma once
#ifndef _GENERAL_VIEWER_H
#define _GENERAL_VIEWER_H

class CPU;

class GeneralViewer
{
public:

	GeneralViewer(const CPU &_cpu, const MMU &_mmu);

	void Render();

private:

	const CPU &mCpu;
    const MMU &mMmu;

    void RenderRegistersWnd();
    void RenderStackWnd();
};

#endif
