
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/gammalookup.h"
#include <math.h>



namespace lepra {



#define GAMMA_FACTOR	2.2f



void GammaLookup::Initialize() {
	for (int i = 0; i < 256; i++) {
		gamma_to_linear_float_[i] = ::powf(i/255.0f, GAMMA_FACTOR) * 65535.0f;
		gamma_to_linear_int_[i] = (int)::floorf(gamma_to_linear_float_[i] + 0.5f);
	}
	for (int i = 0; i < 65536; i++) {
		int result = (int)::floorf(powf(i/65535.0f, 1.0f/GAMMA_FACTOR) * 255.0f + 0.5f);
		linear_to_gamma_int_[i] = (uint8)result;
	}
}

float GammaLookup::GammaToLinearF(float gamma_value) {
	return (powf(gamma_value, GAMMA_FACTOR));
}

float GammaLookup::LinearToGammaF(float linear_value) {
	return (powf(linear_value, 1.0f / GAMMA_FACTOR));
}



int GammaLookup::gamma_to_linear_int_[256];
float GammaLookup::gamma_to_linear_float_[256];
uint8 GammaLookup::linear_to_gamma_int_[65536];



}
