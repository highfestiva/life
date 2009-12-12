// Author: Alexander Hugestrand and Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

#include "../Include/LepraTarget.h"
#include "../Include/Math.h"

#ifdef LEPRA_POSIX
#include <cstdlib>
#endif

namespace Lepra
{

int Math::Mod(int x, int y)
{
	if (y < 0)
	{
		y = -y;
	}

	if (x < 0)
	{
		return y - (-x) % y;
	}
	else
	{
		return x % y;
	}
}

float Math::Mod(float x, float y)
{
	if (y < 0)
	{
		y = -y;
	}

	if (x < 0)
	{
		x = y - fmod(-x, y);
	}
	else
	{
		x = fmod(x, y);
	}

	return x;
}

double Math::Mod(double x, double y)
{
	if (y < 0)
	{
		y = -y;
	}

	if (x < 0)
	{
		x = y - fmod(-x, y);
	}
	else
	{
		x = fmod(x, y);
	}

	return x;
}

float Math::Mod(float x, float pMin, float pMax)
{
	float lX = x - pMin;
	float lY = pMax - pMin;

	if (x >= 0.0f)
	{
		return (lX - lY * floor(lX / lY)) + pMin;
	}
	else
	{
		return (lY * ceil(-lX / lY) - (-lX)) + pMin;
	}
}

int InternalGCD(int pPrevDivider,
		int pPrevRest,
		int* x,
		int* y)
{
	// The Euclidean algorithm (from 300 B.C.).

	int q = pPrevDivider / pPrevRest;
	int r = pPrevDivider % pPrevRest;
	
	int lGCD = 0;

	if (r == 0)
	{
		lGCD = pPrevRest;

		if (x != 0 && y != 0)
		{
			*x = 1;
			*y = 0;
		}
	}
	else
	{
		// Notice the swap of x & y order.
		lGCD = InternalGCD(pPrevRest, r, y, x);

		if (x != 0 && y != 0)
		{
			*x -= q * *y;
		}
	}	
	return lGCD;
}

int Math::GCD(int a, int b, int* x, int* y)
{
	a = abs(a);
	b = abs(b);

	if (a > b)
	{
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

	if (r == 0)
	{
		if (x != 0 && y != 0)
		{
			*x = 1;
			*y = 0;
		}
		return a;
	}


	// Notice the swap of x & y order.
	int lGCD = InternalGCD(a, r, y, x);

	if (x != 0 && y != 0)
	{
		b *= *y;
		*x -= b;
	}

	return lGCD;
}

int Math::Pow(int x, int y)
{
	int lPower = 1;
	while (y != 0)
	{
		if ((y & 1) != 0)
		{
			lPower *= x;
		}

		x *= x;
		y >>= 1;
	}

	return lPower;
}

int Math::Log2(int x)
{
	int lResult = 0;

	do
	{
		++lResult;
		x >>= 1;
	}while (x > 0);

	return lResult;
}

}
