
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/Random.h"



namespace Lepra
{



void Random::SetSeed(uint32 pSeed)
{
	mSeed = pSeed;
}

uint32 Random::GetRandomNumber()
{
	mSeed = mSeed * 214013L + 2531011L;
        return (mSeed);
}

uint64 Random::GetRandomNumber64()
{
	uint64 lRand = ((uint64)GetRandomNumber()) << 32;
	lRand += GetRandomNumber();
	return (lRand);
}

double Random::Uniform(double pLower, double pUpper)
{
	return (GetRandomNumber()/(double)0xFFFFFFFF * (pUpper-pLower) + pLower);
}



uint32 Random::mSeed = 0;



}
