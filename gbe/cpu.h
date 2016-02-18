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
	u8  GetRegD  () const { return mRegD; }
	u8  GetRegE  () const { return mRegE; }
	u8  GetRegH  () const { return mRegH; }
	u8  GetRegL  () const { return mRegL; }
	u16 GetRegSP () const { return mRegSP; }
	u16 GetRegPC () const { return mRegPC; }

	bool GetFlagZ () const { return mFlagZ; }
	bool GetFlagN () const { return mFlagN; }
	bool GetFlagH () const { return mFlagH; }
	bool GetFlagC () const { return mFlagC; }

	void AddListener (ICpuListener *_listener);

private:
	MMU &mMmu;

	u8   mRegA {0};
	u8   mRegB {0};
	u8   mRegC {0};
	u8   mRegD {0};
	u8   mRegE {0};
	u8   mRegH {0};
	u8   mRegL {0};
	u16  mRegSP {0};
	u16  mRegPC {0};

	bool mFlagZ {false};
	bool mFlagN {false};
	bool mFlagH {false};
	bool mFlagC {false};

	vector<ICpuListener*> mListeners;

	void ProcessCb(u8 _opcode);

	inline u16 U8sToU16(u8 _l, u8 _h)
	{
		return (_h << 8) | _l;
	}

	inline void U16ToU8s(u16 _val, u8 &_l, u8 &_h)
	{
		_h = _val >> 8;
		_l = _val & 0x00FF;
	}
};

#endif

