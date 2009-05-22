
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include "../Include/Math.h"
#include "../Include/Number.h"



namespace Lepra
{



String Number::ConvertToPostfixNumber(double pNumber, unsigned decimals)
{
	String lResultString;
	if (pNumber != 0)
	{
		struct PrefixInfo
		{
			int mExponent;
			const tchar* mPrefix;
		};
		PrefixInfo lPrefixList[] =
		{
			{12, _T("T")}, {9, _T("G")}, {6, _T("M")}, {3, _T("k")}, {0, _T("")},
			{-3, _T("m")}, {-6, _T("u")}, {-9, _T("n")}
		};
		double lResult = 0;
		int x;
		for (x = 0; x < (int)((sizeof(lPrefixList)-1)/sizeof(lPrefixList[0])); ++x)
		{
			lResult = pNumber/::pow(10.0, lPrefixList[x].mExponent);
			if (::fabs(lResult) >= 1)
			{
				break;
			}
		}
		String lFormatString = StringUtility::IntToString(decimals, 10);
		lResultString = StringUtility::Format((_T("%.")+lFormatString+_T("f %s")).c_str(), lResult, lPrefixList[x].mPrefix);
	}
	else
	{
		lResultString = _T("0 ");
	}
	return (lResultString);
}



}
