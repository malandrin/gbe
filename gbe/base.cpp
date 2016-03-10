#include <sstream>
#include <algorithm>
#include "base.h"

//--------------------------------------------
// --
//--------------------------------------------
string Int2Hex(int _value)
{
	stringstream ss;
	ss << hex << _value;
	string str = "$" + ss.str();
    string dst;

    dst.resize(str.size());
    transform(str.begin(), str.end(), dst.begin(), ::toupper);

    return dst;
}