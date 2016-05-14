#include "base.h"
#include "rom_walker.h"

static unordered_set<u8> s2byteInstructions { 0x06, 0x0E, 0x16, 0x18, 0x1E, 0x20, 0x28, 0x2E, 0x3E, 0xE0, 0xF0, 0xFE };
static unordered_set<u8> s3byteInstructions { 0x08, 0x11, 0x21, 0x31, 0xCC, 0xCD, 0xEA };

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
        u8 opcode = mRom[pc++];

        // ...
        mCode.insert(pc - 1);

        switch(opcode)
        {
            case 0x18: // JR r8
                pc += (i8)mRom[pc++];
                exit = mCode.find(pc) != mCode.end();
                break;

            case 0x20: // JR NZ, r8
            case 0x28: // JR Z, r8
            {
                u16 dst = (i8)mRom[pc++];
                dst += pc;

                if (mCode.find(dst) == mCode.end())
                    mPaths.push_back(dst);
            }
            break;

            case 0xC9: // RET
            case 0xD9: // RETI
                pc = stack.back();
                stack.pop_back();
                break;

            case 0xCB: // CB
                pc += 1;
                break;

            case 0xCC: // CALL Z, nn
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

            case 0xCD: // CALL nn
            {
                u16 dst = *((u16*)&mRom[pc]);
                stack.push_back(pc + 2);
                pc = dst;
            }
            break;

            default:
                pc += GetInstructionSize(opcode);
                break;
        }
    }
}

//--------------------------------------------
// --
//--------------------------------------------
u8 ROMWalker::GetInstructionSize(u8 _opcode)
{
    if (s2byteInstructions.find(_opcode) != s2byteInstructions.end())
        return 1;

    if (s3byteInstructions.find(_opcode) != s3byteInstructions.end())
        return 2;

    return 0;
}