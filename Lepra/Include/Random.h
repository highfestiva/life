/*
	Class:  Random
	Author: Jonas Byström
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	This file contains a deterministic random number generator.
	Future development may include fractal iteration helpers.
*/



#pragma once

#include "LepraTypes.h"



namespace Lepra
{



class Random
{
public:
	static void SetSeed(uint32 pSeed);
	static uint32 GetRandomNumber();
	static uint64 GetRandomNumber64();
	static float64 Uniform(float64 pLower = 0.0, float64 pUpper = 1.0);

private:
	static uint32 mSeed;
};



}
