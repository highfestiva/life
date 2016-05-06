/*
	File:   TestBigInt.cpp
	Class:  -
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "pch.h"
#include <assert.h>
#include "../include/log.h"
#include "../include/string.h"
#include "../include/BigInt.h"

using namespace lepra;

void ReportTestResult(const lepra::LogDecorator& log, const String& test_name, const String& context, bool result);

bool TestBigInt(const LogDecorator& account) {
	String _context;
	bool test_ok = true;

	BigInt x = 123;
	BigInt y = 321;

	if (test_ok) {
		_context = "assignment and comparison";
		test_ok = (x == 123 && y == 321 && x != y);
		assert(test_ok);
	}

	BigInt z = x + y;
	BigInt w = -x + y;
	if (test_ok) {
		_context = "addition and negation";
		test_ok = (z == 444 && w == 198);
		assert(test_ok);
	}

	z = y - x;
	w = x - y;
	if (test_ok) {
		_context = "subtraction";
		test_ok = !(z != 198 || w != -198);
		assert(test_ok);
	}

	z = x * y;
	w = y * x;
	if (test_ok) {
		_context = "basic multiplication";
		test_ok = !(z != 39483 || w != 39483 || z != w);
		assert(test_ok);
	}

	x = x * x * x * x * x * x * x * x;	// 123^8 =     52389094428262881
	y = y * y * y * y * y * y * y * y;	// 321^8 = 112730195258772277761
	if (test_ok) {
		_context = "large multiplication and string conversion";
		test_ok = (x == BigInt("52389094428262881") && y == BigInt("112730195258772277761"));
		assert(test_ok);
	}

	z = x * -y;
	w = -x * y;
	if (test_ok) {
		_context = "large multiplication and negative numbers";
		test_ok = !(z != w || w > 0 || z > 0);
		assert(test_ok);
	}

	z = 123;
	w = 321;
	z.Pow(8);
	w.Pow(8);
	if (test_ok) {
		_context = "BigInt::Pow(");
		test_ok = !(z != x || w != y);
		assert(test_ok);
	}

	z.Sqrt();
	if (test_ok) {
		_context = "BigInt::Sqrt(");
		test_ok = !(z != 228886641);
		assert(test_ok);
	}

	x = -123;
	y = -234;
	z = x * y;
	char num = (int8)x;
	uint8 num = (uint8)x;
	int16 num = (int16)z;
	uint16 num = (uint16)z;
	z *= x;
	z.Negate();
	int  num  = (int) z;
	unsigned num = (unsigned)z;
	int64  num  = (int64) z;
	uint64 num = (uint64)z;
	if (test_ok) {
		_context = "integer type cast";
		test_ok = !(num  != -123  || num != 133   || num  != 28782 || num != 28782 ||
			num  != 3540186 || num != 3540186 || num  != 3540186 || num != 3540186);
		assert(test_ok);
	}

	float32 __f = (float32)z;
	float64 d = (float64)z;
	if (test_ok) {
		_context = "floating point conversion";
		test_ok = !(__f != 3540186.0f || d != 3540186.0);
		assert(test_ok);
	}

	ReportTestResult(account, "BigInt", _context, test_ok);
	return (test_ok);
}
