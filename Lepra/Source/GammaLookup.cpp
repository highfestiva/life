/*
	Class:  GammaLookup
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "../Include/GammaLookup.h"
#include <math.h>

namespace Lepra
{

bool GammaLookup::smInitialized = false;
const float32 GammaLookup::smGamma = 2.2f;

uint16 GammaLookup::smGammaToLinearInt[256];
float32 GammaLookup::smGammaToLinearFloat[256];
uint8 GammaLookup::smLinearToGammaInt[65536];

void GammaLookup::Initialize()
{
	if (smInitialized == false)
	{
		int i;

		for (i = 0; i < 256; i++)
		{
			smGammaToLinearFloat[i] = (float32)pow((float32)i / 255.0f, smGamma) * 65535.0f;
			smGammaToLinearInt[i] = (uint16)floor(smGammaToLinearFloat[i] + 0.5f);
		}

		for (i = 0; i < 65536; i++)
		{
			int lResult = (int)floor(pow((float32)i / 65535.0f, 1.0f / smGamma) * 255.0 + 0.5);
			smLinearToGammaInt[i] = (uint8)lResult;
		}

		smInitialized = true;
	}
}

float32 GammaLookup::GammaToLinearF(float32 pGammaValue)
{
	return (float32)pow(pGammaValue, smGamma);
}

float32 GammaLookup::LinearToGammaF(float32 pLinearValue)
{
	return (float32)pow(pLinearValue, 1.0f / smGamma);
}

} // End namespace.
