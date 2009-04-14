/*
	Lepra::File:   SoftwarePainter_DrawShadedHLine.cpp
	Class:  SoftwarePainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/UiSoftwarePainter.h"
#include "UiSoftwarePainter_DrawPixelMacros.h"

namespace UiTbc
{

#define MACRO_DRAWHLINE(x1, x2, y, lr, lg, lb, rr, rg, rb, pThis, type, unitsize, drawpixelmacro) \
{ \
	int lLength = (x2 - x1); \
 \
	if (lLength == 0) \
		return; \
 \
	Lepra::Canvas* lCanvas = pThis->GetCanvas(); \
	type* lDest = (type*)lCanvas->GetBuffer(); \
	lDest += (pY * lCanvas->GetPitch() + pX1) * unitsize; \
\
	int lLeftRed   = (int)(lr * 1073741824.0); \
	int lLeftGreen = (int)(lg * 1073741824.0); \
	int lLeftBlue  = (int)(lb * 1073741824.0); \
 \
	int lRightRed   = (int)(rr * 1073741824.0); \
	int lRightGreen = (int)(rg * 1073741824.0); \
	int lRightBlue  = (int)(rb * 1073741824.0); \
 \
	int lRedAdd   = (lRightRed   - lLeftRed  ) / lLength; \
	int lGreenAdd = (lRightGreen - lLeftGreen) / lLength; \
	int lBlueAdd  = (lRightBlue  - lLeftBlue ) / lLength; \
 \
	int lRed   = lLeftRed;   \
	int lGreen = lLeftGreen; \
	int lBlue  = lLeftBlue;  \
	Lepra::uint8 lAlphaValue = pThis->GetAlphaValue(); \
 \
	for (int i = 0; i < lLength; i++) \
	{ \
		drawpixelmacro(lDest, (lRed >> 22), (lGreen >> 22), (lBlue >> 22), lAlphaValue); \
 \
		lRed   += lRedAdd; \
		lGreen += lGreenAdd; \
		lBlue  += lBlueAdd; \
 \
		lDest += unitsize; \
	} \
}

#define MACRO_DRAWHLINE32(x1, x2, y, lr, lg, lb, la, rr, rg, rb, ra, pThis, drawpixelmacro) \
{ \
	int lLength = (x2 - x1); \
 \
	if (lLength == 0) \
		return; \
 \
	Lepra::Canvas* lCanvas = pThis->GetCanvas(); \
	Lepra::uint8* lDest = (Lepra::uint8*)lCanvas->GetBuffer(); \
	lDest += (pY * lCanvas->GetPitch() + pX1) * 4; \
\
	int lLeftRed   = (int)(lr * 1073741824.0); \
	int lLeftGreen = (int)(lg * 1073741824.0); \
	int lLeftBlue  = (int)(lb * 1073741824.0); \
	int lLeftAlpha = (int)(la * 1073741824.0); \
 \
	int lRightRed   = (int)(rr * 1073741824.0); \
	int lRightGreen = (int)(rg * 1073741824.0); \
	int lRightBlue  = (int)(rb * 1073741824.0); \
	int lRightAlpha = (int)(ra * 1073741824.0); \
 \
	int lRedAdd   = (lRightRed   - lLeftRed  ) / lLength; \
	int lGreenAdd = (lRightGreen - lLeftGreen) / lLength; \
	int lBlueAdd  = (lRightBlue  - lLeftBlue ) / lLength; \
	int lAlphaAdd = (lRightAlpha - lLeftAlpha) / lLength; \
 \
	int lRed   = lLeftRed;   \
	int lGreen = lLeftGreen; \
	int lBlue  = lLeftBlue;  \
	int lAlpha = lLeftAlpha; \
	Lepra::uint8 lAlphaValue = pThis->GetAlphaValue(); \
 \
	for (int i = 0; i < lLength; i++) \
	{ \
		drawpixelmacro(lDest, (lRed >> 22), (lGreen >> 22), (lBlue >> 22), (lAlpha >> 22), lAlphaValue); \
 \
		lRed   += lRedAdd;   \
		lGreen += lGreenAdd; \
		lBlue  += lBlueAdd;  \
		lAlpha += lAlphaAdd; \
 \
		lDest += 4; \
	} \
}

void SoftwarePainter::DrawShadedHLine8BitNormal(int pX1, int pX2, int pY, float r1,
	float g1, float b1, float /*a1*/, float r2, float g2,
	float b2, float /*a2*/, SoftwarePainter* pThis)
{
	// Guess what... This doesn't draw a shaded line at all! :)
	MACRO_DRAWHLINE(pX1, pX2, pY,	r1, g1, b1,	r2, g2, b2,
		pThis, unsigned short, 1, MACRO_DrawPixel8BitFromRGB);
}

void SoftwarePainter::DrawShadedHLine15BitNormal(int pX1, int pX2, int pY, float r1,
	float g1, float b1, float /*a1*/, float r2, float g2,
	float b2, float /*a2*/, SoftwarePainter* pThis)
{
	MACRO_DRAWHLINE(pX1, pX2, pY,	r1, g1, b1,	r2, g2, b2,
		pThis, unsigned short, 1, MACRO_DrawPixel15BitFromRGB);
}

void SoftwarePainter::DrawShadedHLine16BitNormal(int pX1, int pX2, int pY, float r1,
	float g1, float b1, float /*a1*/, float r2, float g2,
	float b2, float /*a2*/, SoftwarePainter* pThis)
{
	MACRO_DRAWHLINE(pX1, pX2, pY,	r1, g1, b1,	r2, g2, b2,
		pThis, unsigned short, 1, MACRO_DrawPixel16BitFromRGB);
}

void SoftwarePainter::DrawShadedHLine24BitNormal(int pX1, int pX2, int pY, float r1,
	float g1, float b1, float /*a1*/, float r2, float g2,
	float b2, float /*a2*/, SoftwarePainter* pThis)
{
	MACRO_DRAWHLINE(pX1, pX2, pY,	r1, g1, b1,	r2, g2, b2,
		pThis, Lepra::uint8, 3, MACRO_DrawPixel24BitFromRGB);
}

void SoftwarePainter::DrawShadedHLine32BitNormal(int pX1, int pX2, int pY, float r1,
	float g1, float b1, float a1, float r2, float g2,
	float b2, float a2, SoftwarePainter* pThis)
{
	MACRO_DRAWHLINE32(pX1, pX2, pY, r1, g1, b1, a1, r2, g2, b2, a2,
		pThis, MACRO_DrawPixel32BitFromRGBA);
}

void SoftwarePainter::DrawShadedHLine15BitBlend(int pX1, int pX2, int pY, float r1,
	float g1, float b1, float /*a1*/, float r2, float g2,
	float b2, float /*a2*/, SoftwarePainter* pThis)
{
	MACRO_DRAWHLINE(pX1, pX2, pY,	r1, g1, b1,	r2, g2, b2,
		pThis, unsigned short, 1, MACRO_BlendPixel15BitFromRGB);
}

void SoftwarePainter::DrawShadedHLine16BitBlend(int pX1, int pX2, int pY, float r1,
	float g1, float b1, float /*a1*/, float r2, float g2,
	float b2, float /*a2*/, SoftwarePainter* pThis)
{
	MACRO_DRAWHLINE(pX1, pX2, pY,	r1, g1, b1,	r2, g2, b2,
		pThis, unsigned short, 1, MACRO_BlendPixel16BitFromRGB);
}

void SoftwarePainter::DrawShadedHLine24BitBlend(int pX1, int pX2, int pY, float r1,
	float g1, float b1, float /*a1*/, float r2, float g2,
	float b2, float /*a2*/, SoftwarePainter* pThis)
{
	MACRO_DRAWHLINE(pX1, pX2, pY,	r1, g1, b1,	r2, g2, b2,
		pThis, Lepra::uint8, 3, MACRO_BlendPixel24BitFromRGB);
}

void SoftwarePainter::DrawShadedHLine32BitBlend(int pX1, int pX2, int pY, float r1,
	float g1, float b1, float a1, float r2, float g2,
	float b2, float a2, SoftwarePainter* pThis)
{
	MACRO_DRAWHLINE32(pX1, pX2, pY, r1, g1, b1, a1, r2, g2, b2, a2,
		pThis, MACRO_DrawPixel32BitFromRGBA);
}

void SoftwarePainter::DrawShadedHLine15BitXor(int pX1, int pX2, int pY, float r1,
	float g1, float b1, float /*a1*/, float r2, float g2,
	float b2, float /*a2*/, SoftwarePainter* pThis)
{
	MACRO_DRAWHLINE(pX1, pX2, pY,	r1, g1, b1,	r2, g2, b2,
		pThis, unsigned short, 1, MACRO_XorPixel15BitFromRGB);
}

void SoftwarePainter::DrawShadedHLine16BitXor(int pX1, int pX2, int pY, float r1,
	float g1, float b1, float /*a1*/, float r2, float g2,
	float b2, float /*a2*/, SoftwarePainter* pThis)
{
	MACRO_DRAWHLINE(pX1, pX2, pY,	r1, g1, b1,	r2, g2, b2,
		pThis, unsigned short, 1, MACRO_XorPixel16BitFromRGB);
}

void SoftwarePainter::DrawShadedHLine24BitXor(int pX1, int pX2, int pY, float r1,
	float g1, float b1, float /*a1*/, float r2, float g2,
	float b2, float /*a2*/, SoftwarePainter* pThis)
{
	MACRO_DRAWHLINE(pX1, pX2, pY,	r1, g1, b1,	r2, g2, b2,
		pThis, Lepra::uint8, 3, MACRO_XorPixel24BitFromRGB);
}

void SoftwarePainter::DrawShadedHLine32BitXor(int pX1, int pX2, int pY, float r1,
	float g1, float b1, float a1, float r2, float g2,
	float b2, float a2, SoftwarePainter* pThis)
{
	MACRO_DRAWHLINE32(pX1, pX2, pY, r1, g1, b1, a1, r2, g2, b2, a2,
		pThis, MACRO_XorPixel32BitFromRGBA);
}

void SoftwarePainter::DrawShadedHLine15BitAdd(int pX1, int pX2, int pY, float r1,
	float g1, float b1, float /*a1*/, float r2, float g2,
	float b2, float /*a2*/, SoftwarePainter* pThis)
{
	MACRO_DRAWHLINE(pX1, pX2, pY,	r1, g1, b1,	r2, g2, b2,
		pThis, unsigned short, 1, MACRO_AddPixel15BitFromRGB);
}

void SoftwarePainter::DrawShadedHLine16BitAdd(int pX1, int pX2, int pY, float r1,
	float g1, float b1, float /*a1*/, float r2, float g2,
	float b2, float /*a2*/, SoftwarePainter* pThis)
{
	MACRO_DRAWHLINE(pX1, pX2, pY,	r1, g1, b1,	r2, g2, b2,
		pThis, unsigned short, 1, MACRO_AddPixel16BitFromRGB);
}

void SoftwarePainter::DrawShadedHLine24BitAdd(int pX1, int pX2, int pY, float r1,
	float g1, float b1, float /*a1*/, float r2, float g2,
	float b2, float /*a2*/, SoftwarePainter* pThis)
{
	MACRO_DRAWHLINE(pX1, pX2, pY,	r1, g1, b1,	r2, g2, b2,
		pThis, Lepra::uint8, 3, MACRO_AddPixel24BitFromRGB);
}

void SoftwarePainter::DrawShadedHLine32BitAdd(int pX1, int pX2, int pY, float r1,
	float g1, float b1, float a1, float r2, float g2,
	float b2, float a2, SoftwarePainter* pThis)
{
	MACRO_DRAWHLINE32(pX1, pX2, pY, r1, g1, b1, a1, r2, g2, b2, a2,
		pThis, MACRO_AddPixel32BitFromRGBA);
}

#undef MACRO_DRAWHLINE


} // End namespace.