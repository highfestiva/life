
// Author: Jonas Byström & Alexander Hugestrand
// Copyright (c) 2002-2008, Righteous Games



#include "../Include/Random.h"
#pragma warning(push)
#pragma warning(disable: 4127)	// Conditional expression is constant.
#include "../../ThirdParty/mtwist-0.8/mtwist.h"
#include "../../ThirdParty/mtwist-0.8/randistrs.h"
#pragma warning(pop)
#include <math.h>


namespace Lepra
{

void Random::SetSeed(uint32 pSeed)
{
	::mt_seed32new(pSeed);
}

uint32 Random::GetRandomNumber()
{
	return ::mt_lrand();
}

uint64 Random::GetRandomNumber64()
{
	return ::mt_llrand();
}

float64 Random::Uniform(float64 pLower, float64 pUpper)
{
	return ::rd_luniform(pLower, pUpper);
}

float64 Random::Normal(float64 pMean, float64 pSigma)
{
	return ::rd_lnormal(pMean, pSigma);
}

float64 Random::LogNormal(float64 pShape, float64 pScale)
{
	return ::rd_llognormal(pShape, pScale);
}

float64 Random::Exponential(float64 pMean)
{
	return ::rd_lexponential(pMean);
}

float64 Random::Erlang(int pP, float64 pMean)
{
	return ::rd_lerlang(pP, pMean);
}

float64 Random::Weibull(float64 pShape, float64 pScale)
{
	return ::rd_lweibull(pShape, pScale);
}

float64 Random::Triangular(float64 pLower, float64 pUpper, float64 pMode)
{
	return ::rd_ltriangular(pLower, pUpper, pMode);
}

} // End namespace.
