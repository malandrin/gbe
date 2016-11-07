#include "base.h"
#include "opcodes_info.h"
#include "rom_walker.h"

//--------------------------------------------
// --
//--------------------------------------------
ROMWalker::ROMWalker(const u8* _rom, int _romSize, u16 _addr) : mRom(_rom), mRomSize(_romSize)
{
    Analyze(_addr);
}

//--------------------------------------------
// --
//--------------------------------------------
void ROMWalker::Analyze(u16 _addr, bool _clear)
{
    if (_clear)
        mCode.clear();

    mPaths.push_back(_addr);

    while(!mPaths.empty())
    {
        AnalyzePath(mPaths[0]);
        mPaths.erase(mPaths.begin());
    }
}

//--------------------------------------------
// --
//--------------------------------------------
bool ROMWalker::IsCode(u16 _addr)
{
    return mCode.find(_addr) != mCode.end();
}

//--------------------------------------------
// --
//--------------------------------------------
void ROMWalker::AnalyzePath(u16 _addr)
{
    u16 pc = _addr;
    vector<u16> stack;
    bool exit = false;

    while((pc < mRomSize) && !exit)
    {
        mCode.insert(pc);
        u8 opcode = mRom[pc++];

        if (opcode == 0xCB)
            pc += 1;
        else
        {
            switch(OpcodesInfo::primary[opcode].jumpType)
            {
                case OpcodesInfo::JumpI8:
                    pc += (i8)mRom[pc++];
                    exit = mCode.find(pc) != mCode.end();
                    break;

                case OpcodesInfo::JumpCondI8:
                {
                    u16 dst = (i8)mRom[pc++] + pc + 1;

                    if (mCode.find(dst) == mCode.end())
                        mPaths.push_back(dst);
                }
                break;

                case OpcodesInfo::JumpU16:
                    pc = *((u16*)&mRom[pc]);
                    exit = mCode.find(pc) != mCode.end();
                    break;                

                case OpcodesInfo::JumpCondU16:
                {
                    u16 dst = *((u16*)&mRom[pc]);
                    pc += 2;

                    if (mCode.find(dst) == mCode.end())
                        mPaths.push_back(dst);
                }
                break;

                case OpcodesInfo::Ret:
                    if (!stack.empty())
                    {
                        pc = stack.back();
                        stack.pop_back();
                    }
                    break;

                case OpcodesInfo::CallCond:
                {
                    u16 dst = *((u16*)&mRom[pc]);

                    if (mCode.find(dst) == mCode.end())
                        mPaths.push_back(dst);

                    pc += 2;
                    if (mCode.find(pc) == mCode.end())
                        stack.push_back(pc);
                    else
                        exit = true;
                }
                break;

                case OpcodesInfo::Call:
                {
                    u16 dst = *((u16*)&mRom[pc]);

                    if (mCode.find(dst) == mCode.end())
                    {
                        stack.push_back(pc + 2);
                        pc = dst;
                    }
                }
                break;

                default:
                    pc += OpcodesInfo::primary[opcode].bytesLength - 1;
                    break;
            }
        }
    }
}
