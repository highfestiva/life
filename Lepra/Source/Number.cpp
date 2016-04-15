
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/CyclicArray.h"
#include "../Include/Math.h"
#include "../Include/Number.h"



namespace Lepra
{



str Number::ConvertToPostfixNumber(double pNumber, unsigned decimals)
{
	str lResultString;
	if (pNumber != 0)
	{
		struct PrefixInfo
		{
			int mExponent;
			const char* mPrefix;
		};
		PrefixInfo lPrefixList[] =
		{
			{12, "T"}, {9, "G"}, {6, "M"}, {3, "k"}, {0, ""},
			{-3, "m"}, {-6, "u"}, {-9, "n"}
		};
		double lResult = 0;
		int x;
		for (x = 0; x < (int)LEPRA_ARRAY_COUNT(lPrefixList)-1; ++x)
		{
			lResult = pNumber/::pow(10.0, lPrefixList[x].mExponent);
			if (::fabs(lResult) >= 1)
			{
				break;
			}
		}
		str lFormatString = strutil::IntToString(decimals, 10);
		lResultString = strutil::Format(("%."+lFormatString+"f %s").c_str(), lResult, lPrefixList[x].mPrefix);
	}
	else
	{
		lResultString = "0 ";
	}
	return (lResultString);
}



}
