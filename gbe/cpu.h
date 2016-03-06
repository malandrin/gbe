#pragma once
#ifndef _CPU_H
#define _CPU_H

class MMU;
class ICpuListener;

class CPU
{
public:
	CPU(MMU &_mmu) : mMmu(_mmu) {}

	void Step();

	u8  GetRegA  () const { return mRegA; }
	u8  GetRegB  () const { return mRegB; }
	u8  GetRegC  () const { return mRegC; }
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

	u8   mRegA {0};
	u8   mRegB {0};
	u8   mRegC {0};
	RegDE mRegDE;
	RegHL mRegHL;
	u16  mRegSP {0};
	u16  mRegPC {0};

	bool mFlagZ {false};
	bool mFlagN {false};
	bool mFlagH {false};
	bool mFlagC {false};

	bool mOnDebugMode{false};

	vector<ICpuListener*> mListeners;

	void ProcessCb(u8 _opcode);
	void InternalStep();
	void IncReg(u8 &_reg);
};

#endif

