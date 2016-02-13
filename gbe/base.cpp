#include <sstream>
#include "base.h"

//--------------------------------------------
// --
//--------------------------------------------
string Int2Hex(int _value)
{
	stringstream ss;
	ss << hex << _value;
	return ss.str();
}