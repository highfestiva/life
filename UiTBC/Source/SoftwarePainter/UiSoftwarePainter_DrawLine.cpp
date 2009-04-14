/*
	Lepra::File:   SoftwarePainter_DrawLine.cpp
	Class:  SoftwarePainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/UiSoftwarePainter.h"
#include "UiSoftwarePainter_DrawPixelMacros.h"

namespace UiTbc
{

#define MACRO_CLIPLINE(x1, y1, x2, y2, pThis) \
{ \
	Lepra::PixelRect lClippingRect; \
	pThis->GetScreenCoordClippingRect(lClippingRect); \
	if ( \
		(x1 <  lClippingRect.mLeft   && x2 <  lClippingRect.mLeft) ||  \
		(x1 >= lClippingRect.mRight  && x2 >= lClippingRect.mRight) || \
	    (y1 <  lClippingRect.mTop    && y2 <  lClippingRect.mTop) ||  \
		(y1 >= lClippingRect.mBottom && y2 >= lClippingRect.mBottom) || \
		lClippingRect.mBottom == lClippingRect.mTop || \
		lClippingRect.mRight == lClippingRect.mLeft \
	  ) \
	{ \
	   return; \
	} \
 \
	int lTemp; \
 \
	/* Make sure y1 is above y2. */ \
	if (y1 > y2) \
	{ \
		lTemp = y1; \
		y1 = y2; \
		y2 = lTemp; \
 \
		lTemp = x1; \
		x1 = x2; \
		x2 = lTemp; \
	} \
 \
	/* Clip to top. */ \
	if (y1 < lClippingRect.mTop) \
	{ \
		x1 = x1 + ((x2 - x1) * (lClippingRect.mTop - y1)) / (y2 - y1); \
		y1 = lClippingRect.mTop; \
	} \
 \
	/* Clip to bottom. */ \
	if (y2 > lClippingRect.mBottom) \
	{ \
		x2 = x2 + ((x2 - x1) * (lClippingRect.mBottom - y2)) / (y2 - y1); \
		y2 = lClippingRect.mBottom; \
	} \
 \
	/* Check if there is anything left within the screen... */ \
	if ( \
		(x1 < lClippingRect.mLeft && x2 < lClippingRect.mLeft) ||  \
		(x1 >= lClippingRect.mRight && x2 >= lClippingRect.mRight) \
	  ) \
	{ \
		return; \
	} \
 \
	/* Make sure x1 is to the left of x2. */ \
	if (x1 > x2) \
	{ \
		lTemp = y1; \
		y1 = y2; \
		y2 = lTemp; \
 \
		lTemp = x1; \
		x1 = x2; \
		x2 = lTemp; \
	} \
 \
	/* Clip to left. */ \
	if (x1 < lClippingRect.mLeft) \
	{ \
		y1 = y1 + ((y2 - y1) * (lClippingRect.mLeft - x1)) / (x2 - x1); \
		x1 = lClippingRect.mLeft; \
	} \
 \
	/* Clip to right. */ \
	if (x2 > lClippingRect.mRight) \
	{ \
		y2 = y2 + ((y2 - y1) * (lClippingRect.mRight - x2)) / (x2 - x1); \
		x2 = lClippingRect.mRight; \
	} \
 \
	/* Check if there is anything left within the screen... */ \
	if ( y1 < lClippingRect.mTop && y2 < lClippingRect.mTop ||  \
		y1 >= lClippingRect.mBottom && y2 >= lClippingRect.mBottom) \
	{ \
	   return; \
	} \
}

/*
	The line rendering algorithm (a Bresenham algorithm).
*/
#define MACRO_DRAWLINE(x1, y1, x2, y2, pThis, type, unitsize, color, drawpixelmacro) \
{ \
	Lepra::Canvas* lCanvas = pThis->GetCanvas(); \
	unsigned lOffset = (y1 * lCanvas->GetPitch() + x1) * unitsize; \
 \
	int lXUnit; \
	int lYUnit; \
 \
	int lDy = y2 - y1; \
 \
	if (lDy < 0) \
	{ \
		lDy = -lDy; \
		lYUnit = -((int)lCanvas->GetPitch() * unitsize); \
	} \
	else \
	{ \
		lYUnit = lCanvas->GetPitch() * unitsize; \
	} \
 \
	int lDx = x2 - x1; \
 \
	if (lDx < 0) \
	{ \
		lDx = -lDx; \
		lXUnit = -unitsize; \
	} \
	else \
	{ \
		lXUnit = unitsize; \
	} \
 \
	type* lScreen = (type*)lCanvas->GetBuffer(); \
 \
	if (lDx > lDy) \
	{ \
		int lLength = lDx; \
		int lError = 0; \
 \
		for (int i = 0; i < lLength; i++) \
		{ \
			drawpixelmacro((lScreen + lOffset), color); \
			lOffset += lXUnit; \
 \
			lError += lDy; \
 \
			if (lError >= lDx) \
			{ \
				lError -= lDx; \
				lOffset += lYUnit; \
			} \
		} \
	} \
	else \
	{ \
		int lLength = lDy; \
		int lError = 0; \
 \
		for (int i = 0; i < lLength; i++) \
		{ \
			drawpixelmacro((lScreen + lOffset), color); \
			lOffset += lYUnit; \
 \
			lError += lDx; \
 \
			if (lError >= lDy) \
			{ \
				lError -= lDy; \
				lOffset += lXUnit; \
			} \
		} \
	} \
}


/*
	Implementation of the draw line functions.
*/

void SoftwarePainter::DrawLine8BitNormal(int x1, int y1, int x2, int y2, SoftwarePainter* pThis)
{
	Lepra::uint8 lColor = pThis->mColor8[0];
	MACRO_CLIPLINE(x1, y1, x2, y2, pThis);
	MACRO_DRAWLINE(x1, y1, x2, y2, pThis, 
		Lepra::uint8, 1, lColor, MACRO_DrawPixelNormal);
}

void SoftwarePainter::DrawLine16BitNormal(int x1, int y1, int x2, int y2, SoftwarePainter* pThis)
{
	Lepra::uint16 lColor = pThis->mColor16[0];
	MACRO_CLIPLINE(x1, y1, x2, y2, pThis);
	MACRO_DRAWLINE(x1, y1, x2, y2, pThis, 
		Lepra::uint16, 1, lColor, MACRO_DrawPixelNormal);
}

void SoftwarePainter::DrawLine24BitNormal(int x1, int y1, int x2, int y2, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->GetColorInternal(0);
	MACRO_CLIPLINE(x1, y1, x2, y2, pThis);
	MACRO_DRAWLINE(x1, y1, x2, y2, pThis, 
		Lepra::uint8, 3, lColor, MACRO_DrawPixel24BitNormal);
}

void SoftwarePainter::DrawLine32BitNormal(int x1, int y1, int x2, int y2, SoftwarePainter* pThis)
{
	MACRO_CLIPLINE(x1, y1, x2, y2, pThis);
	if (pThis->mIncrementalAlpha == true)
	{
		Lepra::Color& lColor = pThis->GetColorInternal(0);
		MACRO_DRAWLINE(x1, y1, x2, y2, pThis,
			Lepra::uint8, 4, lColor, MACRO_DrawPixel32BitNormal);
	}
	else
	{
		unsigned lColor = pThis->mColor32[0];
		MACRO_DRAWLINE(x1, y1, x2, y2, pThis, 
			unsigned, 1, lColor, MACRO_DrawPixelNormal);
	}
}

void SoftwarePainter::DrawLine15BitBlend(int x1, int y1, int x2, int y2, SoftwarePainter* pThis)
{
	BlendColor lColor(pThis->GetColorInternal(0), pThis->GetAlphaValue());
	MACRO_CLIPLINE(x1, y1, x2, y2, pThis);
	MACRO_DRAWLINE(x1, y1, x2, y2, pThis, 
		Lepra::uint16, 1, lColor, MACRO_DrawPixel15BitBlend);
}

void SoftwarePainter::DrawLine16BitBlend(int x1, int y1, int x2, int y2, SoftwarePainter* pThis)
{
	BlendColor lColor(pThis->GetColorInternal(0), pThis->GetAlphaValue());
	MACRO_CLIPLINE(x1, y1, x2, y2, pThis);
	MACRO_DRAWLINE(x1, y1, x2, y2, pThis, 
		Lepra::uint16, 1, lColor, MACRO_DrawPixel16BitBlend);
}

void SoftwarePainter::DrawLine24BitBlend(int x1, int y1, int x2, int y2, SoftwarePainter* pThis)
{
	BlendColor lColor(pThis->GetColorInternal(0), pThis->GetAlphaValue());
	MACRO_CLIPLINE(x1, y1, x2, y2, pThis);
	MACRO_DRAWLINE(x1, y1, x2, y2, pThis, 
		Lepra::uint8, 3, lColor, MACRO_DrawPixelBlend);
}

void SoftwarePainter::DrawLine32BitBlend(int x1, int y1, int x2, int y2, SoftwarePainter* pThis)
{
	BlendColor lColor(pThis->GetColorInternal(0), pThis->GetAlphaValue());
	MACRO_CLIPLINE(x1, y1, x2, y2, pThis);
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWLINE(x1, y1, x2, y2, pThis,
			Lepra::uint8, 4, lColor, MACRO_DrawPixel32BitBlend);
	}
	else
	{
		MACRO_DRAWLINE(x1, y1, x2, y2, pThis, 
			Lepra::uint8, 4, lColor, MACRO_DrawPixelBlend);
	}
}

void SoftwarePainter::DrawLine8BitXor(int x1, int y1, int x2, int y2, SoftwarePainter* pThis)
{
	Lepra::uint8 lColor = pThis->mColor8[0];
	MACRO_CLIPLINE(x1, y1, x2, y2, pThis);
	MACRO_DRAWLINE(x1, y1, x2, y2, pThis, 
		Lepra::uint8, 1, lColor, MACRO_DrawPixelXor);
}

void SoftwarePainter::DrawLine16BitXor(int x1, int y1, int x2, int y2, SoftwarePainter* pThis)
{
	Lepra::uint16 lColor = pThis->mColor16[0];
	MACRO_CLIPLINE(x1, y1, x2, y2, pThis);
	MACRO_DRAWLINE(x1, y1, x2, y2, pThis, 
		Lepra::uint16, 1, lColor, MACRO_DrawPixelXor);
}

void SoftwarePainter::DrawLine24BitXor(int x1, int y1, int x2, int y2, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->GetColorInternal(0);
	MACRO_CLIPLINE(x1, y1, x2, y2, pThis);
	MACRO_DRAWLINE(x1, y1, x2, y2, pThis, 
		Lepra::uint8, 3, lColor, MACRO_DrawPixel24BitXor);
}

void SoftwarePainter::DrawLine32BitXor(int x1, int y1, int x2, int y2, SoftwarePainter* pThis)
{
	MACRO_CLIPLINE(x1, y1, x2, y2, pThis);
	if (pThis->mIncrementalAlpha == true)
	{
		Lepra::Color& lColor = pThis->GetColorInternal(0);
		MACRO_DRAWLINE(x1, y1, x2, y2, pThis,
			Lepra::uint8, 4, lColor, MACRO_DrawPixel32BitXor);
	}
	else
	{
		unsigned lColor = pThis->mColor32[0];
		MACRO_DRAWLINE(x1, y1, x2, y2, pThis, 
			unsigned, 1, lColor, MACRO_DrawPixelXor);
	}
}

void SoftwarePainter::DrawLine8BitAdd(int x1, int y1, int x2, int y2, SoftwarePainter* pThis)
{
	Lepra::uint8 lColor = pThis->mColor8[0];
	MACRO_CLIPLINE(x1, y1, x2, y2, pThis);
	MACRO_DRAWLINE(x1, y1, x2, y2, pThis, 
		Lepra::uint8, 1, lColor, MACRO_DrawPixel8BitAdd);
}

void SoftwarePainter::DrawLine15BitAdd(int x1, int y1, int x2, int y2, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->GetColorInternal(0);
	lColor.mRed   >>= 3;
	lColor.mGreen >>= 3;
	lColor.mBlue  >>= 3;

	MACRO_CLIPLINE(x1, y1, x2, y2, pThis);
	MACRO_DRAWLINE(x1, y1, x2, y2, pThis, 
		Lepra::uint16, 1, lColor, MACRO_DrawPixel15BitAdd);
}

void SoftwarePainter::DrawLine16BitAdd(int x1, int y1, int x2, int y2, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->GetColorInternal(0);
	lColor.mRed   >>= 3;
	lColor.mGreen >>= 2;
	lColor.mBlue  >>= 3;

	MACRO_CLIPLINE(x1, y1, x2, y2, pThis);
	MACRO_DRAWLINE(x1, y1, x2, y2, pThis, 
		Lepra::uint16, 1, lColor, MACRO_DrawPixel16BitAdd);
}

void SoftwarePainter::DrawLine24BitAdd(int x1, int y1, int x2, int y2, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->GetColorInternal(0);
	MACRO_CLIPLINE(x1, y1, x2, y2, pThis);
	MACRO_DRAWLINE(x1, y1, x2, y2, pThis, 
		Lepra::uint8, 3, lColor, MACRO_DrawPixelAdd);
}

void SoftwarePainter::DrawLine32BitAdd(int x1, int y1, int x2, int y2, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->GetColorInternal(0);
	MACRO_CLIPLINE(x1, y1, x2, y2, pThis);
	if (pThis->mIncrementalAlpha == true)
	{
		Lepra::Color& lColor = pThis->GetColorInternal(0);
		MACRO_DRAWLINE(x1, y1, x2, y2, pThis,
			Lepra::uint8, 4, lColor, MACRO_DrawPixelAdd32Bit);
	}
	else
	{
		MACRO_DRAWLINE(x1, y1, x2, y2, pThis, 
			Lepra::uint8, 4, lColor, MACRO_DrawPixelAdd);
	}
}


#undef MACRO_CLIPLINE
#undef MACRO_DRAWLINE

} // End namespace.