#pragma once
#ifndef _REGISTERS_VIEWER_H
#define _REGISTERS_VIEWER_H

class CPU;

class RegistersViewer
{
public:

	RegistersViewer(const CPU &_cpu);

	void Render();

private:

	const CPU &mCpu;
};

#endif 
