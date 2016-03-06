#pragma once
#ifndef _MEMORY_VIEWER_H
#define _MEMORY_VIEWER_H

class MMU;

class MemoryViewer
{
public:
    MemoryViewer(const MMU &_mmu);

	void Render();

private:
    static const int mNumCols = 32;

    struct MemInfo
    {
        MemInfo(const u8 *_m, int _s, int _ba, int _ls, string& _mt)
        {
            mem = _m;
            size = _s;
            baseAddr = _ba;
            lineStart = _ls;
            lineEnd = _ls + (int)((_s + mNumCols - 1) / mNumCols);
            memType = _mt;
        }

        const u8 *mem {nullptr};
        int size {0};
        int baseAddr {0};
        int lineStart {0};
        int lineEnd {0};
        string memType;
    };

    const MMU &mMmu;

    vector<MemInfo> mMemInfo;
    int mAddrDigitCount {0};
    int mLineTotalCount {0};
	int mMemSize{0};

    void    CalculateMemInfo  ();

    inline int GetMemInfoByLine (int _line)
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
