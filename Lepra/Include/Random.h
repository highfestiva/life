/*
	Class:  Random
	Author: Jonas Byström
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	This file contains a deterministic random number generator.
	Future development may include fractal iteration helpers.
*/



#ifndef LEPRA_RANDOM_H
#define LEPRA_RANDOM_H



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
	static float64 Normal(float64 pMean, float64 pSigma);
	static float64 LogNormal(float64 pShape, float64 pScale);
	static float64 Exponential(float64 pMean);
	static float64 Erlang(int pP, float64 pMean);
	static float64 Weibull(float64 pShape, float64 pScale);
	static float64 Triangular(float64 pLower, float64 pUpper, float64 pMode);

private:
};



}



#endif // !LEPRA_RANDOM_H
