
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/Random.h"
#include <math.h>



namespace Lepra
{



void Random::SetSeed(uint32 pSeed)
{
	mSeed = pSeed;
}

uint32 Random::GetRandomNumber()
{
	return GetRandomNumber(mSeed);
}

uint32 Random::GetRandomNumber(uint32& pSeed)
{
	pSeed = pSeed * 214013L + 2531011L;
        return (pSeed);
}

uint64 Random::GetRandomNumber64()
{
	uint64 lRand = ((uint64)GetRandomNumber()) << 32;
	lRand += GetRandomNumber();
	return (lRand);
}

double Random::Uniform(double pLower, double pUpper)
{
	return Uniform(mSeed, pLower, pUpper);
}

double Random::Uniform(uint32& pSeed, double pLower, double pUpper)
{
	return (GetRandomNumber(pSeed)/(double)0xFFFFFFFF * (pUpper-pLower) + pLower);
}

float64 Random::Normal(float64 pMean, float64 pStdDev)
{
	// Box-Muller.
	float64 v;
	float64 s;
	do
	{
		v = Uniform(-1, 1);
		float64 u = Uniform(-1, 1);
		s = v*v + u*u;
	}
	while (s >= 1.0);
	const float64 f = ::sqrt(-2.0 * ::log(s) / s);
	v = f * v;
	return v * pStdDev + pMean;
}

float64 Random::Normal(float64 pMean, float64 pStdDev, float64 pLowCutoff, float64 pHighCutoff)
{
	float64 lValue;
	do
	{
		lValue = Normal(pMean, pStdDev);
	}
	while (lValue < pLowCutoff || lValue > pHighCutoff);
	return lValue;
}



uint32 Random::mSeed = 0;



}
