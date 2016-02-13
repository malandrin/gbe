#pragma once
#ifndef _CPU_H
#define _CPU_H

class MMU;

class CPU
{
public:
	void SetMmu(MMU *_mmu) { mMmu = _mmu; }
	void Step();

private:
	MMU *mMmu {nullptr};

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
};

#endif

