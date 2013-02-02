
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <math.h>
#include <vector>



namespace Lepra
{



const double PI = 3.1415926535897932384626433832795028841971693993751;
const float PIF = 3.1415926535897932384626433832795028841971693993751f;

// Alex/TODO: These values are probably too low for practical use. My experience from
//            MindArk is that 1e-15 is the lowest practical epsilon value for floats.
//            These experiences are based on the result of vector operations like the
//            dot product and how to compare when two vectors are parallell or perpendicular.

const double eps = 1e-300;	// 1e-324 is smallest, but denormalized. This cuts us some slack.
const float eps_f = 1e-34f;	// 1e-45 is smallest, but denormalized. This cuts us some slack.


template<class _TVarType>
class MathTraits
{
};

// Specialization...
template<>
class MathTraits<float>
{
public:
	inline static float Eps() { return 1e-34f; }
	inline static float FullEps() { return 1e-7f; }
	inline static float Pi() { return 3.1415926535897932384626433832795028841971693993751f; }
};

template<>
class MathTraits<double>
{
public:
	inline static double Eps() { return 1e-300; }
	inline static double FullEps() { return 1e-16f; }
	inline static double Pi() { return 3.1415926535897932384626433832795028841971693993751; }
};



class Math
{
public:
	static inline float Deg2Rad(float pDeg)
	{
		return pDeg * PIF / 180.0f;
	}
	static inline float Rad2Deg(float pRad)
	{
		return pRad * 180.0f / PIF;
	}

	// Places the (radian) angles in the same "range" so that subtraction will return a value
	// between -PI and PI. Solves the problem that arises when one angle is +2*PI and the other
	// is -0.01 - they are very close but a plain subtraction would not return a value close to
	// zero, but a value that is greater than 2*PI...
	template<class _Type> static void RangeAngles(_Type& pAngle1, _Type& pAngle2);

	template<class _Type, class _TimeType> static inline _Type Lerp(_Type a, _Type b, _TimeType t)
	{
		return (a + (_Type)(t*(b-a)));
	}
	template<class _T> static inline _T Lerp(_T a, _T b, _T t)
	{
		return (a + t*(b-a));
	}

	template<class _Type> static inline _Type GetIterateLerpTime(_Type pTarget, _Type pIterateTime)
	{
		return (std::min(pTarget * (_Type)10 * pIterateTime, (_Type)0.95));
	}

	template<class _Type> static _Type Clamp(_Type value, _Type min, _Type max)
	{
		if (value < min)
		{
			value = min;
		}
		else if (value > max)
		{
			value = max;
		}
		return (value);
	}
	
	template<class _Type> static bool IsInRange(_Type value, _Type min, _Type max)
	{
		return (value >= min && value <= max);
	}
	template<class _Type> static bool IsEpsEqual(_Type value1, _Type value2, _Type eps = MathTraits<_Type>::FullEps())
	{
		return (IsInRange(value1, value2-eps, value2+eps));
	}

	// Modulo with a slightly different definition for negative x. 
	// The return value is always between 0 and |y| - 1.
	// Example, standard modulo vs Mod():
	// 
	// -1 % 3 == -1    // Negative result!
	//
	// Mod(-1, 3) == 2 // Positive result!
	static int Mod(int x, int y);
	static float Mod(float x, float y);
	static double Mod(double x, double y);
	static float Mod(float x, float pMin, float pMax);

	// Returns the greatest common divider between a and b using the 
	// Euclidean extended algorithm.
	// If you are interested in a solution to the Diofantic equation 
	// ax + by = gcd(a, b), you should pass pointers to x and y. If not,
	// you can just set them to 0 (NULL).
	static int GCD(int a, int b, int* x = 0, int* y = 0);

	static int Pow(int x, int y);

	// The base 2 logarithm of x, rounded down to nearest lower integer.
	static int Log2(int x);

	template<class _T> static _T Round(_T pValue);

	template<class _T> static _T Acos(_T pAngle);

	template<class _T> static bool CalculateRoot(const _T& a, const _T& b, const _T& c, _T& pRoot);

	template<class _T, class _Vector> static _T CalculateMedian(const _Vector& pArray);
	// Not necessarily standard deviation, if not providing arithmetic mean.
	template<class _ReturnType, class _ArrayType, class _Vector> static _ReturnType CalculateDeviation(const _Vector& pArray, const _ArrayType& pMean);
	template<class _ReturnType, class _ArrayType, class _Vector> static _ReturnType CalculateNegativeDeviation(const _Vector& pArray, const _ArrayType& pMean);
};



template<class _Type> void Math::RangeAngles(_Type& pAngle1, _Type& pAngle2)
{
	pAngle1 = Mod(pAngle1, (_Type)-PI, (_Type)PI);
	pAngle2 = Mod(pAngle2, (_Type)-PI, (_Type)PI);
	_Type lDiff = pAngle1-pAngle2;
	if (lDiff > (_Type)PI)
	{
		pAngle2 += (_Type)(PI*2);
	}
	else if (lDiff < (_Type)-PI)
	{
		pAngle1 += (_Type)(PI*2);
	}
}

template<class _T> _T Math::Round(_T pValue)
{
	return (pValue > (_T)0)? ::floor(pValue + (_T)0.5) : ::ceil(pValue - (_T)0.5);
}

template<class _T> _T Math::Acos(_T pAngle)
{
	if (pAngle + MathTraits<_T>::FullEps() >= 1)
	{
		return 0;
	}
	if (pAngle - MathTraits<_T>::FullEps() <= -1)
	{
		return MathTraits<_T>::Pi();
	}
	return ::acos(pAngle);
}

template<class _T> static bool Math::CalculateRoot(const _T& a, const _T& b, const _T& c, _T& pRoot)
{
	// Quaderatic formula:
	// ax^2 + bx + c = 0
	// =>
	//     -b +- sqrt(b^2 - 4ac)
	// x = ---------------------
	//             2a
	const float b2 = b*b;
	const float _4ac = 4*a*c;
	if (b2 < _4ac || !a)	// Does not compute.
	{
		return false;
	}
	pRoot = (-b + sqrt(b2 - _4ac)) / (2*a);
	return true;
}

template<class _T, class _Vector> _T Math::CalculateMedian(const _Vector& pArray)
{
	// The following code is public domain.
	// Algorithm by Torben Mogensen, implementation by N. Devillard.
	// Modified by Jonas Byström ("C++ templatified" and some code standard).
	// This code in public domain.

	const int lArraySize = (int)pArray.size();
	int x, less, greater, equal;
	_T min, max, guess, maxltguess, mingtguess;

	min = max = pArray[0];
	for (x = 1; x < lArraySize; ++x)
	{
		if (pArray[x] < min)
		{
			min = pArray[x];
		}
		if (pArray[x] > max)
		{
			max = pArray[x];
		}
	}

	for (;;)
	{
		guess = (min+max)/2;
		less = 0; greater = 0; equal = 0;
		maxltguess = min;
		mingtguess = max;
		for (x = 0; x < lArraySize; ++x)
		{
			if (pArray[x] < guess)
			{
				++less;
				if (pArray[x] > maxltguess)
				{
					maxltguess = pArray[x];
				}
			}
			else if (pArray[x] > guess)
			{
				++greater;
				if (pArray[x] < mingtguess)
				{
					mingtguess = pArray[x];
				}
			}
			else
			{
				++equal;
			}
		}
		if (less <= (lArraySize+1)/2 && greater <= (lArraySize+1)/2)
		{
			break;
		}
		else if (less > greater)
		{
			max = maxltguess;
		}
		else
		{
			min = mingtguess;
		}
	}
	if (less >= (lArraySize+1)/2)
	{
		return maxltguess;
	}
	else if (less+equal >= (lArraySize+1)/2)
	{
		return guess;
	}
	else
	{
		return mingtguess;
	}
}

template<class _ReturnType, class _ArrayType, class _Vector> _ReturnType Math::CalculateDeviation(const _Vector& pArray, const _ArrayType& pMean)
{
	const int lArraySize = (int)pArray.size();
	_ArrayType lSum = 0;
	for (int x = 0; x < lArraySize; ++x)
	{
		const _ArrayType lDiff = pArray[x]-pMean;
		lSum += lDiff*lDiff;
	}
	_ReturnType lDeviation = ::sqrt((_ReturnType)lSum/lArraySize);
	return (lDeviation);
}

template<class _ReturnType, class _ArrayType, class _Vector> _ReturnType Math::CalculateNegativeDeviation(const _Vector& pArray, const _ArrayType& pMean)
{
	const int lArraySize = (int)pArray.size();
	_ArrayType lSum = 0;
	int lNegativeCount = 0;
	for (int x = 0; x < lArraySize; ++x)
	{
		_ArrayType lDiff = pArray[x]-pMean;
		if (lDiff < 0)
		{
			++lNegativeCount;
			lSum += lDiff*lDiff;
		}
	}
	_ReturnType lDeviation;
	if (lNegativeCount)
	{
		lDeviation = ::pow((_ReturnType)lSum/lNegativeCount, (_ReturnType)0.5);
	}
	else
	{
		lDeviation = 0;
	}
	return (lDeviation);
}



}

