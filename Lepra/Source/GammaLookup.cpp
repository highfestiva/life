
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/GammaLookup.h"
#include <math.h>



namespace Lepra
{



#define GAMMA_FACTOR	2.2f



void GammaLookup::Initialize()
{
	for (int i = 0; i < 256; i++)
	{
		smGammaToLinearFloat[i] = ::powf(i/255.0f, GAMMA_FACTOR) * 65535.0f;
		smGammaToLinearInt[i] = (int)::floorf(smGammaToLinearFloat[i] + 0.5f);
	}
	for (int i = 0; i < 65536; i++)
	{
		int lResult = (int)::floorf(powf(i/65535.0f, 1.0f/GAMMA_FACTOR) * 255.0f + 0.5f);
		smLinearToGammaInt[i] = (uint8)lResult;
	}
}

float GammaLookup::GammaToLinearF(float pGammaValue)
{
	return (powf(pGammaValue, GAMMA_FACTOR));
}

float GammaLookup::LinearToGammaF(float pLinearValue)
{
	return (powf(pLinearValue, 1.0f / GAMMA_FACTOR));
}



int GammaLookup::smGammaToLinearInt[256];
float GammaLookup::smGammaToLinearFloat[256];
uint8 GammaLookup::smLinearToGammaInt[65536];



}
