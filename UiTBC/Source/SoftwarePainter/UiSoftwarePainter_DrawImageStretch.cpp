/*
	Lepra::File:   SoftwarePainter_DrawImageStretch.cpp
	Class:  SoftwarePainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/UiSoftwarePainter.h"
#include "UiSoftwarePainter_DrawPixelMacros.h"

namespace UiTbc
{

#define MACRO_STRETCHBITMAP(bitmap, rect, subrect, pThis, type, unitsize, copypixelmacro) \
{ \
	Lepra::PixelRect lClippingRect; \
	pThis->GetScreenCoordClippingRect(lClippingRect); \
	Lepra::Canvas* lCanvas = pThis->GetCanvas(); \
 \
	Lepra::PixelRect lRect(rect); \
 \
	int lXAdd = (subrect.GetWidth() << 16) / lRect.GetWidth(); \
	int lYAdd = (subrect.GetHeight() << 16) / lRect.GetHeight(); \
 \
	int lXClipOffset = (subrect.mLeft << 16); \
	int lYClipOffset = (subrect.mTop << 16); \
 \
	if (lRect.mLeft < lClippingRect.mLeft) \
	{ \
		lXClipOffset += lXAdd * (lClippingRect.mLeft - lRect.mLeft); \
		lRect.mLeft = lClippingRect.mLeft; \
	} \
 \
	if (lRect.mTop < lClippingRect.mTop) \
	{ \
		lYClipOffset += lYAdd * (lClippingRect.mTop - lRect.mTop); \
		lRect.mTop = lClippingRect.mTop; \
	} \
 \
	int lRenderWidth = lRect.GetWidth(); \
	int lRenderHeight = lRect.GetHeight(); \
 \
	if (lRenderWidth <= 0 || lRenderHeight <= 0) \
	{ \
		return; \
	} \
 \
	if ((lRect.mLeft + lRenderWidth) > lClippingRect.mRight) \
	{ \
		lRenderWidth = lClippingRect.mRight - lRect.mLeft; \
	} \
 \
	if ((lRect.mTop + lRenderHeight) > lClippingRect.mBottom) \
	{ \
		lRenderHeight = lClippingRect.mBottom - lRect.mTop; \
	} \
 \
	type* lScreen = (type*)lCanvas->GetBuffer() + \
		(lRect.mTop * lCanvas->GetPitch() + lRect.mLeft) * unitsize; \
	type* lPicture = (type*)bitmap.GetBuffer(); \
 \
	int lPicWidth = (bitmap.GetWidth() << 16); \
	int lPicHeight = (bitmap.GetHeight() << 16); \
 \
	lYClipOffset %= lPicHeight; \
	lXClipOffset %= lPicWidth; \
 \
	int lPicY = lYClipOffset; \
	int lScreenPitch = lCanvas->GetPitch() * unitsize; \
	int lPicPitch = bitmap.GetPitch() * unitsize; \
 \
	lRenderWidth *= unitsize; \
 \
	const int lShiftVal = GetExponent(lPicPitch); \
 \
	Lepra::uint16 lAlphaValue = pThis->GetAlphaValue(); \
 \
	if (lPicPitch == (1 << lShiftVal)) \
	{ \
		for (int lY = 0; lY < lRenderHeight; lY++) \
		{ \
			/* A "light" optimization, removing the mul. */ \
			type* lSrcY = lPicture + ((lPicY >> 16) << lShiftVal); \
			int lPicX = lXClipOffset; \
 \
			for (int lX = 0; lX < lRenderWidth; lX += unitsize) \
			{ \
				/* This multiplication will be optimized by the compiler. */ \
				type* lSrc = lSrcY + (lPicX >> 16) * unitsize; \
				type* lDst = lScreen + lX; \
				copypixelmacro(lDst, lSrc, lAlphaValue); \
				lPicX += lXAdd; \
				if (lPicX >= lPicWidth) \
					lPicX %= lPicWidth; \
			} \
 \
			lScreen += lScreenPitch; \
			lPicY += lYAdd; \
			if (lPicY >= lPicHeight) \
				lPicY %= lPicHeight; \
		} \
	} \
	else \
	{ \
		for (int lY = 0; lY < lRenderHeight; lY++) \
		{ \
			type* lSrcY = lPicture + (lPicY >> 16) * lPicPitch; \
			int lPicX = lXClipOffset; \
 \
			for (int lX = 0; lX < lRenderWidth; lX += unitsize) \
			{ \
				/* This multiplication will be optimized by the compiler. */ \
				type* lSrc = lSrcY + (lPicX >> 16) * unitsize; \
				type* lDst = lScreen + lX; \
				copypixelmacro(lDst, lSrc, lAlphaValue); \
				lPicX += lXAdd; \
				if (lPicX >= lPicWidth) \
					lPicX %= lPicWidth; \
			} \
 \
			lScreen += lScreenPitch; \
			lPicY += lYAdd; \
			if (lPicY >= lPicHeight) \
				lPicY %= lPicHeight; \
		} \
	} \
}

#define MACRO_STRETCHBITMAPALPHA(bitmap, alpha, rect, subrect, pThis, type, unitsize, copypixelmacro) \
{ \
	Lepra::PixelRect lClippingRect; \
	pThis->GetScreenCoordClippingRect(lClippingRect); \
	Lepra::Canvas* lCanvas = pThis->GetCanvas(); \
 \
	Lepra::PixelRect lRect(rect); \
 \
	int lXAdd = (subrect.GetWidth() << 16) / lRect.GetWidth(); \
	int lYAdd = (subrect.GetHeight() << 16) / lRect.GetHeight(); \
 \
	int lXClipOffset = (subrect.mLeft << 16); \
	int lYClipOffset = (subrect.mTop << 16); \
 \
	if (lRect.mLeft < lClippingRect.mLeft) \
	{ \
		lXClipOffset = lXAdd * (lClippingRect.mLeft - lRect.mLeft); \
		lRect.mLeft = lClippingRect.mLeft; \
	} \
 \
	if (lRect.mTop < lClippingRect.mTop) \
	{ \
		lYClipOffset = lYAdd * (lClippingRect.mTop - lRect.mTop); \
		lRect.mTop = lClippingRect.mTop; \
	} \
 \
	int lRenderWidth = lRect.GetWidth(); \
	int lRenderHeight = lRect.GetHeight(); \
 \
	if (lRenderWidth <= 0 || lRenderHeight <= 0) \
	{ \
		return; \
	} \
 \
	if ((lRect.mLeft + lRenderWidth) > lClippingRect.mRight) \
	{ \
		lRenderWidth = lClippingRect.mRight - lRect.mLeft; \
	} \
 \
	if ((lRect.mTop + lRenderHeight) > lClippingRect.mBottom) \
	{ \
		lRenderHeight = lClippingRect.mBottom - lRect.mTop; \
	} \
 \
	type* lScreen = (type*)lCanvas->GetBuffer() + \
		(lRect.mTop * lCanvas->GetPitch() + lRect.mLeft) * unitsize; \
	type* lPicture = (type*)bitmap.GetBuffer(); \
	Lepra::uint8* lAlpha = (Lepra::uint8*)alpha->GetBuffer(); \
 \
	int lPicWidth = (bitmap.GetWidth() << 16); \
	int lPicHeight = (bitmap.GetHeight() << 16); \
 \
	lYClipOffset %= lPicHeight; \
	lXClipOffset %= lPicWidth; \
 \
	int lPicY = lYClipOffset; \
	int lScreenPitch = lCanvas->GetPitch() * unitsize; \
	int lPicPitch = bitmap.GetPitch() * unitsize; \
	int lAlphaPitch = alpha->GetPitch(); \
 \
	Lepra::uint16 lAlphaValue = pThis->GetAlphaValue(); \
 \
	lRenderWidth *= unitsize; \
 \
	for (int lY = 0; lY < lRenderHeight; lY++) \
	{ \
		type* lSrcY = lPicture + (lPicY >> 16) * lPicPitch; \
		Lepra::uint8* lAlphaY = lAlpha + (lPicY >> 16) * lAlphaPitch; \
		int lPicX = lXClipOffset; \
 \
		for (int lX = 0; lX < lRenderWidth; lX += unitsize) \
		{ \
			int lPX = lPicX >> 16; \
			/* This multiplication will be optimized by the compiler. */ \
			type* lSrc = lSrcY + lPX * unitsize; \
			type* lDst = lScreen + lX; \
			Lepra::uint8* lA = lAlphaY + lPX; \
			copypixelmacro(lDst, lSrc, lA, lAlphaValue); \
			lPicX += lXAdd; \
			if (lPicX >= lPicWidth) \
				lPicX %= lPicWidth; \
		} \
 \
		lScreen += lScreenPitch; \
		lPicY += lYAdd; \
		if (lPicY >= lPicHeight) \
			lPicY %= lPicHeight; \
	} \
}

void SoftwarePainter::DrawSImage8BitNormal(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	MACRO_STRETCHBITMAP(pImage, pRect, pSubrect, pThis, 
		Lepra::uint8, 1, MACRO_CopyUnit);
}

void SoftwarePainter::DrawSImage16BitNormal(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	MACRO_STRETCHBITMAP(pImage, pRect, pSubrect, pThis, 
		Lepra::uint16, 1, MACRO_CopyUnit);
}

void SoftwarePainter::DrawSImage24BitNormal(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	MACRO_STRETCHBITMAP(pImage, pRect, pSubrect, pThis, 
		Lepra::uint8, 3, MACRO_Copy24Bit);
}

void SoftwarePainter::DrawSImage32BitNormal(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_STRETCHBITMAP(pImage, pRect, pSubrect, pThis, 
			Lepra::uint8, 4, MACRO_Copy32Bit);
	}
	else
	{
		MACRO_STRETCHBITMAP(pImage, pRect, pSubrect, pThis, 
			unsigned, 1, MACRO_CopyUnit);
	}
}

void SoftwarePainter::DrawSImage8BitTest(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	MACRO_STRETCHBITMAPALPHA(pImage, pAlpha, pRect, pSubrect, pThis, 
		Lepra::uint8, 1, MACRO_CopyTestUnit);
}

void SoftwarePainter::DrawSImage16BitTest(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	MACRO_STRETCHBITMAPALPHA(pImage, pAlpha, pRect, pSubrect, pThis, 
		Lepra::uint16, 1, MACRO_CopyTestUnit);
}

void SoftwarePainter::DrawSImage24BitTest(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	MACRO_STRETCHBITMAPALPHA(pImage, pAlpha, pRect, pSubrect, pThis, 
		Lepra::uint8, 3, MACRO_CopyTest24Bit);
}

void SoftwarePainter::DrawSImage32BitTest(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_STRETCHBITMAPALPHA(pImage, pAlpha, pRect, pSubrect, pThis, 
			Lepra::uint8, 4, MACRO_CopyTest32Bit);
	}
	else
	{
		MACRO_STRETCHBITMAPALPHA(pImage, pAlpha, pRect, pSubrect, pThis, 
			unsigned, 1, MACRO_CopyTestUnit);
	}
}

void SoftwarePainter::DrawSImage15BitBlend(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	MACRO_STRETCHBITMAPALPHA(pImage, pAlpha, pRect, pSubrect, pThis, 
		Lepra::uint16, 1, MACRO_CopyBlend15Bit);
}

void SoftwarePainter::DrawSImage16BitBlend(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	MACRO_STRETCHBITMAPALPHA(pImage, pAlpha, pRect, pSubrect, pThis, 
		Lepra::uint16, 1, MACRO_CopyBlend16Bit);
}

void SoftwarePainter::DrawSImage24BitBlend(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	MACRO_STRETCHBITMAPALPHA(pImage, pAlpha, pRect, pSubrect, pThis, 
		Lepra::uint8, 3, MACRO_CopyBlend24Bit);
}

void SoftwarePainter::DrawSImage32BitBlend(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_STRETCHBITMAPALPHA(pImage, pAlpha, pRect, pSubrect, pThis, 
			Lepra::uint8, 4, MACRO_CopyBlend32Bit);
	}
	else
	{
		MACRO_STRETCHBITMAPALPHA(pImage, pAlpha, pRect, pSubrect, pThis, 
			Lepra::uint8, 4, MACRO_CopyBlend24Bit);
	}
}

void SoftwarePainter::DrawSImage15BitBlend2(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_STRETCHBITMAP(pImage, pRect, pSubpatchRect, pThis, 
		Lepra::uint16, 1, MACRO_CopyBlend215Bit);
}

void SoftwarePainter::DrawSImage16BitBlend2(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_STRETCHBITMAP(pImage, pRect, pSubpatchRect, pThis, 
		Lepra::uint16, 1, MACRO_CopyBlend216Bit);
}

void SoftwarePainter::DrawSImage24BitBlend2(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_STRETCHBITMAP(pImage, pRect, pSubpatchRect, pThis, 
		Lepra::uint8, 3, MACRO_CopyBlend224Bit);
}

void SoftwarePainter::DrawSImage32BitBlend2(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_STRETCHBITMAP(pImage, pRect, pSubpatchRect, pThis, 
			Lepra::uint8, 4, MACRO_CopyBlend232Bit);
	}
	else
	{
		MACRO_STRETCHBITMAP(pImage, pRect, pSubpatchRect, pThis, 
			Lepra::uint8, 4, MACRO_CopyBlend224Bit);
	}
}

void SoftwarePainter::DrawSImage8BitXor(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	MACRO_STRETCHBITMAP(pImage, pRect, pSubrect, pThis, 
		Lepra::uint8, 1, MACRO_CopyXorUnit);
}

void SoftwarePainter::DrawSImage16BitXor(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	MACRO_STRETCHBITMAP(pImage, pRect, pSubrect, pThis, 
		Lepra::uint16, 1, MACRO_CopyXorUnit);
}

void SoftwarePainter::DrawSImage24BitXor(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	MACRO_STRETCHBITMAP(pImage, pRect, pSubrect, pThis, 
		Lepra::uint8, 3, MACRO_CopyXor24Bit);
}

void SoftwarePainter::DrawSImage32BitXor(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_STRETCHBITMAP(pImage, pRect, pSubrect, pThis, 
			Lepra::uint8, 4, MACRO_CopyXor32Bit);
	}
	else
	{
		MACRO_STRETCHBITMAP(pImage, pRect, pSubrect, pThis, 
			unsigned, 1, MACRO_CopyXorUnit);
	}
}

void SoftwarePainter::DrawSImage8BitAdd(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	MACRO_STRETCHBITMAP(pImage, pRect, pSubrect, pThis, 
		Lepra::uint16, 1, MACRO_CopyAdd8Bit);
}

void SoftwarePainter::DrawSImage15BitAdd(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	MACRO_STRETCHBITMAP(pImage, pRect, pSubrect, pThis, 
		Lepra::uint16, 1, MACRO_CopyAdd15Bit);
}

void SoftwarePainter::DrawSImage16BitAdd(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	MACRO_STRETCHBITMAP(pImage, pRect, pSubrect, pThis, 
		Lepra::uint16, 1, MACRO_CopyAdd16Bit);
}

void SoftwarePainter::DrawSImage24BitAdd(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	MACRO_STRETCHBITMAP(pImage, pRect, pSubrect, pThis, 
		Lepra::uint8, 3, MACRO_CopyAdd24Bit);
}

void SoftwarePainter::DrawSImage32BitAdd(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubrect, SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_STRETCHBITMAP(pImage, pRect, pSubrect, pThis, 
			Lepra::uint8, 4, MACRO_CopyAdd32Bit);
	}
	else
	{
		MACRO_STRETCHBITMAP(pImage, pRect, pSubrect, pThis, 
			Lepra::uint8, 4, MACRO_CopyAdd24Bit);
	}
}

} // End namespace.
