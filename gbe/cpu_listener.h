#pragma once
#ifndef _CPU_LISTENER_H
#define _CPU_LISTENER_H

class ICpuListener
{
public:
	virtual void OnStep(int _numCycles) = 0;
};

#endif
