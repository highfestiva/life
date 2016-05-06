
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepraassert.h"
#include "lepratypes.h"
#include <cmath>
#include <vector>



namespace lepra {



const double PI = 3.1415926535897932384626433832795028841971693993751;
const float PIF = 3.1415926535897932384626433832795028841971693993751f;

// Alex/TODO: These values are probably too low for practical use. My experience from
//            MindArk is that 1e-15 is the lowest practical epsilon value for floats.
//            These experiences are based on the result of vector operations like the
//            dot product and how to compare when two vectors are parallell or perpendicular.

const double eps = 1e-300;	// 1e-324 is smallest, but denormalized. This cuts us some slack.
const float eps_f = 1e-34f;	// 1e-45 is smallest, but denormalized. This cuts us some slack.


template<class _TVarType>
class MathTraits {
};

// Specialization...
template<>
class MathTraits<float>
{
public:
	inline static float Eps() { return 1e-34f; }
	inline static float FullEps() { return 1e-5f; }
	inline static float Pi() { return 3.1415926535897932384626433832795028841971693993751f; }
	inline static bool IsNan(float fl) {
		const uint32 u = *(uint32*)&fl;
		return (u&0x7F800000) == 0x7F800000 && (u&0x7FFFFF);	// Both NaN and qNan.
	}
};

template<>
class MathTraits<double>
{
public:
	inline static double Eps() { return 1e-300; }
	inline static double FullEps() { return 1e-14f; }
	inline static double Pi() { return 3.1415926535897932384626433832795028841971693993751; }
	inline static bool IsNan(double d) {
		const uint64 u = *(uint64*)&d;
		return (u&LEPRA_ULONGLONG(0x7FF0000000000000)) == LEPRA_ULONGLONG(0x7FF0000000000000) && (u&LEPRA_ULONGLONG(0xFFFFFFFFFFFFF));
	}
};



class Math {
public:
	static inline float asinh(float x) {
		return ::log(x + ::sqrt(x*x + 1));
	}
	static inline float acosh(float x) {
		return ::log(x + ::sqrt(x*x - 1));
	}
	static inline float atanh(float x) {
		return (::log(1+x) - ::log(1-x)) / 2;
	}

	static inline float Deg2Rad(float deg) {
		return deg * PIF / 180.0f;
	}
	static inline float Rad2Deg(float rad) {
		return rad * 180.0f / PIF;
	}

	// Places the (radian) angles in the same "range" so that subtraction will return a value
	// between -PI and PI. Solves the problem that arises when one angle is +2*PI and the other
	// is -0.01 - they are very close but a plain subtraction would not return a value close to
	// zero, but a value that is greater than 2*PI...
	template<class _Type> static void RangeAngles(_Type& angle1, _Type& angle2);

	template<class _Type, class _TimeType> static inline _Type Lerp(_Type a, _Type b, _TimeType t) {
		return (a + (_Type)(t*(b-a)));
	}
	template<class _T> static inline _T Lerp(_T a, _T b, _T t) {
		return (a + t*(b-a));
	}

	template<class _Type> static inline _Type GetIterateLerpTime(_Type target, _Type iterate_time) {
		return (std::min(target * (_Type)10 * iterate_time, (_Type)1.0));
	}

	template<class _Type> static _Type Clamp(_Type value, _Type min, _Type max) {
		if (value < min) {
			value = min;
		} else if (value > max) {
			value = max;
		}
		return (value);
	}
	template<class _T> static inline _T SmoothClamp(_T value, _T min, _T max, _T part) {
		deb_assert(part <= (_T)0.5);
		const _T k = max-min;
		_T x = (value-min) / k;	// Transpose to [min,max] -> [0-1].
		const _T x0 = part;
		const _T x1 = 1-part;
		if (x > x1) {
			x = 1 - exp((x1-x)/x0 + log(x0));
			return k*x+min;	// Transpose back [0,1] -> [min,max].
		} else if (x < x0) {
			x = exp(x/x0 + log(x0)-1);
			return k*x+min;	// Transpose back [0,1] -> [min,max].
		}
		return value;
	}

	template<class _Type> static bool IsInRange(_Type value, _Type min, _Type max) {
		return (value >= min && value <= max);
	}
	template<class _Type> static bool IsEpsEqual(_Type value1, _Type value2, _Type eps = MathTraits<_Type>::FullEps()) {
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
	static float Mod(float x, float _min, float _max);

	// Returns the greatest common divider between a and b using the
	// Euclidean extended algorithm.
	// If you are interested in a solution to the Diofantic equation
	// ax + by = gcd(a, b), you should pass pointers to x and y. If not,
	// you can just set them to 0 (NULL).
	static int GCD(int a, int b, int* x = 0, int* y = 0);

	static int Pow(int x, int y);

	// The base 2 logarithm of x, rounded down to nearest lower integer.
	static int Log2(int x);

	template<class _T> static _T Round(_T _value);

	template<class _T> static _T Acos(_T angle);

	template<class _T> static bool CalculateRoot(const _T& a, const _T& b, const _T& c, _T& root1, _T& root2);

	template<class _T, class _Vector> static _T CalculateMedian(const _Vector& array);
	// Not necessarily standard deviation, if not providing arithmetic mean.
	template<class _ReturnType, class _ArrayType, class _Vector> static _ReturnType CalculateDeviation(const _Vector& array, const _ArrayType& mean);
	template<class _ReturnType, class _ArrayType, class _Vector> static _ReturnType CalculateNegativeDeviation(const _Vector& array, const _ArrayType& mean);
};



template<class _Type> void Math::RangeAngles(_Type& angle1, _Type& angle2) {
	angle1 = Mod(angle1, (_Type)-PI, (_Type)PI);
	angle2 = Mod(angle2, (_Type)-PI, (_Type)PI);
	_Type diff = angle1-angle2;
	if (diff > (_Type)PI) {
		angle2 += (_Type)(PI*2);
	} else if (diff < (_Type)-PI) {
		angle1 += (_Type)(PI*2);
	}
}

template<class _T> _T Math::Round(_T _value) {
	return (_value > (_T)0)? ::floor(_value + (_T)0.5) : ::ceil(_value - (_T)0.5);
}

template<class _T> _T Math::Acos(_T angle) {
	if (angle + MathTraits<_T>::FullEps() >= 1) {
		return 0;
	}
	if (angle - MathTraits<_T>::FullEps() <= -1) {
		return MathTraits<_T>::Pi();
	}
	return ::acos(angle);
}

 template<class _T> bool Math::CalculateRoot(const _T& a, const _T& b, const _T& c, _T& root1, _T& root2) {
	// Quaderatic formula:
	// ax^2 + bx + c = 0
	// =>
	//     -b +- sqrt(b^2 - 4ac)
	// x = ---------------------
	//             2a
	const _T b2 = b*b;
	const _T _4ac = 4*a*c;
	if (b2 < _4ac || !a) {	// Does not compute.
		return false;
	}
	const _T root = ::sqrt(b2 - _4ac);
	const _T reciprocal = 1 / (2*a);
	root1 = (-b + root) * reciprocal;
	root2 = (-b - root) * reciprocal;
	return true;
}

template<class _T, class _Vector> _T Math::CalculateMedian(const _Vector& array) {
	// The following code is public domain.
	// Algorithm by Torben Mogensen, implementation by N. Devillard.
	// Modified by Jonas Byström ("C++ templatified" and some code standard).
	// This code in public domain.

	const int array_size = (int)array.size();
	int x, less, greater, equal;
	_T min, max, guess, maxltguess, mingtguess;

	min = max = array[0];
	for (x = 1; x < array_size; ++x) {
		if (array[x] < min) {
			min = array[x];
		}
		if (array[x] > max) {
			max = array[x];
		}
	}

	for (;;) {
		guess = (min+max)/2;
		less = 0; greater = 0; equal = 0;
		maxltguess = min;
		mingtguess = max;
		for (x = 0; x < array_size; ++x) {
			if (array[x] < guess) {
				++less;
				if (array[x] > maxltguess) {
					maxltguess = array[x];
				}
			} else if (array[x] > guess) {
				++greater;
				if (array[x] < mingtguess) {
					mingtguess = array[x];
				}
			} else {
				++equal;
			}
		}
		if (less <= (array_size+1)/2 && greater <= (array_size+1)/2) {
			break;
		} else if (less > greater) {
			max = maxltguess;
		} else {
			min = mingtguess;
		}
	}
	if (less >= (array_size+1)/2) {
		return maxltguess;
	} else if (less+equal >= (array_size+1)/2) {
		return guess;
	} else {
		return mingtguess;
	}
}

template<class _ReturnType, class _ArrayType, class _Vector> _ReturnType Math::CalculateDeviation(const _Vector& array, const _ArrayType& mean) {
	const int array_size = (int)array.size();
	_ArrayType sum = 0;
	for (int x = 0; x < array_size; ++x) {
		const _ArrayType diff = array[x]-mean;
		sum += diff*diff;
	}
	_ReturnType deviation = ::sqrt((_ReturnType)sum/array_size);
	return (deviation);
}

template<class _ReturnType, class _ArrayType, class _Vector> _ReturnType Math::CalculateNegativeDeviation(const _Vector& array, const _ArrayType& mean) {
	const int array_size = (int)array.size();
	_ArrayType sum = 0;
	int negative_count = 0;
	for (int x = 0; x < array_size; ++x) {
		_ArrayType diff = array[x]-mean;
		if (diff < 0) {
			++negative_count;
			sum += diff*diff;
		}
	}
	_ReturnType deviation;
	if (negative_count) {
		deviation = ::pow((_ReturnType)sum/negative_count, (_ReturnType)0.5);
	} else {
		deviation = 0;
	}
	return (deviation);
}



}

