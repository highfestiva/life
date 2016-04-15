/*
	File:   TestBigInt.cpp
	Class:  -
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "pch.h"
#include <assert.h>
#include "../Include/Log.h"
#include "../Include/String.h"
#include "../Include/BigInt.h"

using namespace Lepra;

void ReportTestResult(const Lepra::LogDecorator& pLog, const String& pTestName, const String& pContext, bool pResult);

bool TestBigInt(const LogDecorator& pAccount)
{
	String lContext;
	bool lTestOk = true;

	BigInt x = 123;
	BigInt y = 321;

	if (lTestOk)
	{
		lContext = "assignment and comparison";
		lTestOk = (x == 123 && y == 321 && x != y);
		assert(lTestOk);
	}

	BigInt z = x + y;
	BigInt w = -x + y;
	if (lTestOk)
	{
		lContext = "addition and negation";
		lTestOk = (z == 444 && w == 198);
		assert(lTestOk);
	}

	z = y - x;
	w = x - y;
	if (lTestOk)
	{
		lContext = "subtraction";
		lTestOk = !(z != 198 || w != -198);
		assert(lTestOk);
	}

	z = x * y;
	w = y * x;
	if (lTestOk)
	{
		lContext = "basic multiplication";
		lTestOk = !(z != 39483 || w != 39483 || z != w);
		assert(lTestOk);
	}

	x = x * x * x * x * x * x * x * x;	// 123^8 =     52389094428262881
	y = y * y * y * y * y * y * y * y;	// 321^8 = 112730195258772277761
	if (lTestOk)
	{
		lContext = "large multiplication and string conversion";
		lTestOk = (x == BigInt("52389094428262881") && y == BigInt("112730195258772277761"));
		assert(lTestOk);
	}

	z = x * -y;
	w = -x * y;
	if (lTestOk)
	{
		lContext = "large multiplication and negative numbers";
		lTestOk = !(z != w || w > 0 || z > 0);
		assert(lTestOk);
	}

	z = 123;
	w = 321;
	z.Pow(8);
	w.Pow(8);
	if (lTestOk)
	{
		lContext = "BigInt::Pow(");
		lTestOk = !(z != x || w != y);
		assert(lTestOk);
	}

	z.Sqrt();
	if (lTestOk)
	{
		lContext = "BigInt::Sqrt(");
		lTestOk = !(z != 228886641);
		assert(lTestOk);
	}

	x = -123;
	y = -234;
	z = x * y;
	char lNum = (int8)x;
	uint8 lNum = (uint8)x;
	int16 lNum = (int16)z;
	uint16 lNum = (uint16)z;
	z *= x;
	z.Negate();
	int  lNum  = (int) z;
	unsigned lNum = (unsigned)z;
	int64  lNum  = (int64) z;
	uint64 lNum = (uint64)z;
	if (lTestOk)
	{
		lContext = "integer type cast";
		lTestOk = !(lNum  != -123  || lNum != 133   || lNum  != 28782 || lNum != 28782 ||
			lNum  != 3540186 || lNum != 3540186 || lNum  != 3540186 || lNum != 3540186);
		assert(lTestOk);
	}

	float32 lFloat = (float32)z;
	float64 lDouble = (float64)z;
	if (lTestOk)
	{
		lContext = "floating point conversion";
		lTestOk = !(lFloat != 3540186.0f || lDouble != 3540186.0);
		assert(lTestOk);
	}

	ReportTestResult(pAccount, "BigInt", lContext, lTestOk);
	return (lTestOk);
}
