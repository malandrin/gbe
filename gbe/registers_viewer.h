#pragma once
#ifndef _REGISTERS_VIEWER_H
#define _REGISTERS_VIEWER_H

class CPU;

class RegistersViewer
{
public:

	RegistersViewer(const CPU &_cpu, const MMU &_mmu);

	void Render();

private:

	const CPU &mCpu;
    const MMU &mMmu;
};

#endif
