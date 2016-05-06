
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/random.h"
#include <math.h>



namespace lepra {



void Random::SetSeed(uint32 seed) {
	seed_ = seed;
}

uint32 Random::GetRandomNumber() {
	return GetRandomNumber(seed_);
}

uint32 Random::GetRandomNumber(uint32& seed) {
	seed = seed * 214013L + 2531011L;
        return (seed);
}

uint64 Random::GetRandomNumber64() {
	uint64 rand = ((uint64)GetRandomNumber()) << 32;
	rand += GetRandomNumber();
	return (rand);
}



uint32 Random::seed_ = 0;



}
