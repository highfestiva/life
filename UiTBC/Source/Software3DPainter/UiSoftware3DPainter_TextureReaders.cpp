/*
	Lepra::File:   Software3DPainter_TextureReaders.cpp
	Class:  Software3DPainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

///////////////////////////////////////////////////////////////////////////////////
//	
//	The texture readers. (Only used with Software3DPainter::RENDER_NICEST)
//	
///////////////////////////////////////////////////////////////////////////////////

#include "../../Include/UiTBC.h"
#include "../../Include/UiSoftware3DPainter.h"
#include "../../Include/UiTexture.h"
#include "../../../Lepra/Include/FixedPointMath.h"

#include <math.h>

namespace UiTbc
{

void Software3DPainter::GetTextureColor8BitStandard(float pU, float pV, float pM, Software3DPainter* pThis, 
													Lepra::Color& pColor)
{
	int lM = (int)floor(pM);
	unsigned char* lTextureBits = (unsigned char*)pThis->mTexture->GetColorMap(lM)->GetBuffer();
	int lTexturePitch = pThis->mTexture->GetColorMap(lM)->GetPitch();
	int lTWMask = pThis->mTexture->GetColorMap(lM)->GetWidth() - 1;
	int lTHMask = pThis->mTexture->GetColorMap(lM)->GetHeight() - 1;

	float lOneOverM = 1.0f / (float)(1 << lM);
	int lU = (int)(pU * lOneOverM + 0.5f) & lTWMask;
	int lV = (int)(pV * lOneOverM + 0.5f) & lTHMask;
	int lIndex = lV * lTexturePitch + lU;

	pColor.mRed   = lTextureBits[lIndex];
	pColor.mAlpha = pThis->GetAlpha(lM, lIndex);
}

void Software3DPainter::GetTextureColor15BitStandard(float pU, float pV, float pM, Software3DPainter* pThis, 
														Lepra::Color& pColor)
{
	int lM = (int)floor(pM);
	unsigned short* lTextureBits = (unsigned short*)pThis->mTexture->GetColorMap(lM)->GetBuffer();
	int lTexturePitch = pThis->mTexture->GetColorMap(lM)->GetPitch();
	int lTWMask = pThis->mTexture->GetColorMap(lM)->GetWidth() - 1;
	int lTHMask = pThis->mTexture->GetColorMap(lM)->GetHeight() - 1;

	float lOneOverM = 1.0f / (float)(1 << lM);
	int lU = (int)(pU * lOneOverM + 0.5f) & lTWMask;
	int lV = (int)(pV * lOneOverM + 0.5f) & lTHMask;
	int lIndex = lV * lTexturePitch + lU;

	pColor.mRed   = (Lepra::uint8)((lTextureBits[lIndex] >> 10) & 31);
	pColor.mGreen = (Lepra::uint8)((lTextureBits[lIndex] >>  5) & 31);
	pColor.mBlue  = (Lepra::uint8)((lTextureBits[lIndex] >>  0) & 31);
	pColor.mAlpha = pThis->GetAlpha(lM, lIndex);
}

void Software3DPainter::GetTextureColor16BitStandard(float pU, float pV, float pM, Software3DPainter* pThis, 
														Lepra::Color& pColor)
{
	int lM = (int)floor(pM);
	unsigned short* lTextureBits = (unsigned short*)pThis->mTexture->GetColorMap(lM)->GetBuffer();
	int lTexturePitch = pThis->mTexture->GetColorMap(lM)->GetPitch();
	int lTWMask = pThis->mTexture->GetColorMap(lM)->GetWidth() - 1;
	int lTHMask = pThis->mTexture->GetColorMap(lM)->GetHeight() - 1;

	float lOneOverM = 1.0f / (float)(1 << lM);
	int lU = (int)(pU * lOneOverM + 0.5f) & lTWMask;
	int lV = (int)(pV * lOneOverM + 0.5f) & lTHMask;
	int lIndex = lV * lTexturePitch + lU;

	pColor.mRed   = (Lepra::uint8)((lTextureBits[lIndex] >> 11) & 31);
	pColor.mGreen = (Lepra::uint8)((lTextureBits[lIndex] >>  5) & 63);
	pColor.mBlue  = (Lepra::uint8)((lTextureBits[lIndex] >>  0) & 31);
	pColor.mAlpha = pThis->GetAlpha(lM, lIndex);
}

void Software3DPainter::GetTextureColor24BitStandard(float pU, float pV, float pM, Software3DPainter* pThis, 
														Lepra::Color& pColor)
{
	int lM = (int)floor(pM);
	unsigned char* lTextureBits = (unsigned char*)pThis->mTexture->GetColorMap(lM)->GetBuffer();
	int lTexturePitch = pThis->mTexture->GetColorMap(lM)->GetPitch();
	int lTWMask = pThis->mTexture->GetColorMap(lM)->GetWidth() - 1;
	int lTHMask = pThis->mTexture->GetColorMap(lM)->GetHeight() - 1;

	float lOneOverM = 1.0f / (float)(1 << lM);
	int lU = (int)(pU * lOneOverM + 0.5f) & lTWMask;
	int lV = (int)(pV * lOneOverM + 0.5f) & lTHMask;
	int lIndex = lV * lTexturePitch + lU;

	lTextureBits += (lIndex) * 3;

	pColor.mBlue  = lTextureBits[0];
	pColor.mGreen = lTextureBits[1];
	pColor.mRed   = lTextureBits[2];
	pColor.mAlpha = pThis->GetAlpha(lM, lIndex);
}

void Software3DPainter::GetTextureColor32BitStandard(float pU, float pV, float pM, Software3DPainter* pThis, 
														Lepra::Color& pColor)
{
	int lM = (int)floor(pM);
	unsigned int* lTextureBits = (unsigned int*)pThis->mTexture->GetColorMap(lM)->GetBuffer();
	int lTexturePitch = pThis->mTexture->GetColorMap(lM)->GetPitch();
	int lTWMask = pThis->mTexture->GetColorMap(lM)->GetWidth() - 1;
	int lTHMask = pThis->mTexture->GetColorMap(lM)->GetHeight() - 1;

	float lOneOverM = 1.0f / (float)(1 << lM);
	int lU = (int)(pU * lOneOverM + 0.5f) & lTWMask;
	int lV = (int)(pV * lOneOverM + 0.5f) & lTHMask;
	int lIndex = lV * lTexturePitch + lU;

	pColor.mRed   = (Lepra::uint8)((lTextureBits[lIndex] >> 16) & 0xFF);
	pColor.mGreen = (Lepra::uint8)((lTextureBits[lIndex] >>  8) & 0xFF);
	pColor.mBlue  = (Lepra::uint8)((lTextureBits[lIndex] >>  0) & 0xFF);
	pColor.mAlpha = pThis->GetAlpha(lM, lIndex);
}

void Software3DPainter::GetTextureColor15BitBilinear(float pU, float pV, float pM, Software3DPainter* pThis, 
														Lepra::Color& pColor)
{
	int lM = (int)floor(pM);
	unsigned short* lTextureBits = (unsigned short*)pThis->mTexture->GetColorMap(lM)->GetBuffer();
	int lTexturePitch = pThis->mTexture->GetColorMap(lM)->GetPitch();
	int lTWMask = pThis->mTexture->GetColorMap(lM)->GetWidth() - 1;
	int lTHMask = pThis->mTexture->GetColorMap(lM)->GetHeight() - 1;

	float lOneOverM = 1.0f / (float)(1 << lM);
	pU *= lOneOverM;
	pV *= lOneOverM;

	Lepra::FixedPointMath::Fixed16_16 lUFrac = Lepra::FixedPointMath::FloatToFixed16_16(pU) & 0xFFFF;
	Lepra::FixedPointMath::Fixed16_16 lVFrac = Lepra::FixedPointMath::FloatToFixed16_16(pV) & 0xFFFF;

	int lU0 = (int)floor(pU) & lTWMask;
	int lV0 = (int)floor(pV) & lTHMask;

	int lU1 = (lU0 + 1) & lTWMask;
	int lV1 = (lV0 + 1) & lTHMask;

	if (lU0 < 0)
	{
		lU0 = 0;
		lUFrac = 0;
	}

	if (lV0 < 0)
	{
		lV0 = 0;
		lVFrac = 0;
	}

	if (lU1 >= (int)pThis->mTexture->GetColorMap(lM)->GetWidth())
	{
		lU1 = (int)pThis->mTexture->GetColorMap(lM)->GetWidth() - 1;
	}

	if (lV1 >= (int)pThis->mTexture->GetColorMap(lM)->GetHeight())
	{
		lV1 = (int)pThis->mTexture->GetColorMap(lM)->GetHeight() - 1;
	}

	int lV0Offset = lV0 * lTexturePitch;
	int lV1Offset = lV1 * lTexturePitch;

	short lColorUL = lTextureBits[lV0Offset + lU0];
	short lColorUR = lTextureBits[lV0Offset + lU1];
	short lColorLL = lTextureBits[lV1Offset + lU0];
	short lColorLR = lTextureBits[lV1Offset + lU1];

	short lAlphaUL = pThis->GetAlpha(lM, lV0Offset + lU0);
	short lAlphaUR = pThis->GetAlpha(lM, lV0Offset + lU1);
	short lAlphaLL = pThis->GetAlpha(lM, lV1Offset + lU0);
	short lAlphaLR = pThis->GetAlpha(lM, lV1Offset + lU1);

	short lRedUL   = ((lColorUL >> 10) & 31) << 3;
	short lGreenUL = ((lColorUL >> 5 ) & 31) << 3;
	short lBlueUL  = (lColorUL & 31) << 3;

	short lRedUR   = ((lColorUR >> 10) & 31) << 3;
	short lGreenUR = ((lColorUR >> 5 ) & 31) << 3;
	short lBlueUR  = (lColorUR & 31) << 3;

	short lRedLL   = ((lColorLL >> 10) & 31) << 3;
	short lGreenLL = ((lColorLL >> 5 ) & 31) << 3;
	short lBlueLL  = (lColorLL & 31) << 3;

	short lRedLR   = ((lColorLR >> 10) & 31) << 3;
	short lGreenLR = ((lColorLR >> 5 ) & 31) << 3;
	short lBlueLR  = (lColorLR & 31) << 3;

	unsigned char lBlueU  = (unsigned char)((( (lBlueUR  - lBlueUL)  * lUFrac ) >> 16) + lBlueUL);
	unsigned char lGreenU = (unsigned char)((( (lGreenUR - lGreenUL) * lUFrac ) >> 16) + lGreenUL);
	unsigned char lRedU   = (unsigned char)((( (lRedUR   - lRedUL)   * lUFrac ) >> 16) + lRedUL);
	unsigned char lAlphaU = (unsigned char)((( (lAlphaUR - lAlphaUL) * lUFrac ) >> 16) + lAlphaUL);

	unsigned char lBlueL  = (unsigned char)((( (lBlueLR  - lBlueLL)  * lUFrac ) >> 16) + lBlueLL);
	unsigned char lGreenL = (unsigned char)((( (lGreenLR - lGreenLL) * lUFrac ) >> 16) + lGreenLL);
	unsigned char lRedL   = (unsigned char)((( (lRedLR   - lRedLL)   * lUFrac ) >> 16) + lRedLL);
	unsigned char lAlphaL = (unsigned char)((( (lAlphaLR - lAlphaLL) * lUFrac ) >> 16) + lAlphaLL);

	pColor.mBlue  = (unsigned char)((( ((short)lBlueL  - (short)lBlueU)  * lVFrac) >> 16) + lBlueU);
	pColor.mGreen = (unsigned char)((( ((short)lGreenL - (short)lGreenU) * lVFrac) >> 16) + lGreenU);
	pColor.mRed   = (unsigned char)((( ((short)lRedL   - (short)lRedU)   * lVFrac) >> 16) + lRedU);
	pColor.mAlpha = (unsigned char)((( ((short)lAlphaL - (short)lAlphaU) * lVFrac) >> 16) + lAlphaU);
}

void Software3DPainter::GetTextureColor16BitBilinear(float pU, float pV, float pM, Software3DPainter* pThis, 
														Lepra::Color& pColor)
{
	int lM = (int)floor(pM);
	unsigned short* lTextureBits = (unsigned short*)pThis->mTexture->GetColorMap(lM)->GetBuffer();
	int lTexturePitch = pThis->mTexture->GetColorMap(lM)->GetPitch();
	int lTWMask = pThis->mTexture->GetColorMap(lM)->GetWidth() - 1;
	int lTHMask = pThis->mTexture->GetColorMap(lM)->GetHeight() - 1;

	float lOneOverM = 1.0f / (float)(1 << lM);
	pU *= lOneOverM;
	pV *= lOneOverM;

	Lepra::FixedPointMath::Fixed16_16 lUFrac = Lepra::FixedPointMath::FloatToFixed16_16(pU) & 0xFFFF;
	Lepra::FixedPointMath::Fixed16_16 lVFrac = Lepra::FixedPointMath::FloatToFixed16_16(pV) & 0xFFFF;

	int lU0 = (int)floor(pU) & lTWMask;
	int lV0 = (int)floor(pV) & lTHMask;

	int lU1 = (lU0 + 1) & lTWMask;
	int lV1 = (lV0 + 1) & lTHMask;

	if (lU0 < 0)
	{
		lU0 = 0;
		lUFrac = 0;
	}

	if (lV0 < 0)
	{
		lV0 = 0;
		lVFrac = 0;
	}

	if (lU1 >= (int)pThis->mTexture->GetColorMap(lM)->GetWidth())
	{
		lU1 = (int)pThis->mTexture->GetColorMap(lM)->GetWidth() - 1;
	}

	if (lV1 >= (int)pThis->mTexture->GetColorMap(lM)->GetHeight())
	{
		lV1 = (int)pThis->mTexture->GetColorMap(lM)->GetHeight() - 1;
	}

	int lV0Offset = lV0 * lTexturePitch;
	int lV1Offset = lV1 * lTexturePitch;

	short lColorUL = lTextureBits[lV0Offset + lU0];
	short lColorUR = lTextureBits[lV0Offset + lU1];
	short lColorLL = lTextureBits[lV1Offset + lU0];
	short lColorLR = lTextureBits[lV1Offset + lU1];

	short lAlphaUL = pThis->GetAlpha(lM, lV0Offset + lU0);
	short lAlphaUR = pThis->GetAlpha(lM, lV0Offset + lU1);
	short lAlphaLL = pThis->GetAlpha(lM, lV1Offset + lU0);
	short lAlphaLR = pThis->GetAlpha(lM, lV1Offset + lU1);

	short lRedUL   = ((lColorUL >> 11) & 31) << 3;
	short lGreenUL = ((lColorUL >> 5 ) & 63) << 2;
	short lBlueUL  = (lColorUL & 31) << 3;

	short lRedUR   = ((lColorUR >> 11) & 31) << 3;
	short lGreenUR = ((lColorUR >> 5 ) & 63) << 2;
	short lBlueUR  = (lColorUR & 31) << 3;

	short lRedLL   = ((lColorLL >> 11) & 31) << 3;
	short lGreenLL = ((lColorLL >> 5 ) & 63) << 2;
	short lBlueLL  = (lColorLL & 31) << 3;

	short lRedLR   = ((lColorLR >> 11) & 31) << 3;
	short lGreenLR = ((lColorLR >> 5 ) & 63) << 2;
	short lBlueLR  = (lColorLR & 31) << 3;

	unsigned char lBlueU  = (unsigned char)((( (lBlueUR  - lBlueUL)  * lUFrac ) >> 16) + lBlueUL);
	unsigned char lGreenU = (unsigned char)((( (lGreenUR - lGreenUL) * lUFrac ) >> 16) + lGreenUL);
	unsigned char lRedU   = (unsigned char)((( (lRedUR   - lRedUL)   * lUFrac ) >> 16) + lRedUL);
	unsigned char lAlphaU = (unsigned char)((( (lAlphaUR - lAlphaUL) * lUFrac ) >> 16) + lAlphaUL);

	unsigned char lBlueL  = (unsigned char)((( (lBlueLR  - lBlueLL)  * lUFrac ) >> 16) + lBlueLL);
	unsigned char lGreenL = (unsigned char)((( (lGreenLR - lGreenLL) * lUFrac ) >> 16) + lGreenLL);
	unsigned char lRedL   = (unsigned char)((( (lRedLR   - lRedLL)   * lUFrac ) >> 16) + lRedLL);
	unsigned char lAlphaL = (unsigned char)((( (lAlphaLR - lAlphaLL) * lUFrac ) >> 16) + lAlphaLL);

	pColor.mBlue  = (unsigned char)((( ((short)lBlueL  - (short)lBlueU)  * lVFrac) >> 16) + lBlueU);
	pColor.mGreen = (unsigned char)((( ((short)lGreenL - (short)lGreenU) * lVFrac) >> 16) + lGreenU);
	pColor.mRed   = (unsigned char)((( ((short)lRedL   - (short)lRedU)   * lVFrac) >> 16) + lRedU);
	pColor.mAlpha = (unsigned char)((( ((short)lAlphaL - (short)lAlphaU) * lVFrac) >> 16) + lAlphaU);
}

void Software3DPainter::GetTextureColor24BitBilinear(float pU, float pV, float pM, Software3DPainter* pThis, 
														Lepra::Color& pColor)
{
	int lM = (int)floor(pM);
	unsigned char* lTextureBits = (unsigned char*)pThis->mTexture->GetColorMap(lM)->GetBuffer();
	int lTexturePitch = pThis->mTexture->GetColorMap(lM)->GetPitch();
	int lTWMask = pThis->mTexture->GetColorMap(lM)->GetWidth() - 1;
	int lTHMask = pThis->mTexture->GetColorMap(lM)->GetHeight() - 1;

	float lOneOverM = 1.0f / (float)(1 << lM);
	pU *= lOneOverM;
	pV *= lOneOverM;

	Lepra::FixedPointMath::Fixed16_16 lUFrac = Lepra::FixedPointMath::FloatToFixed16_16(pU) & 0xFFFF;
	Lepra::FixedPointMath::Fixed16_16 lVFrac = Lepra::FixedPointMath::FloatToFixed16_16(pV) & 0xFFFF;

	int lU0 = (int)floor(pU) & lTWMask;
	int lV0 = (int)floor(pV) & lTHMask;

	int lU1 = (lU0 + 1) & lTWMask;
	int lV1 = (lV0 + 1) & lTHMask;

	if (lU0 < 0)
	{
		lU0 = 0;
		lUFrac = 0;
	}

	if (lV0 < 0)
	{
		lV0 = 0;
		lVFrac = 0;
	}

	if (lU1 >= (int)pThis->mTexture->GetColorMap(lM)->GetWidth())
	{
		lU1 = (int)pThis->mTexture->GetColorMap(lM)->GetWidth() - 1;
	}

	if (lV1 >= (int)pThis->mTexture->GetColorMap(lM)->GetHeight())
	{
		lV1 = (int)pThis->mTexture->GetColorMap(lM)->GetHeight() - 1;
	}

	int lV0Offset = lV0 * lTexturePitch;
	int lV1Offset = lV1 * lTexturePitch;

	unsigned char* lPixelUL = lTextureBits + (lV0Offset + lU0) * 3;
	unsigned char* lPixelUR = lTextureBits + (lV0Offset + lU1) * 3;
	unsigned char* lPixelLL = lTextureBits + (lV1Offset + lU0) * 3;
	unsigned char* lPixelLR = lTextureBits + (lV1Offset + lU1) * 3;

	short lAlphaUL = pThis->GetAlpha(lM, lV0Offset + lU0);
	short lAlphaUR = pThis->GetAlpha(lM, lV0Offset + lU1);
	short lAlphaLL = pThis->GetAlpha(lM, lV1Offset + lU0);
	short lAlphaLR = pThis->GetAlpha(lM, lV1Offset + lU1);

	unsigned char lBlueU  = (unsigned char)((( ((short)lPixelUR[0] - (short)lPixelUL[0] ) * lUFrac ) >> 16) + lPixelUL[0]);
	unsigned char lGreenU = (unsigned char)((( ((short)lPixelUR[1] - (short)lPixelUL[1] ) * lUFrac ) >> 16) + lPixelUL[1]);
	unsigned char lRedU   = (unsigned char)((( ((short)lPixelUR[2] - (short)lPixelUL[2] ) * lUFrac ) >> 16) + lPixelUL[2]);
	unsigned char lAlphaU = (unsigned char)((( (lAlphaUR - lAlphaUL) * lUFrac ) >> 16) + lAlphaUL);

	unsigned char lBlueL  = (unsigned char)((( ((short)lPixelLR[0] - (short)lPixelLL[0] ) * lUFrac ) >> 16) + lPixelLL[0]);
	unsigned char lGreenL = (unsigned char)((( ((short)lPixelLR[1] - (short)lPixelLL[1] ) * lUFrac ) >> 16) + lPixelLL[1]);
	unsigned char lRedL   = (unsigned char)((( ((short)lPixelLR[2] - (short)lPixelLL[2] ) * lUFrac ) >> 16) + lPixelLL[2]);
	unsigned char lAlphaL = (unsigned char)((( (lAlphaLR - lAlphaLL) * lUFrac ) >> 16) + lAlphaLL);

	pColor.mBlue  = (unsigned char)((( ((short)lBlueL  - (short)lBlueU)  * lVFrac) >> 16) + lBlueU);
	pColor.mGreen = (unsigned char)((( ((short)lGreenL - (short)lGreenU) * lVFrac) >> 16) + lGreenU);
	pColor.mRed   = (unsigned char)((( ((short)lRedL   - (short)lRedU)   * lVFrac) >> 16) + lRedU);
	pColor.mAlpha = (unsigned char)((( ((short)lAlphaL - (short)lAlphaU) * lVFrac) >> 16) + lAlphaU);
}

void Software3DPainter::GetTextureColor32BitBilinear(float pU, float pV, float pM, Software3DPainter* pThis, 
														Lepra::Color& pColor)
{
	int lM = (int)floor(pM);
	unsigned int* lTextureBits = (unsigned int*)pThis->mTexture->GetColorMap(lM)->GetBuffer();
	int lTexturePitch = pThis->mTexture->GetColorMap(lM)->GetPitch();
	int lTWMask = pThis->mTexture->GetColorMap(lM)->GetWidth() - 1;
	int lTHMask = pThis->mTexture->GetColorMap(lM)->GetHeight() - 1;

	float lOneOverM = 1.0f / (float)(1 << lM);
	pU *= lOneOverM;
	pV *= lOneOverM;

	Lepra::FixedPointMath::Fixed16_16 lUFrac = Lepra::FixedPointMath::FloatToFixed16_16(pU) & 0xFFFF;
	Lepra::FixedPointMath::Fixed16_16 lVFrac = Lepra::FixedPointMath::FloatToFixed16_16(pV) & 0xFFFF;

	int lU0 = (int)floor(pU) & lTWMask;
	int lV0 = (int)floor(pV) & lTHMask;

	int lU1 = (lU0 + 1) & lTWMask;
	int lV1 = (lV0 + 1) & lTHMask;

	if (lU0 < 0)
	{
		lU0 = 0;
		lUFrac = 0;
	}

	if (lV0 < 0)
	{
		lV0 = 0;
		lVFrac = 0;
	}

	if (lU1 >= (int)pThis->mTexture->GetColorMap(lM)->GetWidth())
	{
		lU1 = (int)pThis->mTexture->GetColorMap(lM)->GetWidth() - 1;
	}

	if (lV1 >= (int)pThis->mTexture->GetColorMap(lM)->GetHeight())
	{
		lV1 = (int)pThis->mTexture->GetColorMap(lM)->GetHeight() - 1;
	}

	int lV0Offset = lV0 * lTexturePitch;
	int lV1Offset = lV1 * lTexturePitch;

	unsigned char* lPixelUL = (unsigned char*)(lTextureBits + lV0Offset + lU0);
	unsigned char* lPixelUR = (unsigned char*)(lTextureBits + lV0Offset + lU1);
	unsigned char* lPixelLL = (unsigned char*)(lTextureBits + lV1Offset + lU0);
	unsigned char* lPixelLR = (unsigned char*)(lTextureBits + lV1Offset + lU1);

	short lAlphaUL = pThis->GetAlpha(lM, lV0Offset + lU0);
	short lAlphaUR = pThis->GetAlpha(lM, lV0Offset + lU1);
	short lAlphaLL = pThis->GetAlpha(lM, lV1Offset + lU0);
	short lAlphaLR = pThis->GetAlpha(lM, lV1Offset + lU1);

	unsigned char lBlueU  = (unsigned char)((( ((short)lPixelUR[0] - (short)lPixelUL[0]) * lUFrac) >> 16) + lPixelUL[0]);
	unsigned char lGreenU = (unsigned char)((( ((short)lPixelUR[1] - (short)lPixelUL[1]) * lUFrac) >> 16) + lPixelUL[1]);
	unsigned char lRedU   = (unsigned char)((( ((short)lPixelUR[2] - (short)lPixelUL[2]) * lUFrac) >> 16) + lPixelUL[2]);
	unsigned char lAlphaU = (unsigned char)((( (lAlphaUR - lAlphaUL) * lUFrac ) >> 16) + lAlphaUL);

	unsigned char lBlueL  = (unsigned char)((( ( (short)lPixelLR[0] - (short)lPixelLL[0] ) * lUFrac ) >> 16) + lPixelLL[0]);
	unsigned char lGreenL = (unsigned char)((( ( (short)lPixelLR[1] - (short)lPixelLL[1] ) * lUFrac ) >> 16) + lPixelLL[1]);
	unsigned char lRedL   = (unsigned char)((( ( (short)lPixelLR[2] - (short)lPixelLL[2] ) * lUFrac ) >> 16) + lPixelLL[2]);
	unsigned char lAlphaL = (unsigned char)((( (lAlphaLR - lAlphaLL) * lUFrac ) >> 16) + lAlphaLL);

	pColor.mBlue  = (unsigned char)((( ((short)lBlueL  - (short)lBlueU)  * lVFrac) >> 16) + lBlueU);
	pColor.mGreen = (unsigned char)((( ((short)lGreenL - (short)lGreenU) * lVFrac) >> 16) + lGreenU);
	pColor.mRed   = (unsigned char)((( ((short)lRedL   - (short)lRedU)   * lVFrac) >> 16) + lRedU);
	pColor.mAlpha = (unsigned char)((( ((short)lAlphaL - (short)lAlphaU) * lVFrac) >> 16) + lAlphaU);
}






void Software3DPainter::GetTextureColor15BitTrilinear(float pU, float pV, float pM, Software3DPainter* pThis, 
														Lepra::Color& pColor)
{
	Lepra::Color lCol0;
	Lepra::Color lCol1;
	GetTextureColor15BitBilinear(pU, pV, (float)floor(pM), pThis, lCol0);
	GetTextureColor15BitBilinear(pU, pV, (float)ceil(pM), pThis, lCol1);

	Lepra::FixedPointMath::Fixed16_16 lMFrac = Lepra::FixedPointMath::FloatToFixed16_16(pM) & 0xFFFF;

	int lR0 = lCol0.mRed;
	int lG0 = lCol0.mGreen;
	int lB0 = lCol0.mBlue;
	int lA0 = lCol0.mAlpha;

	int lR1 = lCol1.mRed;
	int lG1 = lCol1.mGreen;
	int lB1 = lCol1.mBlue;
	int lA1 = lCol1.mAlpha;

	pColor.mRed   = (unsigned char)((((lR1 - lR0) * lMFrac) >> 16) + lR0);
	pColor.mGreen = (unsigned char)((((lG1 - lG0) * lMFrac) >> 16) + lG0);
	pColor.mBlue  = (unsigned char)((((lB1 - lB0) * lMFrac) >> 16) + lB0);
	pColor.mAlpha = (unsigned char)((((lA1 - lA0) * lMFrac) >> 16) + lA0);
}

void Software3DPainter::GetTextureColor16BitTrilinear(float pU, float pV, float pM, Software3DPainter* pThis, 
														Lepra::Color& pColor)
{
	Lepra::Color lCol0;
	Lepra::Color lCol1;
	GetTextureColor16BitBilinear(pU, pV, (float)floor(pM), pThis, lCol0);
	GetTextureColor16BitBilinear(pU, pV, (float)ceil(pM), pThis, lCol1);

	Lepra::FixedPointMath::Fixed16_16 lMFrac = Lepra::FixedPointMath::FloatToFixed16_16(pM) & 0xFFFF;

	int lR0 = lCol0.mRed;
	int lG0 = lCol0.mGreen;
	int lB0 = lCol0.mBlue;
	int lA0 = lCol0.mAlpha;

	int lR1 = lCol1.mRed;
	int lG1 = lCol1.mGreen;
	int lB1 = lCol1.mBlue;
	int lA1 = lCol1.mAlpha;

	pColor.mRed   = (unsigned char)((((lR1 - lR0) * lMFrac) >> 16) + lR0);
	pColor.mGreen = (unsigned char)((((lG1 - lG0) * lMFrac) >> 16) + lG0);
	pColor.mBlue  = (unsigned char)((((lB1 - lB0) * lMFrac) >> 16) + lB0);
	pColor.mAlpha = (unsigned char)((((lA1 - lA0) * lMFrac) >> 16) + lA0);
}

void Software3DPainter::GetTextureColor24BitTrilinear(float pU, float pV, float pM, Software3DPainter* pThis, 
														Lepra::Color& pColor)
{
	Lepra::Color lCol0;
	Lepra::Color lCol1;
	GetTextureColor24BitBilinear(pU, pV, (float)floor(pM), pThis, lCol0);
	GetTextureColor24BitBilinear(pU, pV, (float)ceil(pM), pThis, lCol1);

	Lepra::FixedPointMath::Fixed16_16 lMFrac = Lepra::FixedPointMath::FloatToFixed16_16(pM) & 0xFFFF;

	int lR0 = lCol0.mRed;
	int lG0 = lCol0.mGreen;
	int lB0 = lCol0.mBlue;
	int lA0 = lCol0.mAlpha;

	int lR1 = lCol1.mRed;
	int lG1 = lCol1.mGreen;
	int lB1 = lCol1.mBlue;
	int lA1 = lCol1.mAlpha;

	pColor.mRed   = (unsigned char)((((lR1 - lR0) * lMFrac) >> 16) + lR0);
	pColor.mGreen = (unsigned char)((((lG1 - lG0) * lMFrac) >> 16) + lG0);
	pColor.mBlue  = (unsigned char)((((lB1 - lB0) * lMFrac) >> 16) + lB0);
	pColor.mAlpha = (unsigned char)((((lA1 - lA0) * lMFrac) >> 16) + lA0);
}

void Software3DPainter::GetTextureColor32BitTrilinear(float pU, float pV, float pM, Software3DPainter* pThis, 
														Lepra::Color& pColor)
{
	Lepra::Color lCol0;
	Lepra::Color lCol1;
	GetTextureColor32BitBilinear(pU, pV, (float)floor(pM), pThis, lCol0);
	GetTextureColor32BitBilinear(pU, pV, (float)ceil(pM), pThis, lCol1);

	Lepra::FixedPointMath::Fixed16_16 lMFrac = Lepra::FixedPointMath::FloatToFixed16_16(pM) & 0xFFFF;

	int lR0 = lCol0.mRed;
	int lG0 = lCol0.mGreen;
	int lB0 = lCol0.mBlue;
	int lA0 = lCol0.mAlpha;

	int lR1 = lCol1.mRed;
	int lG1 = lCol1.mGreen;
	int lB1 = lCol1.mBlue;
	int lA1 = lCol1.mAlpha;

	pColor.mRed   = (unsigned char)((((lR1 - lR0) * lMFrac) >> 16) + lR0);
	pColor.mGreen = (unsigned char)((((lG1 - lG0) * lMFrac) >> 16) + lG0);
	pColor.mBlue  = (unsigned char)((((lB1 - lB0) * lMFrac) >> 16) + lB0);
	pColor.mAlpha = (unsigned char)((((lA1 - lA0) * lMFrac) >> 16) + lA0);
}

} // End namespace.
