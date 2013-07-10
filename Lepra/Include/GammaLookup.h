
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine
//
// Must be quick for image resizes.



#pragma once

#include "LepraTypes.h"



namespace Lepra
{



class GammaLookup
{
public:
	static void Initialize();

	static inline int GammaToLinear(uint8 pGammaValue);
	static inline float GammaToLinearFloat(uint8 pGammaValue);
	static inline uint8 LinearToGamma(int pLinearValue);

	// These are slow... Not using lookup tables.
	static float GammaToLinearF(float pGammaValue);
	static float LinearToGammaF(float pLinearValue);

private:
	static int smGammaToLinearInt[256];
	static float smGammaToLinearFloat[256];
	static uint8 smLinearToGammaInt[65536];
};



int GammaLookup::GammaToLinear(uint8 pGammaValue)
{
	return smGammaToLinearInt[pGammaValue];
}

float GammaLookup::GammaToLinearFloat(uint8 pGammaValue)
{
	return smGammaToLinearFloat[pGammaValue];
}

uint8 GammaLookup::LinearToGamma(int pLinearValue)
{
	return smLinearToGammaInt[pLinearValue];
}



}
