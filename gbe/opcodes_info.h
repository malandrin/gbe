#pragma once

#include <functional>

class GB;

namespace OpcodesInfo
{
    enum JumpType
    {
        None = 0,
        JumpI8,
        JumpCondI8,
        JumpU16,
        JumpCondU16,
        Ret,
        Call,
        CallCond
    };

    struct InfoPrimary
    {
        int bytesLength;
        int cyclesDuration;
        JumpType jumpType;
        function<string(GB&, u16)> asmCode;
    };

    struct InfoCB
    {
        int cyclesDuration;
        function<string(GB&, u16)> asmCode;
    };

    extern OpcodesInfo::InfoPrimary primary[256];
    extern OpcodesInfo::InfoCB      cb[256];
}
