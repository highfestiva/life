
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#ifndef LEPRA_NUMBER_H
#define LEPRA_NUMBER_H



#include "String.h"



namespace Lepra
{



class Number
{
public:
	static String ConvertToPostfixNumber(double pNumber, unsigned decimals);
};



}



#endif // !LEPRA_NUMBER_H
