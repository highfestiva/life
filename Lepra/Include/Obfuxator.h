
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "String.h"



namespace Lepra
{



#define Obfuxator	fO_sx
#define	__O(x)		Obfuxator::de(_T(x))
#define	_O(x, y)	__O(x)
#define _OA(x, y)	astrutil::Encode(_O(x, y))


// This string obfuscator class really has a different name (#defined above) and also only
// manages defuscation. Run the Python program obfuscate.py to encode the strings.
class Obfuxator
{
public:
	static str de(str s);
};


}
