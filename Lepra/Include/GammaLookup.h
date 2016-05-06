
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine
//
// Must be quick for image resizes.



#pragma once

#include "lepratypes.h"



namespace lepra {



class GammaLookup {
public:
	static void Initialize();

	static inline int GammaToLinear(uint8 gamma_value);
	static inline float GammaToLinearFloat(uint8 gamma_value);
	static inline uint8 LinearToGamma(int linear_value);

	// These are slow... Not using lookup tables.
	static float GammaToLinearF(float gamma_value);
	static float LinearToGammaF(float linear_value);

private:
	static int gamma_to_linear_int_[256];
	static float gamma_to_linear_float_[256];
	static uint8 linear_to_gamma_int_[65536];
};



int GammaLookup::GammaToLinear(uint8 gamma_value) {
	return gamma_to_linear_int_[gamma_value];
}

float GammaLookup::GammaToLinearFloat(uint8 gamma_value) {
	return gamma_to_linear_float_[gamma_value];
}

uint8 GammaLookup::LinearToGamma(int linear_value) {
	return linear_to_gamma_int_[linear_value];
}



}
