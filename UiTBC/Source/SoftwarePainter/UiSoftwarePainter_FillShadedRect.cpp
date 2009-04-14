/*
	Class:  SoftwarePainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/UiSoftwarePainter.h"
#include "UiSoftwarePainter_DrawPixelMacros.h"

namespace UiTbc
{

#define MACRO_FILLSHADEDRECT(left, top, right, bottom, pThis, type, unitsize, drawpixelmacro) \
{ \
	Lepra::PixelRect lClippingRect; \
	pThis->GetScreenCoordClippingRect(lClippingRect); \
	Lepra::Canvas* lCanvas = pThis->GetCanvas(); \
	Lepra::Color* lColor = &pThis->GetColorInternal(0); \
 \
	if ( \
		top    >  lClippingRect.mBottom || \
		bottom <= lClippingRect.mTop    || \
		left   >  lClippingRect.mRight  || \
		right  <= lClippingRect.mLeft   || \
		lClippingRect.mBottom <= lClippingRect.mTop  || \
		lClippingRect.mRight  <= lClippingRect.mLeft || \
		bottom <= top || \
		right  <= left \
	  ) \
	{ \
		return; \
	} \
 \
	int lTopLeftRed   = (((int)lColor[0].mRed)   << 23); \
	int lTopLeftGreen = (((int)lColor[0].mGreen) << 23); \
	int lTopLeftBlue  = (((int)lColor[0].mBlue)  << 23); \
 \
	int lTopRightRed   = (((int)lColor[1].mRed)   << 23); \
	int lTopRightGreen = (((int)lColor[1].mGreen) << 23); \
	int lTopRightBlue  = (((int)lColor[1].mBlue)  << 23); \
 \
	int lBottomLeftRed   = (((int)lColor[3].mRed)   << 23); \
	int lBottomLeftGreen = (((int)lColor[3].mGreen) << 23); \
	int lBottomLeftBlue  = (((int)lColor[3].mBlue)  << 23); \
 \
	int lBottomRightRed   = (((int)lColor[2].mRed)   << 23); \
	int lBottomRightGreen = (((int)lColor[2].mGreen) << 23); \
	int lBottomRightBlue  = (((int)lColor[2].mBlue)  << 23); \
 \
	int lLeftRedAdd   = (lBottomLeftRed   - lTopLeftRed  ) / (int)(bottom - top); \
	int lLeftGreenAdd = (lBottomLeftGreen - lTopLeftGreen) / (int)(bottom - top); \
	int lLeftBlueAdd  = (lBottomLeftBlue  - lTopLeftBlue ) / (int)(bottom - top); \
 \
	int lRightRedAdd   = (lBottomRightRed   - lTopRightRed  ) / (int)(bottom - top); \
	int lRightGreenAdd = (lBottomRightGreen - lTopRightGreen) / (int)(bottom - top); \
	int lRightBlueAdd  = (lBottomRightBlue  - lTopRightBlue ) / (int)(bottom - top); \
 \
	if (top < lClippingRect.mTop) \
	{ \
		lTopLeftRed   += lLeftRedAdd   * (lClippingRect.mTop - top); \
		lTopLeftGreen += lLeftGreenAdd * (lClippingRect.mTop - top); \
		lTopLeftBlue  += lLeftBlueAdd  * (lClippingRect.mTop - top); \
 \
		lTopRightRed   += lRightRedAdd   * (lClippingRect.mTop - top); \
		lTopRightGreen += lRightGreenAdd * (lClippingRect.mTop - top); \
		lTopRightBlue  += lRightBlueAdd  * (lClippingRect.mTop - top); \
 \
		top = lClippingRect.mTop; \
	} \
	if (bottom > lClippingRect.mBottom) \
	{ \
		lBottomLeftRed   -= lLeftRedAdd   * (bottom - lClippingRect.mBottom); \
		lBottomLeftGreen -= lLeftGreenAdd * (bottom - lClippingRect.mBottom); \
		lBottomLeftBlue  -= lLeftBlueAdd  * (bottom - lClippingRect.mBottom); \
 \
		lBottomRightRed   -= lRightRedAdd   * (bottom - lClippingRect.mBottom); \
		lBottomRightGreen -= lRightGreenAdd * (bottom - lClippingRect.mBottom); \
		lBottomRightBlue  -= lRightBlueAdd  * (bottom - lClippingRect.mBottom); \
 \
		bottom = lClippingRect.mBottom; \
	} \
 \
	if (bottom <= top) \
	{ \
		return; \
	} \
 \
	if (left < lClippingRect.mLeft || right > lClippingRect.mRight) \
	{ \
		int lTopRedAdd   = (lTopRightRed   - lTopLeftRed  ) / (int)(right - left); \
		int lTopGreenAdd = (lTopRightGreen - lTopLeftGreen) / (int)(right - left); \
		int lTopBlueAdd  = (lTopRightBlue  - lTopLeftBlue ) / (int)(right - left); \
 \
		int lBottomRedAdd   = (lBottomRightRed   - lBottomLeftRed  ) / (int)(right - left); \
		int lBottomGreenAdd = (lBottomRightGreen - lBottomLeftGreen) / (int)(right - left); \
		int lBottomBlueAdd  = (lBottomRightBlue  - lBottomLeftBlue ) / (int)(right - left); \
 \
		if (left < lClippingRect.mLeft) \
		{ \
			lTopLeftRed   += lTopRedAdd   * (lClippingRect.mLeft - left); \
			lTopLeftGreen += lTopGreenAdd * (lClippingRect.mLeft - left); \
			lTopLeftBlue  += lTopBlueAdd  * (lClippingRect.mLeft - left); \
 \
			lBottomLeftRed   += lBottomRedAdd   * (lClippingRect.mLeft - left); \
			lBottomLeftGreen += lBottomGreenAdd * (lClippingRect.mLeft - left); \
			lBottomLeftBlue  += lBottomBlueAdd  * (lClippingRect.mLeft - left); \
 \
			left = lClippingRect.mLeft; \
		} \
 \
		if (right > lClippingRect.mRight) \
		{ \
			lTopRightRed   -= lTopRedAdd   * (right - lClippingRect.mRight); \
			lTopRightGreen -= lTopGreenAdd * (right - lClippingRect.mRight); \
			lTopRightBlue  -= lTopBlueAdd  * (right - lClippingRect.mRight); \
 \
			lBottomRightRed   -= lBottomRedAdd   * (right - lClippingRect.mRight); \
			lBottomRightGreen -= lBottomGreenAdd * (right - lClippingRect.mRight); \
			lBottomRightBlue  -= lBottomBlueAdd  * (right - lClippingRect.mRight); \
 \
			right = lClippingRect.mRight; \
		} \
	} \
 \
	if (right <= left) \
	{ \
		return; \
	} \
 \
	/* \
	   0 = Top Left \
	   1 = Top Right \
	   2 = Bottom Right \
	   3 = Bottom Left \
	 */ \
 \
	int lLeftRed   = lTopLeftRed; \
	int lLeftGreen = lTopLeftGreen; \
	int lLeftBlue  = lTopLeftBlue; \
 \
	int lRightRed   = lTopRightRed; \
	int lRightGreen = lTopRightGreen; \
	int lRightBlue  = lTopRightBlue; \
 \
	int lHeight = (int)(bottom - top); \
 \
	lLeftRedAdd   = (lBottomLeftRed   - lLeftRed  ) / lHeight; \
	lLeftGreenAdd = (lBottomLeftGreen - lLeftGreen) / lHeight; \
	lLeftBlueAdd  = (lBottomLeftBlue  - lLeftBlue ) / lHeight; \
 \
	lRightRedAdd   = (lBottomRightRed   - lRightRed  ) / lHeight; \
	lRightGreenAdd = (lBottomRightGreen - lRightGreen) / lHeight; \
	lRightBlueAdd  = (lBottomRightBlue  - lRightBlue ) / lHeight; \
 \
	type* lScreen = (type*)lCanvas->GetBuffer(); \
	lScreen += (top * lCanvas->GetPitch() + left) * unitsize; \
 \
	int lPitchAdd = lCanvas->GetPitch() * unitsize; \
	Lepra::uint16 lAlphaValue = (Lepra::uint16)pThis->GetAlphaValue(); \
 \
	for (int y = top; y < bottom; y++) \
	{ \
		int lRed   = lLeftRed; \
		int lGreen = lLeftGreen; \
		int lBlue  = lLeftBlue; \
 \
		int lWidth = (int)(right - left); \
		int lRedAdd   = (lRightRed   - lLeftRed)   / lWidth; \
		int lGreenAdd = (lRightGreen - lLeftGreen) / lWidth; \
		int lBlueAdd  = (lRightBlue  - lLeftBlue)  / lWidth; \
 \
		type* lDest = lScreen; \
 \
		for (int x = left; x < right; x++) \
		{ \
			drawpixelmacro(lDest, (lRed >> 23), (lGreen >> 23), (lBlue >> 23), lAlphaValue); \
 \
			lRed   += lRedAdd; \
			lGreen += lGreenAdd; \
			lBlue  += lBlueAdd; \
 \
			lDest += unitsize; \
		} \
 \
		lScreen += lPitchAdd; \
 \
		lLeftRed   += lLeftRedAdd; \
		lLeftGreen += lLeftGreenAdd; \
		lLeftBlue  += lLeftBlueAdd; \
 \
		lRightRed   += lRightRedAdd; \
		lRightGreen += lRightGreenAdd; \
		lRightBlue  += lRightBlueAdd; \
	} \
}

#define MACRO_FILLSHADEDRECT32(left, top, right, bottom, pThis, drawpixelmacro) \
{ \
	Lepra::PixelRect lClippingRect; \
	pThis->GetScreenCoordClippingRect(lClippingRect); \
	Lepra::Canvas* lCanvas = pThis->GetCanvas(); \
	Lepra::Color* lColor = &pThis->GetColorInternal(0); \
 \
	if ( \
		top    >  lClippingRect.mBottom || \
		bottom <= lClippingRect.mTop    || \
		left   >  lClippingRect.mRight  || \
		right  <= lClippingRect.mLeft   || \
		lClippingRect.mBottom <= lClippingRect.mTop  || \
		lClippingRect.mRight  <= lClippingRect.mLeft || \
		bottom <= top || \
		right  <= left \
	  ) \
	{ \
		return; \
	} \
 \
	int lTopLeftRed   = (((int)lColor[0].mRed)   << 23); \
	int lTopLeftGreen = (((int)lColor[0].mGreen) << 23); \
	int lTopLeftBlue  = (((int)lColor[0].mBlue)  << 23); \
	int lTopLeftAlpha = (((int)lColor[0].mAlpha) << 23); \
 \
	int lTopRightRed   = (((int)lColor[1].mRed)   << 23); \
	int lTopRightGreen = (((int)lColor[1].mGreen) << 23); \
	int lTopRightBlue  = (((int)lColor[1].mBlue)  << 23); \
	int lTopRightAlpha = (((int)lColor[1].mAlpha) << 23); \
 \
	int lBottomLeftRed   = (((int)lColor[3].mRed)   << 23); \
	int lBottomLeftGreen = (((int)lColor[3].mGreen) << 23); \
	int lBottomLeftBlue  = (((int)lColor[3].mBlue)  << 23); \
	int lBottomLeftAlpha = (((int)lColor[3].mAlpha) << 23); \
 \
	int lBottomRightRed   = (((int)lColor[2].mRed)   << 23); \
	int lBottomRightGreen = (((int)lColor[2].mGreen) << 23); \
	int lBottomRightBlue  = (((int)lColor[2].mBlue)  << 23); \
	int lBottomRightAlpha = (((int)lColor[2].mAlpha) << 23); \
 \
	int lLeftRedAdd   = (lBottomLeftRed   - lTopLeftRed  ) / (int)(bottom - top); \
	int lLeftGreenAdd = (lBottomLeftGreen - lTopLeftGreen) / (int)(bottom - top); \
	int lLeftBlueAdd  = (lBottomLeftBlue  - lTopLeftBlue ) / (int)(bottom - top); \
	int lLeftAlphaAdd = (lBottomLeftAlpha - lTopLeftAlpha) / (int)(bottom - top); \
 \
	int lRightRedAdd   = (lBottomRightRed   - lTopRightRed  ) / (int)(bottom - top); \
	int lRightGreenAdd = (lBottomRightGreen - lTopRightGreen) / (int)(bottom - top); \
	int lRightBlueAdd  = (lBottomRightBlue  - lTopRightBlue ) / (int)(bottom - top); \
	int lRightAlphaAdd = (lBottomRightAlpha - lTopRightAlpha) / (int)(bottom - top); \
 \
	if (top < lClippingRect.mTop) \
	{ \
		lTopLeftRed   += lLeftRedAdd   * (lClippingRect.mTop - top); \
		lTopLeftGreen += lLeftGreenAdd * (lClippingRect.mTop - top); \
		lTopLeftBlue  += lLeftBlueAdd  * (lClippingRect.mTop - top); \
		lTopLeftAlpha += lLeftAlphaAdd * (lClippingRect.mTop - top); \
 \
		lTopRightRed   += lRightRedAdd   * (lClippingRect.mTop - top); \
		lTopRightGreen += lRightGreenAdd * (lClippingRect.mTop - top); \
		lTopRightBlue  += lRightBlueAdd  * (lClippingRect.mTop - top); \
		lTopRightAlpha += lRightAlphaAdd * (lClippingRect.mTop - top); \
 \
		top = lClippingRect.mTop; \
	} \
	if (bottom > lClippingRect.mBottom) \
	{ \
		lBottomLeftRed   -= lLeftRedAdd   * (bottom - lClippingRect.mBottom); \
		lBottomLeftGreen -= lLeftGreenAdd * (bottom - lClippingRect.mBottom); \
		lBottomLeftBlue  -= lLeftBlueAdd  * (bottom - lClippingRect.mBottom); \
		lBottomLeftAlpha -= lLeftAlphaAdd * (bottom - lClippingRect.mBottom); \
 \
		lBottomRightRed   -= lRightRedAdd   * (bottom - lClippingRect.mBottom); \
		lBottomRightGreen -= lRightGreenAdd * (bottom - lClippingRect.mBottom); \
		lBottomRightBlue  -= lRightBlueAdd  * (bottom - lClippingRect.mBottom); \
		lBottomRightAlpha -= lRightAlphaAdd * (bottom - lClippingRect.mBottom); \
 \
		bottom = lClippingRect.mBottom; \
	} \
 \
	if (bottom <= top) \
	{ \
		return; \
	} \
 \
	if (left < lClippingRect.mLeft || right > lClippingRect.mRight) \
	{ \
		int lTopRedAdd   = (lTopRightRed   - lTopLeftRed  ) / (int)(right - left); \
		int lTopGreenAdd = (lTopRightGreen - lTopLeftGreen) / (int)(right - left); \
		int lTopBlueAdd  = (lTopRightBlue  - lTopLeftBlue ) / (int)(right - left); \
		int lTopAlphaAdd = (lTopRightAlpha - lTopLeftAlpha) / (int)(right - left); \
 \
		int lBottomRedAdd   = (lBottomRightRed   - lBottomLeftRed  ) / (int)(right - left); \
		int lBottomGreenAdd = (lBottomRightGreen - lBottomLeftGreen) / (int)(right - left); \
		int lBottomBlueAdd  = (lBottomRightBlue  - lBottomLeftBlue ) / (int)(right - left); \
		int lBottomAlphaAdd = (lBottomRightAlpha - lBottomLeftAlpha) / (int)(right - left); \
 \
		if (left < lClippingRect.mLeft) \
		{ \
			lTopLeftRed   += lTopRedAdd   * (lClippingRect.mLeft - left); \
			lTopLeftGreen += lTopGreenAdd * (lClippingRect.mLeft - left); \
			lTopLeftBlue  += lTopBlueAdd  * (lClippingRect.mLeft - left); \
			lTopLeftAlpha += lTopAlphaAdd * (lClippingRect.mLeft - left); \
 \
			lBottomLeftRed   += lBottomRedAdd   * (lClippingRect.mLeft - left); \
			lBottomLeftGreen += lBottomGreenAdd * (lClippingRect.mLeft - left); \
			lBottomLeftBlue  += lBottomBlueAdd  * (lClippingRect.mLeft - left); \
			lBottomLeftAlpha += lBottomAlphaAdd * (lClippingRect.mLeft - left); \
 \
			left = lClippingRect.mLeft; \
		} \
 \
		if (right > lClippingRect.mRight) \
		{ \
			lTopRightRed   -= lTopRedAdd   * (right - lClippingRect.mRight); \
			lTopRightGreen -= lTopGreenAdd * (right - lClippingRect.mRight); \
			lTopRightBlue  -= lTopBlueAdd  * (right - lClippingRect.mRight); \
			lTopRightAlpha -= lTopAlphaAdd * (right - lClippingRect.mRight); \
 \
			lBottomRightRed   -= lBottomRedAdd   * (right - lClippingRect.mRight); \
			lBottomRightGreen -= lBottomGreenAdd * (right - lClippingRect.mRight); \
			lBottomRightBlue  -= lBottomBlueAdd  * (right - lClippingRect.mRight); \
			lBottomRightAlpha -= lBottomAlphaAdd * (right - lClippingRect.mRight); \
 \
			right = lClippingRect.mRight; \
		} \
	} \
 \
	if (right <= left) \
	{ \
		return; \
	} \
 \
	/* \
	   0 = Top Left \
	   1 = Top Right \
	   2 = Bottom Right \
	   3 = Bottom Left \
	 */ \
 \
	int lLeftRed   = lTopLeftRed; \
	int lLeftGreen = lTopLeftGreen; \
	int lLeftBlue  = lTopLeftBlue; \
	int lLeftAlpha = lTopLeftAlpha; \
 \
	int lRightRed   = lTopRightRed; \
	int lRightGreen = lTopRightGreen; \
	int lRightBlue  = lTopRightBlue; \
	int lRightAlpha = lTopRightAlpha; \
 \
	int lHeight = (int)(bottom - top); \
 \
	lLeftRedAdd   = (lBottomLeftRed   - lLeftRed  ) / lHeight; \
	lLeftGreenAdd = (lBottomLeftGreen - lLeftGreen) / lHeight; \
	lLeftBlueAdd  = (lBottomLeftBlue  - lLeftBlue ) / lHeight; \
	lLeftAlphaAdd = (lBottomLeftAlpha - lLeftAlpha) / lHeight; \
 \
	lRightRedAdd   = (lBottomRightRed   - lRightRed  ) / lHeight; \
	lRightGreenAdd = (lBottomRightGreen - lRightGreen) / lHeight; \
	lRightBlueAdd  = (lBottomRightBlue  - lRightBlue ) / lHeight; \
	lRightAlphaAdd = (lBottomRightAlpha - lRightAlpha) / lHeight; \
 \
	Lepra::uint8* lScreen = (Lepra::uint8*)lCanvas->GetBuffer(); \
	lScreen += (top * lCanvas->GetPitch() + left) * 4; \
 \
	int lPitchAdd = lCanvas->GetPitch() * 4; \
	Lepra::uint16 lAlphaValue = (Lepra::uint16)pThis->GetAlphaValue(); \
 \
	for (int y = top; y < bottom; y++) \
	{ \
		int lRed   = lLeftRed; \
		int lGreen = lLeftGreen; \
		int lBlue  = lLeftBlue; \
		int lAlpha = lLeftAlpha; \
 \
		int lWidth = (int)(right - left); \
		int lRedAdd   = (lRightRed   - lLeftRed)   / lWidth; \
		int lGreenAdd = (lRightGreen - lLeftGreen) / lWidth; \
		int lBlueAdd  = (lRightBlue  - lLeftBlue)  / lWidth; \
		int lAlphaAdd = (lRightAlpha - lLeftAlpha) / lWidth; \
 \
		Lepra::uint8* lDest = lScreen; \
 \
		for (int x = left; x < right; x++) \
		{ \
			drawpixelmacro(lDest, (lRed >> 23), (lGreen >> 23), (lBlue >> 23), (lAlpha >> 23), lAlphaValue); \
 \
			lRed   += lRedAdd; \
			lGreen += lGreenAdd; \
			lBlue  += lBlueAdd; \
			lAlpha += lAlphaAdd; \
 \
			lDest += 4; \
		} \
 \
		lScreen += lPitchAdd; \
 \
		lLeftRed   += lLeftRedAdd; \
		lLeftGreen += lLeftGreenAdd; \
		lLeftBlue  += lLeftBlueAdd; \
		lLeftAlpha += lLeftAlphaAdd; \
 \
		lRightRed   += lRightRedAdd; \
		lRightGreen += lRightGreenAdd; \
		lRightBlue  += lRightBlueAdd; \
		lRightAlpha += lRightAlphaAdd; \
	} \
}

void SoftwarePainter::FillShadedRect8BitNormal(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis)
{
	// Can't shade in 8 bit mode.
	pThis->FillRect(pLeft, pTop, pRight, pBottom);
}

void SoftwarePainter::FillShadedRect15BitNormal(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis)
{
	MACRO_FILLSHADEDRECT(pLeft, pTop, pRight, pBottom, pThis, 
		Lepra::uint16, 1, MACRO_DrawPixel15BitFromRGB);
}

void SoftwarePainter::FillShadedRect16BitNormal(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis)
{
	MACRO_FILLSHADEDRECT(pLeft, pTop, pRight, pBottom, pThis, 
		Lepra::uint16, 1, MACRO_DrawPixel16BitFromRGB);
}

void SoftwarePainter::FillShadedRect24BitNormal(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis)
{
	MACRO_FILLSHADEDRECT(pLeft, pTop, pRight, pBottom, pThis, 
		Lepra::uint8, 3, MACRO_DrawPixel24BitFromRGB);
}

void SoftwarePainter::FillShadedRect32BitNormal(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis)
{
	MACRO_FILLSHADEDRECT32(pLeft, pTop, pRight, pBottom, pThis, MACRO_DrawPixel32BitFromRGBA);
}

void SoftwarePainter::FillShadedRect15BitBlend(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis)
{
	MACRO_FILLSHADEDRECT(pLeft, pTop, pRight, pBottom, pThis, 
		Lepra::uint16, 1, MACRO_BlendPixel15BitFromRGB);
}

void SoftwarePainter::FillShadedRect16BitBlend(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis)
{
	MACRO_FILLSHADEDRECT(pLeft, pTop, pRight, pBottom, pThis, 
		Lepra::uint16, 1, MACRO_BlendPixel16BitFromRGB);
}

void SoftwarePainter::FillShadedRect24BitBlend(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis)
{
	MACRO_FILLSHADEDRECT(pLeft, pTop, pRight, pBottom, pThis, 
		Lepra::uint8, 3, MACRO_BlendPixel24BitFromRGB);
}

void SoftwarePainter::FillShadedRect32BitBlend(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis)
{
	MACRO_FILLSHADEDRECT32(pLeft, pTop, pRight, pBottom, pThis, MACRO_BlendPixel32BitFromRGBA);
}

void SoftwarePainter::FillShadedRect15BitXor(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis)
{
	MACRO_FILLSHADEDRECT(pLeft, pTop, pRight, pBottom, pThis, 
		Lepra::uint16, 1, MACRO_XorPixel15BitFromRGB);
}

void SoftwarePainter::FillShadedRect16BitXor(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis)
{
	MACRO_FILLSHADEDRECT(pLeft, pTop, pRight, pBottom, pThis, 
		Lepra::uint16, 1, MACRO_XorPixel16BitFromRGB);
}

void SoftwarePainter::FillShadedRect24BitXor(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis)
{
	MACRO_FILLSHADEDRECT(pLeft, pTop, pRight, pBottom, pThis, 
		Lepra::uint8, 3, MACRO_XorPixel24BitFromRGB);
}

void SoftwarePainter::FillShadedRect32BitXor(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis)
{
	MACRO_FILLSHADEDRECT32(pLeft, pTop, pRight, pBottom, pThis, MACRO_XorPixel32BitFromRGBA);
}

void SoftwarePainter::FillShadedRect15BitAdd(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis)
{
	MACRO_FILLSHADEDRECT(pLeft, pTop, pRight, pBottom, pThis, 
		Lepra::uint16, 1, MACRO_AddPixel15BitFromRGB);
}

void SoftwarePainter::FillShadedRect16BitAdd(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis)
{
	MACRO_FILLSHADEDRECT(pLeft, pTop, pRight, pBottom, pThis, 
		Lepra::uint16, 1, MACRO_AddPixel16BitFromRGB);
}

void SoftwarePainter::FillShadedRect24BitAdd(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis)
{
	MACRO_FILLSHADEDRECT(pLeft, pTop, pRight, pBottom, pThis, 
		Lepra::uint8, 3, MACRO_AddPixel24BitFromRGB);
}

void SoftwarePainter::FillShadedRect32BitAdd(int pLeft, int pTop, int pRight, int pBottom, SoftwarePainter* pThis)
{
	MACRO_FILLSHADEDRECT32(pLeft, pTop, pRight, pBottom, pThis, MACRO_AddPixel32BitFromRGBA);
}

} // End namespace.
