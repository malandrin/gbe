#pragma once
#ifndef _MEMORY_VIEWER_H
#define _MEMORY_VIEWER_H

class MMU;

class MemoryViewer
{
public:
         MemoryViewer   (const MMU &_mmu);

	void Render         ();

private:
    static const int mNumCols = 32;

    struct MemInfo
    {
        MemInfo(const u8 *_m, int _s, int _ba, int _ls, string& _mt)
        {
            mem = _m;
            size = _s;
            baseAddr = _ba;
            memType = _mt;

            SetLineStart(_ls);
        }

        void SetLineStart(int _ls)
        {
            lineStart = _ls;
            lineEnd = _ls + (int)((size + mNumCols - 1) / mNumCols);
        }

        MemInfo& operator = (const MemInfo &_mi)
        {
            mem = _mi.mem;
            size = _mi.size;
            baseAddr = _mi.baseAddr;
            lineStart = _mi.lineStart;
            lineEnd = _mi.lineEnd;
            memType = _mi.memType;

            return *this;
        }

        const u8 *mem { nullptr };
        int size { 0 };
        int baseAddr { 0 };
        int lineStart { 0 };
        int lineEnd { 0 };
        string memType;
    };

    const MMU       &mMmu;
    vector<MemInfo> mMemInfo;
    vector<MemInfo> mRomBanksMemInfo;
    int             mLineTotalCount { 0 };
	int             mMemSize { 0 };
    bool            mShowingBootRom { false };
    int             mPrevRomBank { 0 };
    int             mRomBank1Idx { 0 };

    // ...
    void CalculateMemInfo  ();

    inline int GetMemInfoByLine  (int _line)
    {
		for (u32 i = 0; i < mMemInfo.size(); ++i)
		{
			if ((_line >= mMemInfo[i].lineStart) && (_line < mMemInfo[i].lineEnd))
				return i;
        }

        return -1;
    }
};

#endif
