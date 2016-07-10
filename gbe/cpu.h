#pragma once
#ifndef _CPU_H
#define _CPU_H

#include "cpu_listener.h"

class MMU;

class CPUDummyListener : public ICpuListener
{
    void OnStep(int _numCycles) {};
};

class CPU
{
public:
	    CPU(MMU &_mmu);
       ~CPU();

	int Step();

	u8  GetRegA  () const { return mRegA; }
	u8  GetRegB  () const { return mRegBC.b; }
	u8  GetRegC  () const { return mRegBC.c; }
	u16 GetRegDE () const { return mRegDE.de; }
	u8  GetRegD  () const { return mRegDE.d; }
	u8  GetRegE  () const { return mRegDE.e; }
	u16 GetRegHL () const { return mRegHL.hl; }
	u8  GetRegH  () const { return mRegHL.h; }
	u8  GetRegL  () const { return mRegHL.l; }
	u16 GetRegSP () const { return mRegSP; }
	u16 GetRegPC () const { return mRegPC; }

	bool GetFlagZ () const { return mFlagZ; }
	bool GetFlagN () const { return mFlagN; }
	bool GetFlagH () const { return mFlagH; }
	bool GetFlagC () const { return mFlagC; }

	bool IsOnDebugMode () const { return mOnDebugMode; }
	void Break		   () { mOnDebugMode = true; }
	void Continue	   () { mOnDebugMode = false; }

    void SetStateAfterBoot  ();

	void AddListener (ICpuListener *_listener);

private:
	MMU &mMmu;

	union RegHL
	{
		u16 hl{ 0 };
		struct
		{
			u8 l;
			u8 h;
		};
	};

	union RegDE
	{
		u16 de{ 0 };
		struct
		{
			u8 e;
			u8 d;
		};
	};

	union RegBC
	{
		u16 bc{ 0 };
		struct
		{
			u8 c;
			u8 b;
		};
	};

	u8   mRegA {0};
	RegBC mRegBC;
	RegDE mRegDE;
	RegHL mRegHL;
	u16  mRegSP {0};
	u16  mRegPC {0};

	bool mFlagZ {false};
	bool mFlagN {false};
	bool mFlagH {false};
	bool mFlagC {false};

	bool mOnDebugMode{false};

    ICpuListener* mListeners[2] {nullptr};
    CPUDummyListener* mDummyListener {nullptr};

	int ProcessCb(u8 _opcode);
	int  InternalStep();

	void IncReg(u8 &_reg);
	void DecReg(u8 &_reg);
	void RotateLeft(u8 &_reg);

	void SubRegA(u8 _val);
	void AddRegA(u8 _val);
	void CpRegA(u8 _val);
	void AndRegA(u8 _val);

	void Push(u16 _val);
	u16  Pop();
};

#endif

