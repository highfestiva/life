/*
	Class:  GammaLookup
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	A static class used for convertion of RGB color values between
	gamma space (standard RGB) and linear space.

	To interpolate RGB-values correctly they need to be in linear
	space. But to display them correctly on a monitor, they need
	to be in gamma space.
*/

#ifndef GAMMALOOKUP_H
#define GAMMALOOKUP_H

#include "LepraTypes.h"

namespace Lepra
{

class GammaLookup
{
public:
	friend void Init();
	friend void Shutdown();

	static inline uint16 GammaToLinear(uint8 pGammaValue);
	static inline float32 GammaToLinearFloat(uint8 pGammaValue);
	static inline uint8 LinearToGamma(uint16 pLinearValue);

	// These are slow... Not using lookup tables.
	static float32 GammaToLinearF(float32 pGammaValue);
	static float32 LinearToGammaF(float32 pLinearValue);

private:

	static void Initialize();

	static bool smInitialized;

	// The gamma constant. Usually set to 2.2.
	static const float32 smGamma;

	static uint16 smGammaToLinearInt[256];
	static float32 smGammaToLinearFloat[256];
    static uint8 smLinearToGammaInt[65536];
};

uint16 GammaLookup::GammaToLinear(uint8 pGammaValue)
{
	return smGammaToLinearInt[pGammaValue];
}

float32 GammaLookup::GammaToLinearFloat(uint8 pGammaValue)
{
	return smGammaToLinearFloat[pGammaValue];
}

uint8 GammaLookup::LinearToGamma(uint16 pLinearValue)
{
	return smLinearToGammaInt[pLinearValue];
}

} // End namespace.

#endif
