// Author: Jonas Byström and Jonas Byström
// Copyright (c) Pixel Doctrine

#include "pch.h"
#include "../include/lepratarget.h"
#include "../include/math.h"

#ifdef LEPRA_POSIX
#include <cstdlib>
#endif

namespace lepra {

int Math::Mod(int x, int y) {
	if (y < 0) {
		y = -y;
	}

	if (x < 0) {
		return y - (-x) % y;
	} else {
		return x % y;
	}
}

float Math::Mod(float x, float y) {
	if (y < 0) {
		y = -y;
	}

	if (x < 0) {
		x = y - fmod(-x, y);
	} else {
		x = fmod(x, y);
	}

	return x;
}

double Math::Mod(double x, double y) {
	if (y < 0) {
		y = -y;
	}

	if (x < 0) {
		x = y - fmod(-x, y);
	} else {
		x = fmod(x, y);
	}

	return x;
}

float Math::Mod(float x, float min, float max) {
	float __x = x - min;
	float __y = max - min;

	if (x >= 0.0f) {
		return (__x - __y * floor(__x / __y)) + min;
	} else {
		return (__y * ceil(-__x / __y) - (-__x)) + min;
	}
}

int InternalGCD(int prev_divider,
		int prev_rest,
		int* x,
		int* y) {
	// The Euclidean algorithm (from 300 B.C.).

	int q = prev_divider / prev_rest;
	int r = prev_divider % prev_rest;

	int gcd = 0;

	if (r == 0) {
		gcd = prev_rest;

		if (x != 0 && y != 0) {
			*x = 1;
			*y = 0;
		}
	} else {
		// Notice the swap of x & y order.
		gcd = InternalGCD(prev_rest, r, y, x);

		if (x != 0 && y != 0) {
			*x -= q * *y;
		}
	}
	return gcd;
}

int Math::GCD(int a, int b, int* x, int* y) {
	a = abs(a);
	b = abs(b);

	if (a > b) {
		// Swap values.
		int t = a;
		a = b;
		b = t;

		int* p = x;
		x = y;
		y = p;
	}

	int r = b % a;
	b /= a;

	if (r == 0) {
		if (x != 0 && y != 0) {
			*x = 1;
			*y = 0;
		}
		return a;
	}


	// Notice the swap of x & y order.
	int gcd = InternalGCD(a, r, y, x);

	if (x != 0 && y != 0) {
		b *= *y;
		*x -= b;
	}

	return gcd;
}

int Math::Pow(int x, int y) {
	int power = 1;
	while (y != 0) {
		if ((y & 1) != 0) {
			power *= x;
		}

		x *= x;
		y >>= 1;
	}

	return power;
}

int Math::Log2(int x) {
	int result = 0;

	do {
		++result;
		x >>= 1;
	}while (x > 0);

	return result;
}

}
