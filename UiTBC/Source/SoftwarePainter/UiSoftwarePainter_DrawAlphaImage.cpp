/*
	Class:  SoftwarePainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/UiSoftwarePainter.h"
#include "UiSoftwarePainter_DrawPixelMacros.h"

namespace UiTbc
{

#define MACRO_Interpolate8Bit(col0, col1, alpha, outcolor) \
{ \
	outcolor = col0 + (Lepra::uint8)(((Lepra::uint16)(col1 - col0) * (Lepra::uint16)alpha) >> 8); \
}

#define MACRO_Interpolate15Bit(col0, col1, alpha, outcolor) \
{ \
	Lepra::uint16 lR0 = (Lepra::uint16)col0.mRed; \
	Lepra::uint16 lG0 = (Lepra::uint16)col0.mGreen; \
	Lepra::uint16 lB0 = (Lepra::uint16)col0.mBlue; \
 \
	Lepra::uint16 lR1 = (Lepra::uint16)col1.mRed; \
	Lepra::uint16 lG1 = (Lepra::uint16)col1.mGreen; \
	Lepra::uint16 lB1 = (Lepra::uint16)col1.mBlue; \
 \
	Lepra::uint16 lR = lR0 + (((lR1 - lR0) * (Lepra::uint16)alpha) >> 8); \
	Lepra::uint16 lG = lG0 + (((lG1 - lG0) * (Lepra::uint16)alpha) >> 8); \
	Lepra::uint16 lB = lB0 + (((lB1 - lB0) * (Lepra::uint16)alpha) >> 8); \
 \
	outcolor = ((lR >> 3) << 10) | ((lG >> 3) << 5) | (lB >> 3); \
}

#define MACRO_Interpolate16Bit(col0, col1, alpha, outcolor) \
{ \
	Lepra::uint16 lR0 = (Lepra::uint16)col0.mRed; \
	Lepra::uint16 lG0 = (Lepra::uint16)col0.mGreen; \
	Lepra::uint16 lB0 = (Lepra::uint16)col0.mBlue; \
 \
	Lepra::uint16 lR1 = (Lepra::uint16)col1.mRed; \
	Lepra::uint16 lG1 = (Lepra::uint16)col1.mGreen; \
	Lepra::uint16 lB1 = (Lepra::uint16)col1.mBlue; \
 \
	Lepra::uint16 lR = lR0 + (((lR1 - lR0) * (Lepra::uint16)alpha) >> 8); \
	Lepra::uint16 lG = lG0 + (((lG1 - lG0) * (Lepra::uint16)alpha) >> 8); \
	Lepra::uint16 lB = lB0 + (((lB1 - lB0) * (Lepra::uint16)alpha) >> 8); \
 \
	outcolor = ((lR >> 3) << 11) | ((lG >> 2) << 5) | (lB >> 3); \
}

#define MACRO_Interpolate24Bit(col0, col1, alpha, outcolor) \
{ \
	Lepra::uint16 lR0 = (Lepra::uint16)col0.mRed; \
	Lepra::uint16 lG0 = (Lepra::uint16)col0.mGreen; \
	Lepra::uint16 lB0 = (Lepra::uint16)col0.mBlue; \
 \
	Lepra::uint16 lR1 = (Lepra::uint16)col1.mRed; \
	Lepra::uint16 lG1 = (Lepra::uint16)col1.mGreen; \
	Lepra::uint16 lB1 = (Lepra::uint16)col1.mBlue; \
 \
	outcolor.mRed   = (Lepra::uint8)(lR0 + (((lR1 - lR0) * (Lepra::uint16)alpha) >> 8)); \
	outcolor.mGreen = (Lepra::uint8)(lG0 + (((lG1 - lG0) * (Lepra::uint16)alpha) >> 8)); \
	outcolor.mBlue  = (Lepra::uint8)(lB0 + (((lB1 - lB0) * (Lepra::uint16)alpha) >> 8)); \
 \
}

#define MACRO_Interpolate32Bit(col0, col1, alpha, outcolor) \
{ \
	Lepra::uint16 lR0 = (Lepra::uint16)col0.mRed; \
	Lepra::uint16 lG0 = (Lepra::uint16)col0.mGreen; \
	Lepra::uint16 lB0 = (Lepra::uint16)col0.mBlue; \
	Lepra::uint16 lA0 = (Lepra::uint16)col0.mAlpha; \
 \
	Lepra::uint16 lR1 = (Lepra::uint16)col1.mRed; \
	Lepra::uint16 lG1 = (Lepra::uint16)col1.mGreen; \
	Lepra::uint16 lB1 = (Lepra::uint16)col1.mBlue; \
	Lepra::uint16 lA1 = (Lepra::uint16)col1.mAlpha; \
 \
	outcolor.mRed   = (Lepra::uint8)(lR0 + (((lR1 - lR0) * (Lepra::uint16)alpha) >> 8)); \
	outcolor.mGreen = (Lepra::uint8)(lG0 + (((lG1 - lG0) * (Lepra::uint16)alpha) >> 8)); \
	outcolor.mBlue  = (Lepra::uint8)(lB0 + (((lB1 - lB0) * (Lepra::uint16)alpha) >> 8)); \
	outcolor.mAlpha = (Lepra::uint8)(lA0 + (((lA1 - lA0) * (Lepra::uint16)alpha) >> 8)); \
 \
}

#define MACRO_Interpolate24To15Bit(col0, col1, alpha, outcolor) \
{ \
	Lepra::uint16 lR0 = (Lepra::uint16)col0.mRed; \
	Lepra::uint16 lG0 = (Lepra::uint16)col0.mGreen; \
	Lepra::uint16 lB0 = (Lepra::uint16)col0.mBlue; \
 \
	Lepra::uint16 lR1 = (Lepra::uint16)col1.mRed; \
	Lepra::uint16 lG1 = (Lepra::uint16)col1.mGreen; \
	Lepra::uint16 lB1 = (Lepra::uint16)col1.mBlue; \
 \
	outcolor.mRed   = (Lepra::uint8)(lR0 + (((lR1 - lR0) * (Lepra::uint16)alpha) >> 8)) >> 3; \
	outcolor.mGreen = (Lepra::uint8)(lG0 + (((lG1 - lG0) * (Lepra::uint16)alpha) >> 8)) >> 3; \
	outcolor.mBlue  = (Lepra::uint8)(lB0 + (((lB1 - lB0) * (Lepra::uint16)alpha) >> 8)) >> 3; \
 \
}

#define MACRO_Interpolate24To16Bit(col0, col1, alpha, outcolor) \
{ \
	Lepra::uint16 lR0 = (Lepra::uint16)col0.mRed; \
	Lepra::uint16 lG0 = (Lepra::uint16)col0.mGreen; \
	Lepra::uint16 lB0 = (Lepra::uint16)col0.mBlue; \
 \
	Lepra::uint16 lR1 = (Lepra::uint16)col1.mRed; \
	Lepra::uint16 lG1 = (Lepra::uint16)col1.mGreen; \
	Lepra::uint16 lB1 = (Lepra::uint16)col1.mBlue; \
 \
	outcolor.mRed   = (Lepra::uint8)(lR0 + (((lR1 - lR0) * (Lepra::uint16)alpha) >> 8)) >> 3; \
	outcolor.mGreen = (Lepra::uint8)(lG0 + (((lG1 - lG0) * (Lepra::uint16)alpha) >> 8)) >> 2; \
	outcolor.mBlue  = (Lepra::uint8)(lB0 + (((lB1 - lB0) * (Lepra::uint16)alpha) >> 8)) >> 3; \
 \
}

#define MACRO_DRAWALPHAIMAGENORMAL(Image, x, y, pThis, type, unitsize, colortype, color0, color1, interpolatecolorsmacro, drawpixelmacro) \
{ \
	Lepra::PixelRect lClippingRect; \
	pThis->GetScreenCoordClippingRect(lClippingRect); \
	Lepra::Canvas* lCanvas = pThis->GetCanvas(); \
 \
	int lXClipOffset = 0; \
	int lYClipOffset = 0; \
 \
	if (x < lClippingRect.mLeft) \
	{ \
		lXClipOffset = lClippingRect.mLeft - x; \
		x = lClippingRect.mLeft; \
	} \
 \
	if (y < lClippingRect.mTop) \
	{ \
		lYClipOffset = lClippingRect.mTop - y; \
		y = lClippingRect.mTop; \
	} \
 \
	int lRenderWidth = Image.GetWidth() - lXClipOffset; \
	int lRenderHeight = Image.GetHeight() - lYClipOffset; \
 \
	if ((lRenderWidth + x) > lClippingRect.mRight) \
	{ \
		lRenderWidth = lClippingRect.mRight - x; \
	} \
 \
	if ((lRenderHeight + y) > lClippingRect.mBottom) \
	{ \
		lRenderHeight = lClippingRect.mBottom - y; \
	} \
 \
	type* lScreen = (type*)lCanvas->GetBuffer() + (y * lCanvas->GetPitch() + x) * unitsize; \
	Lepra::uint8* lPicture = (Lepra::uint8*)Image.GetBuffer() + lYClipOffset * Image.GetPitch() + lXClipOffset; \
 \
	colortype lColor; \
	int lPitchAdd = lCanvas->GetPitch() * unitsize; \
 \
	for (int lY = 0; lY < lRenderHeight; lY++) \
	{ \
		type* lDest = lScreen; \
 \
		for (int lX = 0; lX < lRenderWidth; lX++) \
		{ \
			Lepra::uint8 lAlpha = lPicture[lX]; \
			interpolatecolorsmacro(color1, color0, lAlpha, lColor); \
			drawpixelmacro(lDest, lColor); \
 \
			lDest += unitsize; \
		} \
 \
		lScreen += lPitchAdd; \
		lPicture += Image.GetPitch(); \
	} \
}

#define MACRO_DRAWALPHAIMAGETEST(Image, x, y, pThis, type, unitsize, color, drawpixelmacro) \
{ \
	Lepra::PixelRect lClippingRect; \
	pThis->GetScreenCoordClippingRect(lClippingRect); \
	Lepra::Canvas* lCanvas = pThis->GetCanvas(); \
 \
	int lXClipOffset = 0; \
	int lYClipOffset = 0; \
 \
	if (x < lClippingRect.mLeft) \
	{ \
		lXClipOffset = lClippingRect.mLeft - x; \
		x = lClippingRect.mLeft; \
	} \
 \
	if (y < lClippingRect.mTop) \
	{ \
		lYClipOffset = lClippingRect.mTop - y; \
		y = lClippingRect.mTop; \
	} \
 \
	int lRenderWidth = Image.GetWidth() - lXClipOffset; \
	int lRenderHeight = Image.GetHeight() - lYClipOffset; \
 \
	if ((lRenderWidth + x) > lClippingRect.mRight) \
	{ \
		lRenderWidth = lClippingRect.mRight - x; \
	} \
 \
	if ((lRenderHeight + y) > lClippingRect.mBottom) \
	{ \
		lRenderHeight = lClippingRect.mBottom - y; \
	} \
 \
	type* lScreen = (type*)lCanvas->GetBuffer() + (y * lCanvas->GetPitch() + x) * unitsize; \
	Lepra::uint8* lPicture = (Lepra::uint8*)Image.GetBuffer() + lYClipOffset * Image.GetPitch() + lXClipOffset; \
 \
	int lPitchAdd = lCanvas->GetPitch() * unitsize; \
	Lepra::uint8 lAlpha = pThis->GetAlphaValue(); \
 \
	for (int lY = 0; lY < lRenderHeight; lY++) \
	{ \
		type* lDest = lScreen; \
 \
		for (int lX = 0; lX < lRenderWidth; lX++) \
		{ \
			if (lPicture[lX] >= lAlpha) \
			{ \
				drawpixelmacro(lDest, color); \
			} \
			lDest += unitsize; \
		} \
 \
		lScreen += lPitchAdd; \
		lPicture += Image.GetPitch(); \
	} \
}

#define MACRO_DRAWALPHAIMAGEBLEND(Image, x, y, pThis, type, unitsize, color, blendpixelmacro) \
{ \
	Lepra::PixelRect lClippingRect; \
	pThis->GetScreenCoordClippingRect(lClippingRect); \
	Lepra::Canvas* lCanvas = pThis->GetCanvas(); \
 \
	int lXClipOffset = 0; \
	int lYClipOffset = 0; \
 \
	if (x < lClippingRect.mLeft) \
	{ \
		lXClipOffset = lClippingRect.mLeft - x; \
		x = lClippingRect.mLeft; \
	} \
 \
	if (y < lClippingRect.mTop) \
	{ \
		lYClipOffset = lClippingRect.mTop - y; \
		y = lClippingRect.mTop; \
	} \
 \
	int lRenderWidth = Image.GetWidth() - lXClipOffset; \
	int lRenderHeight = Image.GetHeight() - lYClipOffset; \
 \
	if ((lRenderWidth + x) > lClippingRect.mRight) \
	{ \
		lRenderWidth = lClippingRect.mRight - x; \
	} \
 \
	if ((lRenderHeight + y) > lClippingRect.mBottom) \
	{ \
		lRenderHeight = lClippingRect.mBottom - y; \
	} \
 \
	type* lScreen = (type*)lCanvas->GetBuffer() + (y * lCanvas->GetPitch() + x) * unitsize; \
	Lepra::uint8* lPicture = (Lepra::uint8*)Image.GetBuffer() + lYClipOffset * Image.GetPitch() + lXClipOffset; \
 \
	int lPitchAdd = lCanvas->GetPitch() * unitsize; \
	Lepra::uint16 lAlpha = (Lepra::uint16)pThis->GetAlphaValue(); \
 \
	for (int lY = 0; lY < lRenderHeight; lY++) \
	{ \
		type* lDest = lScreen; \
 \
		for (int lX = 0; lX < lRenderWidth; lX++) \
		{ \
			Lepra::uint16 lA = ((Lepra::uint16)lPicture[lX] * lAlpha) >> 8; \
 \
			blendpixelmacro(lDest, color, lA); \
 \
			lDest += unitsize; \
		} \
 \
		lScreen += lPitchAdd; \
		lPicture += pImage.GetPitch(); \
	} \
}

void SoftwarePainter::DrawAlphaImage8BitNormal(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::uint8 lColor0 = pThis->mColor8[0];
	Lepra::uint8 lColor1 = pThis->mColor8[1];
	MACRO_DRAWALPHAIMAGENORMAL(pImage, x, y, pThis, 
		Lepra::uint8, 1, Lepra::uint8, lColor0, lColor1, 
		MACRO_Interpolate8Bit, MACRO_DrawPixelNormal);
}

void SoftwarePainter::DrawAlphaImage15BitNormal(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::Color& lColor0 = pThis->GetColorInternal(0);
	Lepra::Color& lColor1 = pThis->GetColorInternal(1);
	MACRO_DRAWALPHAIMAGENORMAL(pImage, x, y, pThis, 
		Lepra::uint16, 1, Lepra::uint16, lColor0, lColor1, 
		MACRO_Interpolate15Bit, MACRO_DrawPixelNormal);
}

void SoftwarePainter::DrawAlphaImage16BitNormal(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::Color& lColor0 = pThis->GetColorInternal(0);
	Lepra::Color& lColor1 = pThis->GetColorInternal(1);
	MACRO_DRAWALPHAIMAGENORMAL(pImage, x, y, pThis, 
		Lepra::uint16, 1, Lepra::uint16, lColor0, lColor1, 
		MACRO_Interpolate16Bit, MACRO_DrawPixelNormal);
}

void SoftwarePainter::DrawAlphaImage24BitNormal(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::Color& lColor0 = pThis->GetColorInternal(0);
	Lepra::Color& lColor1 = pThis->GetColorInternal(1);
	MACRO_DRAWALPHAIMAGENORMAL(pImage, x, y, pThis, 
		Lepra::uint8, 3, Lepra::Color, lColor0, lColor1, 
		MACRO_Interpolate24Bit, MACRO_DrawPixel24BitNormal);
}

void SoftwarePainter::DrawAlphaImage32BitNormal(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::Color& lColor0 = pThis->GetColorInternal(0);
	Lepra::Color& lColor1 = pThis->GetColorInternal(1);

	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWALPHAIMAGENORMAL(pImage, x, y, pThis, 
			Lepra::uint8, 4, Lepra::Color, lColor0, lColor1, 
			MACRO_Interpolate32Bit, MACRO_DrawPixel32BitNormal);
	}
	else
	{
		MACRO_DRAWALPHAIMAGENORMAL(pImage, x, y, pThis, 
			Lepra::uint8, 4, Lepra::Color, lColor0, lColor1, 
			MACRO_Interpolate32Bit, MACRO_DrawPixel24BitNormal);
	}
}

void SoftwarePainter::DrawAlphaImage15BitBlend(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->GetColorInternal(0);
	MACRO_DRAWALPHAIMAGEBLEND(pImage, x, y, pThis, 
		Lepra::uint16, 1, lColor, MACRO_BlendPixel15Bit);
}

void SoftwarePainter::DrawAlphaImage16BitBlend(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->GetColorInternal(0);
	MACRO_DRAWALPHAIMAGEBLEND(pImage, x, y, pThis, 
		Lepra::uint16, 1, lColor, MACRO_BlendPixel16Bit);
}

void SoftwarePainter::DrawAlphaImage24BitBlend(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->GetColorInternal(0);
	MACRO_DRAWALPHAIMAGEBLEND(pImage, x, y, pThis, 
		Lepra::uint8, 3, lColor, MACRO_BlendPixel24Bit);
}

void SoftwarePainter::DrawAlphaImage32BitBlend(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->GetColorInternal(0);

	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWALPHAIMAGEBLEND(pImage, x, y, pThis, 
			Lepra::uint8, 4, lColor, MACRO_BlendPixel32Bit);
	}
	else
	{
		MACRO_DRAWALPHAIMAGEBLEND(pImage, x, y, pThis, 
			Lepra::uint8, 4, lColor, MACRO_BlendPixel24Bit);
	}
}

void SoftwarePainter::DrawAlphaImage8BitTest(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::uint8 lColor = pThis->mColor8[0];
	MACRO_DRAWALPHAIMAGETEST(pImage, x, y, pThis, 
		Lepra::uint8, 1, lColor, MACRO_DrawPixelNormal);
}

void SoftwarePainter::DrawAlphaImage16BitTest(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::uint16 lColor = pThis->mColor16[0];
	MACRO_DRAWALPHAIMAGETEST(pImage, x, y, pThis, 
		Lepra::uint16, 1, lColor, MACRO_DrawPixelNormal);
}

void SoftwarePainter::DrawAlphaImage24BitTest(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->GetColorInternal(0);
	MACRO_DRAWALPHAIMAGETEST(pImage, x, y, pThis, 
		Lepra::uint8, 3, lColor, MACRO_DrawPixel24BitNormal);
}

void SoftwarePainter::DrawAlphaImage32BitTest(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		Lepra::Color& lColor = pThis->GetColorInternal(0);
		MACRO_DRAWALPHAIMAGETEST(pImage, x, y, pThis, 
			Lepra::uint8, 4, lColor, MACRO_DrawPixel32BitNormal);
	}
	else
	{
		unsigned lColor = pThis->mColor32[0];
		MACRO_DRAWALPHAIMAGETEST(pImage, x, y, pThis, 
			unsigned, 1, lColor, MACRO_DrawPixelNormal);
	}
}

void SoftwarePainter::DrawAlphaImage8BitXor(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::uint8 lColor0 = pThis->mColor8[0];
	Lepra::uint8 lColor1 = pThis->mColor8[1];
	MACRO_DRAWALPHAIMAGENORMAL(pImage, x, y, pThis, 
		Lepra::uint8, 1, Lepra::uint8, lColor0, lColor1, 
		MACRO_Interpolate8Bit, MACRO_DrawPixelXor);
}

void SoftwarePainter::DrawAlphaImage16BitXor(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::Color& lColor0 = pThis->GetColorInternal(0);
	Lepra::Color& lColor1 = pThis->GetColorInternal(1);
	MACRO_DRAWALPHAIMAGENORMAL(pImage, x, y, pThis, 
		Lepra::uint16, 1, Lepra::uint16, lColor0, lColor1, 
		MACRO_Interpolate16Bit, MACRO_DrawPixelXor);
}

void SoftwarePainter::DrawAlphaImage24BitXor(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::Color& lColor0 = pThis->GetColorInternal(0);
	Lepra::Color& lColor1 = pThis->GetColorInternal(1);
	MACRO_DRAWALPHAIMAGENORMAL(pImage, x, y, pThis, 
		Lepra::uint8, 3, Lepra::Color, lColor0, lColor1, 
		MACRO_Interpolate24Bit, MACRO_DrawPixel24BitXor);
}

void SoftwarePainter::DrawAlphaImage32BitXor(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::Color& lColor0 = pThis->GetColorInternal(0);
	Lepra::Color& lColor1 = pThis->GetColorInternal(1);

	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWALPHAIMAGENORMAL(pImage, x, y, pThis, 
			Lepra::uint8, 4, Lepra::Color, lColor0, lColor1, 
			MACRO_Interpolate32Bit, MACRO_DrawPixel32BitXor);
	}
	else
	{
		MACRO_DRAWALPHAIMAGENORMAL(pImage, x, y, pThis, 
			Lepra::uint8, 4, Lepra::Color, lColor0, lColor1, 
			MACRO_Interpolate32Bit, MACRO_DrawPixel24BitXor);
	}
}

void SoftwarePainter::DrawAlphaImage8BitAdd(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::uint8 lColor0 = pThis->mColor8[0];
	Lepra::uint8 lColor1 = pThis->mColor8[1];
	MACRO_DRAWALPHAIMAGENORMAL(pImage, x, y, pThis, 
		Lepra::uint16, 1, Lepra::uint8, lColor0, lColor1,
		MACRO_Interpolate8Bit, MACRO_DrawPixel8BitAdd);
}

void SoftwarePainter::DrawAlphaImage15BitAdd(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::Color& lColor0 = pThis->GetColorInternal(0);
	Lepra::Color& lColor1 = pThis->GetColorInternal(1);
	MACRO_DRAWALPHAIMAGENORMAL(pImage, x, y, pThis, 
		Lepra::uint16, 1, Lepra::Color, lColor0, lColor1,
		MACRO_Interpolate24To15Bit, MACRO_DrawPixel15BitAdd);
}

void SoftwarePainter::DrawAlphaImage16BitAdd(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::Color& lColor0 = pThis->GetColorInternal(0);
	Lepra::Color& lColor1 = pThis->GetColorInternal(1);
	MACRO_DRAWALPHAIMAGENORMAL(pImage, x, y, pThis, 
		Lepra::uint16, 1, Lepra::Color, lColor0, lColor1,
		MACRO_Interpolate24To16Bit, MACRO_DrawPixel16BitAdd);
}

void SoftwarePainter::DrawAlphaImage24BitAdd(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::Color& lColor0 = pThis->GetColorInternal(0);
	Lepra::Color& lColor1 = pThis->GetColorInternal(1);
	MACRO_DRAWALPHAIMAGENORMAL(pImage, x, y, pThis, 
		Lepra::uint8, 3, Lepra::Color, lColor0, lColor1,
		MACRO_Interpolate24Bit, MACRO_DrawPixelAdd);
}

void SoftwarePainter::DrawAlphaImage32BitAdd(const Lepra::Canvas& pImage, int x, int y, SoftwarePainter* pThis)
{
	Lepra::Color& lColor0 = pThis->GetColorInternal(0);
	Lepra::Color& lColor1 = pThis->GetColorInternal(1);

	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWALPHAIMAGENORMAL(pImage, x, y, pThis, 
			Lepra::uint8, 4, Lepra::Color, lColor0, lColor1,
			MACRO_Interpolate32Bit, MACRO_DrawPixelAdd32Bit);
	}
	else
	{
		MACRO_DRAWALPHAIMAGENORMAL(pImage, x, y, pThis, 
			Lepra::uint8, 4, Lepra::Color, lColor0, lColor1,
			MACRO_Interpolate32Bit, MACRO_DrawPixelAdd);
	}
}

#undef MACRO_Interpolate8Bit
#undef MACRO_Interpolate15Bit
#undef MACRO_Interpolate16Bit
#undef MACRO_Interpolate24Bit
#undef MACRO_Interpolate32Bit
#undef MACRO_Interpolate24To15Bit
#undef MACRO_Interpolate24To16Bit
#undef MACRO_DRAWALPHAIMAGENORMAL
#undef MACRO_DRAWALPHAIMAGETEST
#undef MACRO_DRAWALPHAIMAGEBLEND

} // End namespace.