/*
	Lepra::File:   SoftwarePainter_DrawHLine.cpp
	Class:  SoftwarePainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/UiSoftwarePainter.h"
#include "UiSoftwarePainter_DrawPixelMacros.h"

namespace UiTbc
{

#define MACRO_CLIPHLINE(x1, x2, y, painter) \
{ \
	Lepra::PixelRect _lClippingRect; \
	painter->GetScreenCoordClippingRect(_lClippingRect); \
	if (y < _lClippingRect.mTop || y >= _lClippingRect.mBottom) \
	{ \
		return; \
	} \
 \
	if (x1 > x2) \
	{ \
		int lTemp = x1; \
		x1 = x2; \
		x2 = lTemp; \
	} \
 \
	if (x1 >= _lClippingRect.mRight || x2 < _lClippingRect.mLeft) \
	{ \
		return; \
	} \
 \
	if (x1 < _lClippingRect.mLeft) \
	{ \
		x1 = _lClippingRect.mLeft; \
	} \
 \
	if (x2 > _lClippingRect.mRight) \
	{ \
		x2 = _lClippingRect.mRight; \
	} \
}

// A macro with an unoptimized drawing loop.
#define MACRO_DRAWHLINE(x1, x2, y, pThis, type, unitsize, color, drawpixelmacro) \
{ \
	int _lLength = (x2 - x1); \
 \
	Lepra::Canvas* _lCanvas = pThis->GetCanvas(); \
	type* _lScreen = (type*)_lCanvas->GetBuffer(); \
	_lScreen += (pY * _lCanvas->GetPitch() + pX1) * unitsize; \
 \
	for (int i = 0; i < _lLength; i++) \
	{ \
		drawpixelmacro(_lScreen, color); \
		_lScreen += unitsize; \
	} \
}

void SoftwarePainter::DrawHLine8BitNormal(int pX1, int pX2, int pY, int pColorIndex, SoftwarePainter* pThis)
{
	MACRO_CLIPHLINE(pX1, pX2, pY, pThis);

	Lepra::uint8 lColor = pThis->mColor8[pColorIndex];

	int lLength = (pX2 - pX1);
	int lQuadSize = (lLength >> 2);
	Lepra::uint16 lDoubleColor = (((Lepra::uint16)lColor) << 8) | (Lepra::uint16)lColor;
	unsigned lQuadColor = (((unsigned)lDoubleColor) << 16) | (unsigned)lDoubleColor;

	Lepra::Canvas* lCanvas = pThis->GetCanvas();
	Lepra::uint8* lScreen = (Lepra::uint8*)lCanvas->GetBuffer();
	lScreen += pY * lCanvas->GetPitch() + pX1;

	for (int i = 0; i < lQuadSize; i++)
	{
		*((unsigned*)lScreen) = lQuadColor;
		lScreen += 4;
	}

	if ((lLength & 2) != 0)
	{
		*((Lepra::uint16*)lScreen) = lDoubleColor;
		lScreen += 2;
	}

	if ((lLength & 1) != 0)
	{
		*lScreen = lColor;
	}
}

void SoftwarePainter::DrawHLine16BitNormal(int pX1, int pX2, int pY, int pColorIndex, SoftwarePainter* pThis)
{
	MACRO_CLIPHLINE(pX1, pX2, pY, pThis);

	Lepra::uint16 lColor = pThis->mColor16[pColorIndex];

	int lLength = (pX2 - pX1);
	int lDoubleSize = (lLength >> 1);
	unsigned lDoubleColor = (((unsigned)lColor) << 16) | lColor;

	Lepra::Canvas* lCanvas = pThis->GetCanvas();
	Lepra::uint16* lScreen = (Lepra::uint16*)lCanvas->GetBuffer();
	lScreen += pY * lCanvas->GetPitch() + pX1;

	for (int i = 0; i < lDoubleSize; i++)
	{
		*((unsigned*)lScreen) = lDoubleColor;
		lScreen += 2;
	}

	if ((lLength & 1) != 0)
	{
		*lScreen = lColor;
	}
}

void SoftwarePainter::DrawHLine24BitNormal(int pX1, int pX2, int pY, int pColorIndex, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->GetColorInternal(pColorIndex);
	MACRO_CLIPHLINE(pX1, pX2, pY, pThis);
	MACRO_DRAWHLINE(pX1, pX2, pY, pThis,
		Lepra::uint8, 3, lColor, MACRO_DrawPixel24BitNormal);
}

void SoftwarePainter::DrawHLine32BitNormal(int pX1, int pX2, int pY, int pColorIndex, SoftwarePainter* pThis)
{
	MACRO_CLIPHLINE(pX1, pX2, pY, pThis);

	if (pThis->mIncrementalAlpha == true)
	{
		Lepra::Color& lColor = pThis->GetColorInternal(pColorIndex);
		MACRO_DRAWHLINE(pX1, pX2, pY, pThis,
			Lepra::uint8, 4, lColor, MACRO_DrawPixel32BitNormal);
	}
	else
	{
		unsigned lColor = pThis->mColor32[pColorIndex];
		MACRO_DRAWHLINE(pX1, pX2, pY, pThis,
			unsigned, 1, lColor, MACRO_DrawPixelNormal);
	}
}

void SoftwarePainter::DrawHLine15BitBlend(int pX1, int pX2, int pY, int pColorIndex, SoftwarePainter* pThis)
{
	BlendColor lColor(pThis->GetColorInternal(pColorIndex), pThis->GetAlphaValue());
	MACRO_CLIPHLINE(pX1, pX2, pY, pThis);
	MACRO_DRAWHLINE(pX1, pX2, pY, pThis,
		Lepra::uint16, 1, lColor, MACRO_DrawPixel15BitBlend);
}

void SoftwarePainter::DrawHLine16BitBlend(int pX1, int pX2, int pY, int pColorIndex, SoftwarePainter* pThis)
{
	BlendColor lColor(pThis->GetColorInternal(pColorIndex), pThis->GetAlphaValue());
	MACRO_CLIPHLINE(pX1, pX2, pY, pThis);
	MACRO_DRAWHLINE(pX1, pX2, pY, pThis,
		Lepra::uint16, 1, lColor, MACRO_DrawPixel16BitBlend);
}

void SoftwarePainter::DrawHLine24BitBlend(int pX1, int pX2, int pY, int pColorIndex, SoftwarePainter* pThis)
{
	BlendColor lColor(pThis->GetColorInternal(pColorIndex), pThis->GetAlphaValue());
	MACRO_CLIPHLINE(pX1, pX2, pY, pThis);
	MACRO_DRAWHLINE(pX1, pX2, pY, pThis,
		Lepra::uint8, 3, lColor, MACRO_DrawPixelBlend);
}

void SoftwarePainter::DrawHLine32BitBlend(int pX1, int pX2, int pY, int pColorIndex, SoftwarePainter* pThis)
{
	BlendColor lColor(pThis->GetColorInternal(pColorIndex), pThis->GetAlphaValue());
	MACRO_CLIPHLINE(pX1, pX2, pY, pThis);

	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWHLINE(pX1, pX2, pY, pThis,
			Lepra::uint8, 4, lColor, MACRO_DrawPixel32BitBlend);
	}
	else
	{
		MACRO_DRAWHLINE(pX1, pX2, pY, pThis,
			Lepra::uint8, 4, lColor, MACRO_DrawPixelBlend);
	}
}

void SoftwarePainter::DrawHLine8BitXor(int pX1, int pX2, int pY, int pColorIndex, SoftwarePainter* pThis)
{
	MACRO_CLIPHLINE(pX1, pX2, pY, pThis);

	Lepra::uint8 lColor = pThis->mColor8[pColorIndex];

	int lLength = (pX2 - pX1);
	int lQuadSize = (lLength >> 2);
	Lepra::uint16 lDoubleColor = (((Lepra::uint16)lColor) << 8) | (Lepra::uint16)lColor;
	unsigned lQuadColor = (((unsigned)lDoubleColor) << 16) | (unsigned)lDoubleColor;

	Lepra::Canvas* lCanvas = pThis->GetCanvas();
	Lepra::uint8* lScreen = (Lepra::uint8*)lCanvas->GetBuffer();
	lScreen += pY * lCanvas->GetPitch() + pX1;

	for (int i = 0; i < lQuadSize; i++)
	{
		*((unsigned*)lScreen) ^= lQuadColor;
		lScreen += 4;
	}

	if ((lLength & 2) != 0)
	{
		*((Lepra::uint16*)lScreen) ^= lDoubleColor;
		lScreen += 2;
	}

	if ((lLength & 1) != 0)
	{
		*lScreen ^= lColor;
	}
}

void SoftwarePainter::DrawHLine16BitXor(int pX1, int pX2, int pY, int pColorIndex, SoftwarePainter* pThis)
{
	MACRO_CLIPHLINE(pX1, pX2, pY, pThis);

	Lepra::uint16 lColor = pThis->mColor16[pColorIndex];

	int lLength = (pX2 - pX1);
	int lDoubleSize = (lLength >> 1);
	unsigned lDoubleColor = (((unsigned)lColor) << 16) | lColor;

	Lepra::Canvas* lCanvas = pThis->GetCanvas();
	Lepra::uint16* lScreen = (Lepra::uint16*)lCanvas->GetBuffer();
	lScreen += pY * lCanvas->GetPitch() + pX1;

	for (int i = 0; i < lDoubleSize; i++)
	{
		*((unsigned*)lScreen) ^= lDoubleColor;
		lScreen += 2;
	}

	if ((lLength & 1) != 0)
	{
		*lScreen ^= lColor;
	}
}

void SoftwarePainter::DrawHLine24BitXor(int pX1, int pX2, int pY, int pColorIndex, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->GetColorInternal(pColorIndex);
	MACRO_CLIPHLINE(pX1, pX2, pY, pThis);
	MACRO_DRAWHLINE(pX1, pX2, pY, pThis,
		Lepra::uint8, 3, lColor, MACRO_DrawPixel24BitXor);
}

void SoftwarePainter::DrawHLine32BitXor(int pX1, int pX2, int pY, int pColorIndex, SoftwarePainter* pThis)
{
	MACRO_CLIPHLINE(pX1, pX2, pY, pThis);

	if (pThis->mIncrementalAlpha == true)
	{
		Lepra::Color& lColor = pThis->GetColorInternal(pColorIndex);
		MACRO_DRAWHLINE(pX1, pX2, pY, pThis,
			Lepra::uint8, 4, lColor, MACRO_DrawPixel32BitXor);
	}
	else
	{
		unsigned lColor = pThis->mColor32[pColorIndex];
		MACRO_DRAWHLINE(pX1, pX2, pY, pThis,
			unsigned, 1, lColor, MACRO_DrawPixelXor);
	}
}

void SoftwarePainter::DrawHLine8BitAdd(int pX1, int pX2, int pY, int pColorIndex, SoftwarePainter* pThis)
{
	Lepra::uint16 lColor = (Lepra::uint16)pThis->mColor16[pColorIndex];
	MACRO_CLIPHLINE(pX1, pX2, pY, pThis);
	MACRO_DRAWHLINE(pX1, pX2, pY, pThis,
		Lepra::uint8, 1, lColor, MACRO_DrawPixel8BitAdd);
}

void SoftwarePainter::DrawHLine15BitAdd(int pX1, int pX2, int pY, int pColorIndex, SoftwarePainter* pThis)
{
	Lepra::Color lColor(pThis->GetColorInternal(pColorIndex));
	lColor.mRed   >>= 3;
	lColor.mGreen >>= 3;
	lColor.mBlue  >>= 3;

	MACRO_CLIPHLINE(pX1, pX2, pY, pThis);
	MACRO_DRAWHLINE(pX1, pX2, pY, pThis,
		Lepra::uint16, 1, lColor, MACRO_DrawPixel15BitAdd);
}

void SoftwarePainter::DrawHLine16BitAdd(int pX1, int pX2, int pY, int pColorIndex, SoftwarePainter* pThis)
{
	Lepra::Color lColor(pThis->GetColorInternal(pColorIndex));
	lColor.mRed   >>= 3;
	lColor.mGreen >>= 2;
	lColor.mBlue  >>= 3;

	MACRO_CLIPHLINE(pX1, pX2, pY, pThis);
	MACRO_DRAWHLINE(pX1, pX2, pY, pThis,
		Lepra::uint16, 1, lColor, MACRO_DrawPixel16BitAdd);
}

void SoftwarePainter::DrawHLine24BitAdd(int pX1, int pX2, int pY, int pColorIndex, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->GetColorInternal(pColorIndex);
	MACRO_CLIPHLINE(pX1, pX2, pY, pThis);
	MACRO_DRAWHLINE(pX1, pX2, pY, pThis,
		Lepra::uint8, 3, lColor, MACRO_DrawPixelAdd);
}

void SoftwarePainter::DrawHLine32BitAdd(int pX1, int pX2, int pY, int pColorIndex, SoftwarePainter* pThis)
{
	Lepra::Color& lColor = pThis->GetColorInternal(pColorIndex);
	MACRO_CLIPHLINE(pX1, pX2, pY, pThis);

	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWHLINE(pX1, pX2, pY, pThis,
			Lepra::uint8, 4, lColor, MACRO_DrawPixelAdd32Bit);
	}
	else
	{
		MACRO_DRAWHLINE(pX1, pX2, pY, pThis,
			Lepra::uint8, 4, lColor, MACRO_DrawPixelAdd);
	}
}

#undef MACRO_CLIPHLINE
#undef MACRO_DRAWHLINE

} // End namespace.