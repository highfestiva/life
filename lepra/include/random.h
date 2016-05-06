
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepratypes.h"



namespace lepra {



#define RNDVEC3(q0,q1,q2)	vec3(Random::Uniform(-q0, +q0), Random::Uniform(-q1, +q1), Random::Uniform(-q2, +q2))
#define RNDVEC(q)		RNDVEC3(q, q, q)
#define RNDPOSVEC()		vec3(Random::Uniform(0.0f, 1.0f), Random::Uniform(0.0f, 1.0f), Random::Uniform(0.0f, 1.0f))



class Random {
public:
	static void SetSeed(uint32 seed);
	static uint32 GetRandomNumber();
	static uint32 GetRandomNumber(uint32& seed);
	static uint64 GetRandomNumber64();

	template<class _T> static inline _T Uniform(_T lower, _T upper);
	template<class _T> static inline _T Uniform(uint32& seed, _T lower, _T upper);
	template<class _T> static inline _T Normal(_T mean, _T std_dev);
	template<class _T> static inline _T Normal(_T mean, _T std_dev, _T low_cutoff, _T high_cutoff);

private:
	static uint32 seed_;
};



template<class _T> _T Random::Uniform(_T lower, _T upper) {
	return Uniform(seed_, lower, upper);
}

template<class _T> _T Random::Uniform(uint32& seed, _T lower, _T upper) {
	return GetRandomNumber(seed)/(_T)0xFFFFFFFF * (upper-lower) + lower;
}

template<class _T> _T Random::Normal(_T mean, _T std_dev) {
	// Box-Muller.
	_T v;
	_T s;
	do {
		v = Uniform((_T)-1, (_T)1);
		_T u = Uniform((_T)-1, (_T)1);
		s = v*v + u*u;
	} while (s >= 1.0);
	const _T f = sqrt((_T)-2.0 * (_T)log(s) / s);
	v = f * v;
	return v * std_dev + mean;
}

template<class _T> _T Random::Normal(_T mean, _T std_dev, _T low_cutoff, _T high_cutoff) {
	_T value;
	do {
		value = Normal(mean, std_dev);
	} while (value < low_cutoff || value > high_cutoff);
	return value;
}



}
