
// Author: Jonas Byström
// Copyright (c) 2002-2013, Pixel Doctrine



#pragma once

#include "LepraTypes.h"



namespace Lepra
{



#define RNDVEC(q)	Vector3DF(Random::Uniform(-q, +q), Random::Uniform(-q, +q), Random::Uniform(-q, +q))
#define RNDPOSVEC()	Vector3DF(Random::Uniform(0.0f, 1.0f), Random::Uniform(0.0f, 1.0f), Random::Uniform(0.0f, 1.0f))



class Random
{
public:
	static void SetSeed(uint32 pSeed);
	static uint32 GetRandomNumber();
	static uint32 GetRandomNumber(uint32& pSeed);
	static uint64 GetRandomNumber64();

	template<class _T> static inline _T Uniform(_T pLower, _T pUpper);
	template<class _T> static inline _T Uniform(uint32& pSeed, _T pLower, _T pUpper);
	template<class _T> static inline _T Normal(_T pMean, _T pStdDev);
	template<class _T> static inline _T Normal(_T pMean, _T pStdDev, _T pLowCutoff, _T pHighCutoff);

private:
	static uint32 mSeed;
};



template<class _T> _T Random::Uniform(_T pLower, _T pUpper)
{
	return Uniform(mSeed, pLower, pUpper);
}

template<class _T> _T Random::Uniform(uint32& pSeed, _T pLower, _T pUpper)
{
	return GetRandomNumber(pSeed)/(_T)0xFFFFFFFF * (pUpper-pLower) + pLower;
}

template<class _T> _T Random::Normal(_T pMean, _T pStdDev)
{
	// Box-Muller.
	_T v;
	_T s;
	do
	{
		v = Uniform((_T)-1, (_T)1);
		_T u = Uniform((_T)-1, (_T)1);
		s = v*v + u*u;
	}
	while (s >= 1.0);
	const _T f = sqrt((_T)-2.0 * (_T)log(s) / s);
	v = f * v;
	return v * pStdDev + pMean;
}

template<class _T> _T Random::Normal(_T pMean, _T pStdDev, _T pLowCutoff, _T pHighCutoff)
{
	_T lValue;
	do
	{
		lValue = Normal(pMean, pStdDev);
	}
	while (lValue < pLowCutoff || lValue > pHighCutoff);
	return lValue;
}



}
