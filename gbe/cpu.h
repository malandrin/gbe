#pragma once
#ifndef _CPU_H
#define _CPU_H

class MMU;
class Timer;

class CPU
{
public:
	    CPU      (MMU &_mmu, Timer &_timer);
       ~CPU      ();

	int Step     ();

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

    u16 GetSPStartAddr () const { return mSPStartAddr; }

	bool GetFlagZ () const { return mFlagZ; }
	bool GetFlagN () const { return mFlagN; }
	bool GetFlagH () const { return mFlagH; }
	bool GetFlagC () const { return mFlagC; }

	bool IsOnDebugMode () const { return mOnDebugMode; }
	void Break		   () { mOnDebugMode = true; }
	void Continue	   () { mOnDebugMode = false; }

    void ExitStopped   () { mStopped = false; }

    void SetStateAfterBoot  ();

private:
	MMU     &mMmu;
    Timer   &mTimer;

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

	u8    mRegA {0};
	RegBC mRegBC;
	RegDE mRegDE;
	RegHL mRegHL;
	u16   mRegSP {0};
	u16   mRegPC {0};

    u16   mSPStartAddr {0};

	bool mFlagZ {false};
	bool mFlagN {false};
	bool mFlagH {false};
	bool mFlagC {false};
    bool mIME {true}; // Interrupt master enabled
    bool mDI {false}; // Disable interrupts
    bool mEI {false}; // Enable interrupts

	bool mOnDebugMode{false};

    bool  mHalted {false};
    bool  mStopped {false};

    // ...
    int  ProcessCb(u8 _opcode);
	int  InternalStep();

	void IncReg(u8 &_reg);
	void DecReg(u8 &_reg);
	void RotateLeft(u8 &_reg);
    void RotateLeftC(u8 &_reg);
    void RotateRight(u8 &_reg);
    void RotateRightC(u8 &_reg);

	void SubRegA(u8 _val);
	void AddRegA(u8 _val);
	void CpRegA(u8 _val);
	void AndRegA(u8 _val);
    void XorRegA(u8 _val);
    void OrRegA(u8 _val);

    void AddReg(u16 &_dest, u16 _orig);

    void Bit(u8 _bit, u8 _reg);
    void Res(u8 _bit, u8 &_reg);
    void Set(u8 _bit, u8 &_reg);
    void Swap(u8 &_reg);
    void Sla(u8 &_reg);
    void Sra(u8 &_reg);
    void Srl(u8 &_reg);

	void Push(u16 _val);
	u16  Pop();

    bool ManageInterrupt    (int _interruptBit, u16 _interruptAddr, u8 _iflags, u8 _ienable);
    u8   GetFlagsAsU8       ();
    void SetFlagsFromU8     (u8 _val);
};

#endif

