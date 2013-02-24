
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



uint32 Random::mSeed = 0;



}
