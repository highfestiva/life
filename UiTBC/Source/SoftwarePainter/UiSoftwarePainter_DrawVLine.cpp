/*
	Lepra::File:   SoftwarePainter_DrawVLine.cpp
	Class:  SoftwarePainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/UiSoftwarePainter.h"
#include "UiSoftwarePainter_DrawPixelMacros.h"

namespace UiTbc
{

#define MACRO_CLIPVLINE(y1, y2, x, painter) \
{ \
	Lepra::PixelRect _lClippingRect; \
	painter->GetScreenCoordClippingRect(_lClippingRect); \
	if (x < _lClippingRect.mLeft || x >= _lClippingRect.mRight) \
	{ \
		return; \
	} \
 \
	if (y1 > y2) \
	{ \
		int lTemp = y1; \
		y1 = y2; \
		y2 = lTemp; \
	} \
 \
	if (y1 >= _lClippingRect.mBottom || y2 < _lClippingRect.mTop) \
	{ \
		return; \
	} \
 \
	if (y1 < _lClippingRect.mTop) \
	{ \
		y1 = _lClippingRect.mTop; \
	} \
 \
	if (y2 >= _lClippingRect.mBottom) \
	{ \
		y2 = _lClippingRect.mBottom - 1; \
	} \
}

#define MACRO_DRAWVLINE(y1, y2, x, pThis, type, unitsize, color, drawpixelmacro) \
{ \
	int _lLength = (y2 - y1); \
 \
	Lepra::Canvas* _lCanvas = pThis->GetCanvas(); \
	type* _lScreen = (type*)_lCanvas->GetBuffer(); \
	_lScreen += (y1 * _lCanvas->GetPitch() + x) * unitsize; \
 \
	int _lPitchAdd = _lCanvas->GetPitch() * unitsize;\
 \
	for (int i = 0; i < _lLength; i++) \
	{ \
		drawpixelmacro(_lScreen, color); \
		_lScreen += _lPitchAdd; \
	} \
}

void SoftwarePainter::DrawVLine8BitNormal(int pY1, int pY2, int pX, int pColorIndex, SoftwarePainter* pThis)
{
	Lepra::uint8 lColor = pThis->mColor8[pColorIndex];
	MACRO_CLIPVLINE(pY1, pY2, pX, pThis);
	MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
		Lepra::uint8, 1, lColor, MACRO_DrawPixelNormal);
}

void SoftwarePainter::DrawVLine16BitNormal(int pY1, int pY2, int pX, int pColorIndex, SoftwarePainter* pThis)
{
	Lepra::uint16 lColor = pThis->mColor16[pColorIndex];
	MACRO_CLIPVLINE(pY1, pY2, pX, pThis);
	MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
		Lepra::uint16, 1, lColor, MACRO_DrawPixelNormal);
}

void SoftwarePainter::DrawVLine24BitNormal(int pY1, int pY2, int pX, int pColorIndex, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->GetColorInternal(pColorIndex);
	MACRO_CLIPVLINE(pY1, pY2, pX, pThis);
	MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
		Lepra::uint8, 3, lColor, MACRO_DrawPixel24BitNormal);
}

void SoftwarePainter::DrawVLine32BitNormal(int pY1, int pY2, int pX, int pColorIndex, SoftwarePainter* pThis)
{
	MACRO_CLIPVLINE(pY1, pY2, pX, pThis);

	if (pThis->mIncrementalAlpha == true)
	{
		Lepra::Color& lColor = pThis->GetColorInternal(pColorIndex);
		MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
			Lepra::uint8, 4, lColor, MACRO_DrawPixel32BitNormal);
	}
	else
	{
		unsigned lColor = pThis->mColor32[pColorIndex];
		MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
			unsigned, 1, lColor, MACRO_DrawPixelNormal);
	}
}

void SoftwarePainter::DrawVLine15BitBlend(int pY1, int pY2, int pX, int pColorIndex, SoftwarePainter* pThis)
{
	BlendColor lColor(pThis->GetColorInternal(pColorIndex), pThis->GetAlphaValue());
	MACRO_CLIPVLINE(pY1, pY2, pX, pThis);
	MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
		Lepra::uint16, 1, lColor, MACRO_DrawPixel15BitBlend);
}

void SoftwarePainter::DrawVLine16BitBlend(int pY1, int pY2, int pX, int pColorIndex, SoftwarePainter* pThis)
{
	BlendColor lColor(pThis->GetColorInternal(pColorIndex), pThis->GetAlphaValue());
	MACRO_CLIPVLINE(pY1, pY2, pX, pThis);
	MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
		Lepra::uint16, 1, lColor, MACRO_DrawPixel16BitBlend);
}

void SoftwarePainter::DrawVLine24BitBlend(int pY1, int pY2, int pX, int pColorIndex, SoftwarePainter* pThis)
{
	BlendColor lColor(pThis->GetColorInternal(pColorIndex), pThis->GetAlphaValue());
	MACRO_CLIPVLINE(pY1, pY2, pX, pThis);
	MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
		Lepra::uint8, 3, lColor, MACRO_DrawPixelBlend);
}

void SoftwarePainter::DrawVLine32BitBlend(int pY1, int pY2, int pX, int pColorIndex, SoftwarePainter* pThis)
{
	BlendColor lColor(pThis->GetColorInternal(pColorIndex), pThis->GetAlphaValue());
	MACRO_CLIPVLINE(pY1, pY2, pX, pThis);

	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
			Lepra::uint8, 4, lColor, MACRO_DrawPixel32BitBlend);
	}
	else
	{
		MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
			Lepra::uint8, 4, lColor, MACRO_DrawPixelBlend);
	}
}

void SoftwarePainter::DrawVLine8BitXor(int pY1, int pY2, int pX, int pColorIndex, SoftwarePainter* pThis)
{
	Lepra::uint8 lColor = pThis->mColor8[pColorIndex];
	MACRO_CLIPVLINE(pY1, pY2, pX, pThis);
	MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
		Lepra::uint8, 1, lColor, MACRO_DrawPixelXor);
}

void SoftwarePainter::DrawVLine16BitXor(int pY1, int pY2, int pX, int pColorIndex, SoftwarePainter* pThis)
{
	Lepra::uint16 lColor = pThis->mColor16[pColorIndex];
	MACRO_CLIPVLINE(pY1, pY2, pX, pThis);
	MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
		Lepra::uint16, 1, lColor, MACRO_DrawPixelXor);
}

void SoftwarePainter::DrawVLine24BitXor(int pY1, int pY2, int pX, int pColorIndex, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->GetColorInternal(pColorIndex);
	MACRO_CLIPVLINE(pY1, pY2, pX, pThis);
	MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
		Lepra::uint8, 3, lColor, MACRO_DrawPixel24BitXor);
}

void SoftwarePainter::DrawVLine32BitXor(int pY1, int pY2, int pX, int pColorIndex, SoftwarePainter* pThis)
{
	MACRO_CLIPVLINE(pY1, pY2, pX, pThis);
	if (pThis->mIncrementalAlpha == true)
	{
		Lepra::Color& lColor = pThis->GetColorInternal(pColorIndex);
		MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
			Lepra::uint8, 4, lColor, MACRO_DrawPixel32BitXor);
	}
	else
	{
		unsigned lColor = pThis->mColor32[pColorIndex];
		MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
			unsigned, 1, lColor, MACRO_DrawPixelXor);
	}
}

void SoftwarePainter::DrawVLine8BitAdd(int pY1, int pY2, int pX, int pColorIndex, SoftwarePainter* pThis)
{
	Lepra::uint8 lColor = (Lepra::uint8)pThis->mColor8[pColorIndex];
	MACRO_CLIPVLINE(pY1, pY2, pX, pThis);
	MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
		Lepra::uint8, 1, lColor, MACRO_DrawPixel8BitAdd);
}

void SoftwarePainter::DrawVLine15BitAdd(int pY1, int pY2, int pX, int pColorIndex, SoftwarePainter* pThis)
{
	Lepra::Color lColor(pThis->GetColorInternal(pColorIndex));
	lColor.mRed   >>= 3;
	lColor.mGreen >>= 3;
	lColor.mBlue  >>= 3;

	MACRO_CLIPVLINE(pY1, pY2, pX, pThis);
	MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
		Lepra::uint16, 1, lColor, MACRO_DrawPixel15BitAdd);
}

void SoftwarePainter::DrawVLine16BitAdd(int pY1, int pY2, int pX, int pColorIndex, SoftwarePainter* pThis)
{
	Lepra::Color lColor(pThis->GetColorInternal(pColorIndex));
	lColor.mRed   >>= 3;
	lColor.mGreen >>= 2;
	lColor.mBlue  >>= 3;

	MACRO_CLIPVLINE(pY1, pY2, pX, pThis);
	MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
		Lepra::uint16, 1, lColor, MACRO_DrawPixel16BitAdd);
}

void SoftwarePainter::DrawVLine24BitAdd(int pY1, int pY2, int pX, int pColorIndex, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->GetColorInternal(pColorIndex);
	MACRO_CLIPVLINE(pY1, pY2, pX, pThis);
	MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
		Lepra::uint8, 3, lColor, MACRO_DrawPixelAdd);
}

void SoftwarePainter::DrawVLine32BitAdd(int pY1, int pY2, int pX, int pColorIndex, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->GetColorInternal(pColorIndex);
	MACRO_CLIPVLINE(pY1, pY2, pX, pThis);

	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
			Lepra::uint8, 4, lColor, MACRO_DrawPixelAdd32Bit);
	}
	else
	{
		MACRO_DRAWVLINE(pY1, pY2, pX, pThis,
			Lepra::uint8, 4, lColor, MACRO_DrawPixelAdd);
	}
}

#undef MACRO_CLIPVLINE
#undef MACRO_DRAWVLINE

} // End namespace.
