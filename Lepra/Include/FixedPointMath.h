/*
	Class:  FixedPointMath
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	The purpose of this class is to collect some common mathematical functions
	working on fixed point numbers (opposed to floating point), which in turn
	are mainly used in software renderers and such.

	Some of the ideas used in this class come from Chris Hecker's homepage,
	and his papers on texture mapping: http://www.d6.com/users/checker/

	In case something is unclear, and if you don't understand some of the algorithms
	used here, please read these papers.
*/

#ifndef FIXEDPOINTMATH_H
#define FIXEDPOINTMATH_H

#include "LepraTypes.h"

namespace Lepra
{

class FixedPointMath
{
public:
	typedef int Fixed28_4;		// 24 bits integer part, 4 bits fractional.
	typedef int Fixed16_16;	// 16 bits integer part, 16 bits fractional.

	static inline void FloorDivMod(int pNumerator, int pDenominator, int& pFloor, int& pMod);
	
	static inline Fixed28_4 FloatToFixed28_4(float64 pValue);
	static inline float64 Fixed28_4ToFloat(Fixed28_4 pValue);
		
	static inline Fixed28_4 Fixed28_4Mul(Fixed28_4 pA, Fixed28_4 pB);
	static inline int Ceil28_4(Fixed28_4 pValue);

	static inline Fixed16_16 FloatToFixed16_16(float64 pValue);
	static inline float64 Fixed16_16ToFloat(Fixed16_16 pValue);

private:

	static inline unsigned AddWithCarry(unsigned pA, unsigned pB, unsigned& pCarry);
};

void FixedPointMath::FloorDivMod(int pNumerator, int pDenominator, int& pFloor, int& pMod)
{
	//assert(pDenominator > 0);

	if (pNumerator >= 0)
	{
		// Standard operators are ok to use...
		pFloor = pNumerator / pDenominator;
		pMod = pNumerator % pDenominator;
	}
	else
	{
		// Numerator is negative, do it right...
		pFloor = -((-pNumerator) / pDenominator);
		pMod = (-pNumerator) % pDenominator;

		if (pMod != 0)
		{
			pFloor--;
			pMod = pDenominator - pMod;
		}
	}
}

FixedPointMath::Fixed28_4 FixedPointMath::FloatToFixed28_4(float64 pValue)
{
	return (Fixed28_4)(pValue * 16.0f);
}

float64 FixedPointMath::Fixed28_4ToFloat(Fixed28_4 pValue)
{
	return (float64)pValue / 16.0f;
}

FixedPointMath::Fixed16_16 FixedPointMath::FloatToFixed16_16(float64 pValue)
{
	return (Fixed16_16)(pValue * 65536.0f);
}

float64 FixedPointMath::Fixed16_16ToFloat(Fixed16_16 pValue)
{
	return (float64)pValue / 65536.0f;
}

FixedPointMath::Fixed28_4 FixedPointMath::Fixed28_4Mul(Fixed28_4 pA, Fixed28_4 pB)
{
	return (Fixed28_4)(((int64)pA * (int64)pB) >> 4);

/*
	// This function is totally my own, and not Chris Hecker's. It performs
	// a 64-bit multiplication using two 32-bit 28.4 fixed point numbers.
	// The result from this mul will be a 56.8-bit value, which will be shifted
	// down to a 56.4-bit value. Obviously, a 56 bits integer value will never
	// fit in 28 bits... In case any of the 28 upper bits of this 56-bit result
	// are actually used, we have an overflow and the result can be considered
	// to have "wrapped around".

	unsigned lSignA = (pA & 0x80000000);
	unsigned lSignB = (pB & 0x80000000);
	unsigned lExpectedSign = (lSignA ^ lSignB);

	// Make sure that there is no sign when multiplicating.
	if (lSignA != 0)
	{
		pA = -pA;
	}
	if (lSignB != 0)
	{
		pB = -pB;
	}

	unsigned lAH = (unsigned)pA >> 16;
	unsigned lBH = (unsigned)pB >> 16;
	unsigned lAL = (unsigned)pA & 0xFFFF;
	unsigned lBL = (unsigned)pB & 0xFFFF;

	// Multiplication using the same technique as you use in school.
	unsigned lBLAL = lBL * lAL;
	unsigned lBLAH = lBL * lAH;
	unsigned lBHAL = lBH * lAL;
	unsigned lBHAH = lBH * lAH;

	unsigned lCarry;

	// lT1 = lBLAL + lBLAH
	unsigned lT1[4];
	lT1[0] = lBLAL & 0xFFFF;	// The lowest 16 bits...
	lT1[1] = AddWithCarry(lBLAL >> 16, lBLAH & 0xFFFF, lCarry);
	lT1[2] = AddWithCarry(lBLAH >> 16, lCarry, lCarry);
	lT1[3] = lCarry;

	// lT2 = lBHAL + lBHAH
	unsigned lT2[4];
	lT2[0] = lBHAL & 0xFFFF;	// The lowest 16 bits...
	lT2[1] = AddWithCarry(lBHAL >> 16, lBHAH & 0xFFFF, lCarry);
	lT2[2] = AddWithCarry(lBHAH >> 16, lCarry, lCarry);
	lT2[3] = lCarry;

	// And now the final result...
	unsigned lTemp1 = AddWithCarry(lT1[1], lT2[0], lCarry);
	unsigned lTemp2 = AddWithCarry(lT1[2], lT2[1] + lCarry, lCarry);

	if (lExpectedSign == 0)
	{
		return (lT1[0] >> 4) | (lTemp1 << 12) | ((lTemp2 << 28) & 0x7FFFFFFF);
	}
	else
	{
		return -(int)((lT1[0] >> 4) | (lTemp1 << 12) | ((lTemp2 << 28) & 0x7FFFFFFF));
	}
*/
}

int FixedPointMath::Ceil28_4(Fixed28_4 pValue)
{
	int lReturnValue;
	int lNumerator = pValue - 1 + 16;

	if (lNumerator >= 0)
	{
		lReturnValue = (lNumerator >> 4); // RetVal = Numerator / 16
	}
	else
	{
		lReturnValue = -((-lNumerator) >> 4); // RetVal = -Numerator / 16
		lReturnValue -= (-(lNumerator & 0xF)) ? 1 : 0;
	}

	return lReturnValue;
}

unsigned FixedPointMath::AddWithCarry(unsigned pA, unsigned pB, unsigned& pCarry)
{
	unsigned lResult = pA + pB;
	pCarry = lResult >> 16;
	return lResult & 0xFFFF;
}

} // End namespace.

#endif