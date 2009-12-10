
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#pragma once

#include "String.h"



namespace Lepra
{



class Number
{
public:
	static String ConvertToPostfixNumber(double pNumber, unsigned decimals);
};



}
