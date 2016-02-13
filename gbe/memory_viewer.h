#pragma once
#ifndef _MEMORY_VIEWER_H
#define _MEMORY_VIEWER_H

class MMU;

class MemoryViewer
{
public:
    MemoryViewer(const MMU &_mmu) : mMmu(_mmu) {}

	void Render();

private:
	const int mNumCols = 32;

    const MMU &mMmu;
};

#endif
