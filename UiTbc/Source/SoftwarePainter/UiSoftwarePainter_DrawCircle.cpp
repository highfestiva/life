/*
	Lepra::File:   SoftwarePainter_DrawCircle.cpp
	Class:  SoftwarePainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "pch.h"
#include "../../Include/UiSoftwarePainter.h"
#include "UiSoftwarePainter_DrawPixelMacros.h"

#include <math.h>

namespace UiTbc
{

#define MACRO_DRAWCIRCLE(x, y, radius, pThis, type, unitsize, color, drawpixelmacro) \
{ \
	Lepra::Canvas* lCanvas = pThis->mCanvas; \
	Lepra::PixelRect& lClippingRect = pThis->mClippingRect; \
 \
	Lepra::float64 lY; \
	int lY; \
	int lPrevSpanLeftX = (int)(-radius); \
	type* lScreen = (type*)lCanvas->GetBuffer(); \
 \
	for (lY = 0, lY = 0; lY <= radius + 1.0f; lY += 1.0f, lY++) \
	{ \
		int lHalfSpanWidth = (int)(cos(asin(lY / radius)) * radius); \
		int lLowerSpanY = y + lY; \
		int lUpperSpanY = y - lY; \
 \
		if ( lUpperSpanY >= lClippingRect.mTop && \
			lUpperSpanY <  lClippingRect.mBottom) \
		{ \
			int lOffset = lUpperSpanY * lCanvas->GetPitch(); \
 \
			for (int lX = lPrevSpanLeftX; lX <= -lHalfSpanWidth; lX++) \
			{ \
				int lLeftX = x + lX; \
				int lRightX = x - lX; \
 \
				if ( lLeftX >= lClippingRect.mLeft && \
					lLeftX <  lClippingRect.mRight) \
				{ \
					type* lDest = &lScreen[(lOffset + lLeftX) * unitsize]; \
					drawpixelmacro(lDest, color); \
				} \
				if ( lRightX >= lClippingRect.mLeft && \
					lRightX <  lClippingRect.mRight) \
				{ \
					type* lDest = &lScreen[(lOffset + lRightX) * unitsize]; \
					drawpixelmacro(lDest, color); \
				} \
			} \
		} \
 \
		if ( lLowerSpanY >= lClippingRect.mTop && \
			lLowerSpanY <  lClippingRect.mBottom) \
		{ \
			int lOffset = lLowerSpanY * lCanvas->GetPitch(); \
 \
			for (int lX = lPrevSpanLeftX; lX <= -lHalfSpanWidth; lX++) \
			{ \
				int lLeftX = x + lX; \
				int lRightX = x - lX; \
 \
				if ( lLeftX >= lClippingRect.mLeft && \
					lLeftX <  lClippingRect.mRight) \
				{ \
					type* lDest = &lScreen[(lOffset + lLeftX) * unitsize]; \
					drawpixelmacro(lDest, color); \
				} \
				if ( lRightX >= lClippingRect.mLeft && \
					lRightX <  lClippingRect.mRight) \
				{ \
					type* lDest = &lScreen[(lOffset + lRightX) * unitsize]; \
					drawpixelmacro(lDest, color); \
				} \
			} \
		} \
 \
		lPrevSpanLeftX = -lHalfSpanWidth; \
	} \
}

void SoftwarePainter::DrawCircle8BitNormal(int x, int y, Lepra::float64 pRadius, SoftwarePainter* pThis)
{
	Lepra::uint8 lColor = pThis->mColor[0];
	MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
		Lepra::uint8, 1, lColor, MACRO_DrawPixelNormal);
}

void SoftwarePainter::DrawCircle16BitNormal(int x, int y, Lepra::float64 pRadius, SoftwarePainter* pThis)
{
	Lepra::uint16 lColor = pThis->mColor[0];
	MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
		Lepra::uint16, 1, lColor, MACRO_DrawPixelNormal);
}

void SoftwarePainter::DrawCircle24BitNormal(int x, int y, Lepra::float64 pRadius, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->mColor[0];
	MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
		Lepra::uint8, 3, lColor, MACRO_DrawPixel24BitNormal);
}

void SoftwarePainter::DrawCircle32BitNormal(int x, int y, Lepra::float64 pRadius, SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		Lepra::Color& lColor = pThis->mColor[0];
		MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
			Lepra::uint8, 4, lColor, MACRO_DrawPixel32BitNormal);
	}
	else
	{
		unsigned lColor = pThis->mColor[0];
		MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
			unsigned, 1, lColor, MACRO_DrawPixelNormal);
	}
}

void SoftwarePainter::DrawCircle15BitBlend(int x, int y, Lepra::float64 pRadius, SoftwarePainter* pThis)
{
	BlendColor lColor(pThis->mColor[0], pThis->mAlphaValue);
	MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
		Lepra::uint16, 1, lColor, MACRO_DrawPixel15BitBlend);
}

void SoftwarePainter::DrawCircle16BitBlend(int x, int y, Lepra::float64 pRadius, SoftwarePainter* pThis)
{
	BlendColor lColor(pThis->mColor[0], pThis->mAlphaValue);
	MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
		Lepra::uint16, 1, lColor, MACRO_DrawPixel16BitBlend);
}

void SoftwarePainter::DrawCircle24BitBlend(int x, int y, Lepra::float64 pRadius, SoftwarePainter* pThis)
{
	BlendColor lColor(pThis->mColor[0], pThis->mAlphaValue);
	MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
		Lepra::uint8, 3, lColor, MACRO_DrawPixelBlend);
}

void SoftwarePainter::DrawCircle32BitBlend(int x, int y, Lepra::float64 pRadius, SoftwarePainter* pThis)
{
	BlendColor lColor(pThis->mColor[0], pThis->mAlphaValue);
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
			Lepra::uint8, 4, lColor, MACRO_DrawPixel32BitBlend);
	}
	else
	{
		MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
			Lepra::uint8, 4, lColor, MACRO_DrawPixelBlend);
	}
}

void SoftwarePainter::DrawCircle8BitXor(int x, int y, Lepra::float64 pRadius, SoftwarePainter* pThis)
{
	Lepra::uint8 lColor = pThis->mColor[0];
	MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
		Lepra::uint8, 1, lColor, MACRO_DrawPixelXor);
}

void SoftwarePainter::DrawCircle16BitXor(int x, int y, Lepra::float64 pRadius, SoftwarePainter* pThis)
{
	Lepra::uint16 lColor = pThis->mColor[0];
	MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
		Lepra::uint16, 1, lColor, MACRO_DrawPixelXor);
}

void SoftwarePainter::DrawCircle24BitXor(int x, int y, Lepra::float64 pRadius, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->mColor[0];
	MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
		Lepra::uint8, 3, lColor, MACRO_DrawPixel24BitXor);
}

void SoftwarePainter::DrawCircle32BitXor(int x, int y, Lepra::float64 pRadius, SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		Lepra::Color& lColor = pThis->mColor[0];
		MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
			Lepra::uint8, 4, lColor, MACRO_DrawPixel32BitXor);
	}
	else
	{
		unsigned lColor = pThis->mColor[0];
		MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
			unsigned, 1, lColor, MACRO_DrawPixelXor);
	}
}

void SoftwarePainter::DrawCircle8BitAdd(int x, int y, Lepra::float64 pRadius, SoftwarePainter* pThis)
{
	Lepra::uint8 lColor = pThis->mColor[0];
	MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
		Lepra::uint8, 1, lColor, MACRO_DrawPixel8BitAdd);
}

void SoftwarePainter::DrawCircle15BitAdd(int x, int y, Lepra::float64 pRadius, SoftwarePainter* pThis)
{
	Lepra::Color lColor(pThis->mColor[0]);
	lColor.mRed   >>= 3;
	lColor.mGreen >>= 3;
	lColor.mBlue  >>= 3;

	MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
		Lepra::uint16, 1, lColor, MACRO_DrawPixel15BitAdd);
}

void SoftwarePainter::DrawCircle16BitAdd(int x, int y, Lepra::float64 pRadius, SoftwarePainter* pThis)
{
	Lepra::Color lColor(pThis->mColor[0]);
	lColor.mRed   >>= 3;
	lColor.mGreen >>= 2;
	lColor.mBlue  >>= 3;

	MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
		Lepra::uint16, 1, lColor, MACRO_DrawPixel16BitAdd);
}

void SoftwarePainter::DrawCircle24BitAdd(int x, int y, Lepra::float64 pRadius, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->mColor[0];
	MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
		Lepra::uint8, 3, lColor, MACRO_DrawPixelAdd);
}

void SoftwarePainter::DrawCircle32BitAdd(int x, int y, Lepra::float64 pRadius, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->mColor[0];

	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
			Lepra::uint8, 4, lColor, MACRO_DrawPixelAdd32Bit);
	}
	else
	{
		MACRO_DRAWCIRCLE(x, y, pRadius, pThis, 
			Lepra::uint8, 4, lColor, MACRO_DrawPixelAdd);
	}
}

#undef MACRO_DRAWCIRCLE

}
