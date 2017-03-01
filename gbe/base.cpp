#include <sstream>
#include <algorithm>
#include <sys/stat.h>
#include "base.h"

//--------------------------------------------
// --
//--------------------------------------------
string Int2Hex(int _value, int _minLength, bool _addSign)
{
	stringstream ss;
	ss << hex << _value;
	string str = (_addSign ? "$" : "") + ss.str();
    string dst;

    dst.resize(str.size());
    transform(str.begin(), str.end(), dst.begin(), ::toupper);

    if ((int)dst.length() < _minLength)
    {
        for (int i = 0; i < (_minLength - (int)dst.length()); ++i)
            dst = "0" + dst;
    }

    return dst;
}

//--------------------------------------------
// --
//--------------------------------------------
bool FileExists(const string &_filename)
{
    struct stat buffer;
    return (stat(_filename.c_str(), &buffer) == 0);
}