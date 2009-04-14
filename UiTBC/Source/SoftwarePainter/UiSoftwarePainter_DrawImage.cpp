/*
	Class:  SoftwarePainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/UiSoftwarePainter.h"
#include "UiSoftwarePainter_DrawPixelMacros.h"

#include <memory.h>

namespace UiTbc
{

#define MACRO_DRAWBITMAP(bitmap, x, y, subrect, pThis, type, unitsize, pixelsize, spanmacro, tiledspanmacro, copypixelmacro) \
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
	int lRenderWidth = subrect.GetWidth() - lXClipOffset; \
	int lRenderHeight = subrect.GetHeight() - lYClipOffset; \
 \
	/* Add top left corner offset... */ \
	lXClipOffset += subrect.mLeft; \
	lYClipOffset += subrect.mTop; \
 \
	lXClipOffset %= bitmap.GetWidth(); \
	lYClipOffset %= bitmap.GetHeight(); \
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
	if (lRenderWidth <= 0 || lRenderHeight <= 0) \
	{ \
		return; \
	} \
 \
	type* lScreen = (type*)lCanvas->GetBuffer() + (y * lCanvas->GetPitch() + x) * unitsize; \
	type* lPicture = (type*)bitmap.GetBuffer() + lYClipOffset * bitmap.GetPitch() * unitsize; \
 \
	int lHeight = bitmap.GetHeight(); \
	int lWidth = bitmap.GetWidth(); \
	int lScreenPitch = lCanvas->GetPitch() * unitsize; \
	int lPicturePitch = bitmap.GetPitch() * unitsize; \
 \
	if (lRenderHeight > lHeight) \
	{ \
		int lPictureVSize = lHeight * lPicturePitch; \
 \
		if (lRenderWidth > lWidth) \
		{ \
			for (int lY = 0; lY < lRenderHeight; lY++) \
			{ \
				if (lY >= lHeight) \
				{ \
					lY = 0; \
					lRenderHeight -= lHeight; \
					lPicture -= lPictureVSize; \
				} \
 \
				tiledspanmacro(lScreen, lPicture, lRenderWidth, lWidth, lXClipOffset, type, unitsize, pixelsize, copypixelmacro); \
				lScreen += lScreenPitch; \
				lPicture += lPicturePitch; \
			} \
		} \
		else \
		{ \
			for (int lY = 0; lY < lRenderHeight; lY++) \
			{ \
				if (lY >= lHeight) \
				{ \
					lY = 0; \
					lRenderHeight -= lHeight; \
					lPicture -= lPictureVSize; \
				} \
 \
				lPicture += lXClipOffset * (unitsize); \
				spanmacro(lScreen, lPicture, lRenderWidth, lWidth, type, unitsize, pixelsize, copypixelmacro); \
				lScreen += lScreenPitch; \
				lPicture += lPicturePitch; \
			} \
		} \
	} \
	else \
	{ \
		if (lRenderWidth > lWidth) \
		{ \
			for (int lY = 0; lY < lRenderHeight; lY++) \
			{ \
				tiledspanmacro(lScreen, lPicture, lRenderWidth, lWidth, lXClipOffset, type, unitsize, pixelsize, copypixelmacro); \
				lScreen += lScreenPitch; \
				lPicture += lPicturePitch; \
			} \
		} \
		else \
		{ \
			lPicture += lXClipOffset * (unitsize); \
			for (int lY = 0; lY < lRenderHeight; lY++) \
			{ \
				spanmacro(lScreen, lPicture, lRenderWidth, lWidth, type, unitsize, pixelsize, copypixelmacro); \
				lScreen += lScreenPitch; \
				lPicture += lPicturePitch; \
			} \
		} \
	} \
}

#define MACRO_DRAWBITMAPALPHA(bitmap, alpha, x, y, subrect, pThis, type, unitsize, pixelsize, spanmacro, tiledspanmacro, copypixelmacro) \
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
	int lRenderWidth = subrect.GetWidth() - lXClipOffset; \
	int lRenderHeight = subrect.GetHeight() - lYClipOffset; \
 \
	/* Add top left corner offset... */ \
	lXClipOffset += subrect.mLeft; \
	lYClipOffset += subrect.mTop; \
 \
	lXClipOffset %= bitmap.GetWidth(); \
	lYClipOffset %= bitmap.GetHeight(); \
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
	if (lRenderWidth <= 0 || lRenderHeight <= 0) \
	{ \
		return; \
	} \
 \
	type* lScreen = (type*)lCanvas->GetBuffer() + (y * lCanvas->GetPitch() + x) * unitsize; \
	type* lPicture = (type*)bitmap.GetBuffer() + (lYClipOffset * bitmap.GetPitch()) * unitsize; \
	Lepra::uint8* lAlpha = (Lepra::uint8*)alpha->GetBuffer() + lYClipOffset * alpha->GetPitch(); \
 \
	int lHeight = bitmap.GetHeight(); \
	int lWidth = bitmap.GetWidth(); \
	int lScreenPitch = lCanvas->GetPitch() * unitsize; \
	int lPicturePitch = bitmap.GetPitch() * unitsize; \
 \
	if (lRenderHeight > lHeight) \
	{ \
		int lPictureVSize = lHeight * lPicturePitch; \
		int lAlphaVSize   = lHeight * alpha->GetPitch(); \
 \
		if (lRenderWidth > lWidth) \
		{ \
			for (int lY = 0; lY < lRenderHeight; lY++) \
			{ \
				if (lY >= lHeight) \
				{ \
					lY = 0; \
					lRenderHeight -= lHeight; \
					lPicture -= lPictureVSize; \
					lAlpha -= lAlphaVSize; \
				} \
 \
				tiledspanmacro(lScreen, lPicture, lAlpha, lRenderWidth, lWidth, lXClipOffset, type, unitsize, pixelsize, copypixelmacro); \
				lScreen += lScreenPitch; \
				lPicture += lPicturePitch; \
				lAlpha += alpha->GetPitch(); \
			} \
		} \
		else \
		{ \
			for (int lY = 0; lY < lRenderHeight; lY++) \
			{ \
				if (lY >= lHeight) \
				{ \
					lY = 0; \
					lRenderHeight -= lHeight; \
					lPicture -= lPictureVSize; \
					lAlpha -= lAlphaVSize; \
				} \
 \
				lPicture += lXClipOffset * (unitsize); \
				lAlpha   += lXClipOffset; \
				spanmacro(lScreen, lPicture, lAlpha, lRenderWidth, lWidth, type, unitsize, pixelsize, copypixelmacro); \
				lScreen += lScreenPitch; \
				lPicture += lPicturePitch; \
				lAlpha += alpha->GetPitch(); \
			} \
		} \
	} \
	else \
	{ \
		if (lRenderWidth > lWidth) \
		{ \
			for (int lY = 0; lY < lRenderHeight; lY++) \
			{ \
				tiledspanmacro(lScreen, lPicture, lAlpha, lRenderWidth, lWidth, lXClipOffset, type, unitsize, pixelsize, copypixelmacro); \
				lScreen += lScreenPitch; \
				lPicture += lPicturePitch; \
				lAlpha += alpha->GetPitch(); \
			} \
		} \
		else \
		{ \
			lPicture += lXClipOffset * (unitsize); \
			lAlpha   += lXClipOffset; \
			for (int lY = 0; lY < lRenderHeight; lY++) \
			{ \
				spanmacro(lScreen, lPicture, lAlpha, lRenderWidth, lWidth, type, unitsize, pixelsize, copypixelmacro); \
				lScreen += lScreenPitch; \
				lPicture += lPicturePitch; \
				lAlpha += alpha->GetPitch(); \
			} \
		} \
	} \
}

#define MACRO_SPANMEMCPY(destPtr, srcPtr, spanlength, srcwidth, type, unitsize, pixelsize, copypixelmacro) \
{ \
	memcpy(destPtr, srcPtr, spanlength * pixelsize); \
}

#define MACRO_SPANMEMCPYTILED(destPtr, srcPtr, spanlength, srcwidth, xclipoffs, type, unitsize, pixelsize, copypixelmacro) \
{ \
	int lSpanLength = (int)(spanlength); \
 	type* lDest = (type*)destPtr; \
 \
	if (xclipoffs != 0) \
	{ \
		type* lSrc  = ((type*)srcPtr) + (xclipoffs) * (unitsize); \
		int lWidth__ = (srcwidth) - (xclipoffs); \
		lSpanLength -= lWidth__; \
		memcpy(lDest, lSrc, lWidth__ * (pixelsize)); \
		lDest += lWidth__ * (unitsize); \
	} \
 \
	if (lSpanLength > 0) \
	{ \
		int lSize    = (srcwidth) * (pixelsize); \
		int lCount   = lSpanLength / (srcwidth); \
		int lRest    = lSpanLength - (srcwidth) * lCount; \
		int lDestAdd = (srcwidth) * (unitsize); \
 \
		for (int i = 0; i < lCount; i++) \
		{ \
			memcpy(lDest, srcPtr, lSize); \
			lDest += lDestAdd; \
		} \
 \
		memcpy(lDest, srcPtr, lRest * pixelsize); \
	} \
}

#define MACRO_SPANITER(destPtr, srcPtr, spanlength, srcwidth, type, unitsize, pixelsize, copypixelmacro) \
{ \
	type* lDest = destPtr; \
	type* lSrc  = srcPtr; \
	Lepra::uint16 lAlphaValue = pThis->GetAlphaValue(); \
 \
	for (int lX = 0; lX < spanlength; lX++) \
	{ \
		copypixelmacro(lDest, lSrc, lAlphaValue); \
 \
		lDest += unitsize; \
		lSrc  += unitsize; \
	} \
}

#define MACRO_SPANITERTILED(destPtr, srcPtr, spanlength, srcwidth, xclipoffs, type, unitsize, pixelsize, copypixelmacro) \
{ \
	int lSpanLength = (int)(spanlength); \
	int lX; \
 	type* lDest = (type*)destPtr; \
	type* lSrc  = (type*)srcPtr; \
	Lepra::uint16 lAlphaValue = pThis->GetAlphaValue(); \
 \
	if (xclipoffs != 0) \
	{ \
		lSrc  += (xclipoffs) * (unitsize); \
		int lWidth__ = (srcwidth) - (xclipoffs); \
		lSpanLength -= lWidth__; \
		for (lX = 0; lX < lWidth__; lX++) \
		{ \
			copypixelmacro(lDest, lSrc, lAlphaValue); \
 \
			lDest += unitsize; \
			lSrc  += unitsize; \
		} \
	} \
 \
	if (lSpanLength > 0) \
	{ \
		int lCount = lSpanLength / (srcwidth); \
		int lRest  = lSpanLength - lCount * (srcwidth); \
	 \
		for (int i = 0; i < lCount; i++) \
		{ \
			lSrc  = (type*)srcPtr; \
			for (lX = 0; lX < srcwidth; lX++) \
			{ \
				copypixelmacro(lDest, lSrc, lAlphaValue); \
		\
				lDest += unitsize; \
				lSrc  += unitsize; \
			} \
		} \
	 \
		lSrc  = (type*)srcPtr; \
		for (lX = 0; lX < lRest; lX++) \
		{ \
			copypixelmacro(lDest, lSrc, lAlphaValue); \
	 \
			lDest += unitsize; \
			lSrc  += unitsize; \
		} \
	} \
}


#define MACRO_SPANITERALPHA(destPtr, srcPtr, alphaPtr, spanlength, srcwidth, type, unitsize, pixelsize, copypixelmacro) \
{ \
	type* lDest = destPtr; \
	type* lSrc  = srcPtr; \
	Lepra::uint8* lA = alphaPtr; \
	Lepra::uint16 lAlphaValue = pThis->GetAlphaValue(); \
	for (int lX = 0; lX < spanlength; lX++) \
	{ \
		copypixelmacro(lDest, lSrc, lA, lAlphaValue); \
 \
		lDest += unitsize; \
		lSrc  += unitsize; \
		lA++; \
	} \
}

#define MACRO_SPANITERALPHATILED(destPtr, srcPtr, alphaPtr, spanlength, srcwidth, xclipoffs, type, unitsize, pixelsize, copypixelmacro) \
{ \
	int lSpanLength = (int)(spanlength); \
	int lX; \
 	type* lDest = (type*)destPtr; \
	type* lSrc  = (type*)srcPtr; \
	Lepra::uint8* lA   = (Lepra::uint8*)alphaPtr; \
	Lepra::uint16 lAlphaValue = pThis->GetAlphaValue(); \
 \
	if (xclipoffs != 0) \
	{ \
		lSrc += (xclipoffs) * (unitsize); \
		lA   += (xclipoffs); \
		int lWidth__ = (srcwidth) - (xclipoffs); \
		lSpanLength -= lWidth__; \
		for (lX = 0; lX < lWidth__; lX++) \
		{ \
			copypixelmacro(lDest, lSrc, lA, lAlphaValue); \
 \
			lDest += unitsize; \
			lSrc  += unitsize; \
			lA++; \
		} \
	} \
 \
	if (lSpanLength > 0) \
	{ \
		int lCount = lSpanLength / (srcwidth); \
		int lRest  = lSpanLength - lCount * (srcwidth); \
		type* lSrc  = srcPtr; \
		Lepra::uint8* lA = alphaPtr; \
 \
		for (int i = 0; i < lCount; i++) \
		{ \
			lSrc = srcPtr; \
			lA   = alphaPtr; \
			for (lX = 0; lX < srcwidth; lX++) \
			{ \
				copypixelmacro(lDest, lSrc, lA, lAlphaValue); \
 \
				lDest += unitsize; \
				lSrc  += unitsize; \
				lA++; \
			} \
		} \
 \
		lSrc = srcPtr; \
		lA   = alphaPtr; \
		for (lX = 0; lX < lRest; lX++) \
		{ \
			copypixelmacro(lDest, lSrc, lA, lAlphaValue); \
 \
			lDest += unitsize; \
			lSrc  += unitsize; \
			lA++; \
		} \
	} \
}

void SoftwarePainter::DrawImage8BitNormal(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
		Lepra::uint8, 1, 1, MACRO_SPANMEMCPY, MACRO_SPANMEMCPYTILED, MACRO_CopyUnit);
}

void SoftwarePainter::DrawImage16BitNormal(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
		Lepra::uint16, 1, 2, MACRO_SPANMEMCPY, MACRO_SPANMEMCPYTILED, MACRO_CopyUnit);
}

void SoftwarePainter::DrawImage24BitNormal(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
		Lepra::uint8, 3, 3, MACRO_SPANMEMCPY, MACRO_SPANMEMCPYTILED, MACRO_CopyUnit);
}

void SoftwarePainter::DrawImage32BitNormal(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
			Lepra::uint8, 4, 4, MACRO_SPANMEMCPY, MACRO_SPANMEMCPYTILED, MACRO_Copy32Bit);
	}
	else
	{
		MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
			unsigned, 1, 4, MACRO_SPANMEMCPY, MACRO_SPANMEMCPYTILED, MACRO_CopyUnit);
	}
}

void SoftwarePainter::DrawImage8BitTest(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_DRAWBITMAPALPHA(pImage, pAlpha, x, y, pSubpatchRect, pThis, 
		Lepra::uint8, 1, 1, MACRO_SPANITERALPHA, MACRO_SPANITERALPHATILED, MACRO_CopyTestUnit);
}

void SoftwarePainter::DrawImage16BitTest(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_DRAWBITMAPALPHA(pImage, pAlpha, x, y, pSubpatchRect, pThis, 
		Lepra::uint16, 1, 2, MACRO_SPANITERALPHA, MACRO_SPANITERALPHATILED, MACRO_CopyTestUnit);
}

void SoftwarePainter::DrawImage24BitTest(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_DRAWBITMAPALPHA(pImage, pAlpha, x, y, pSubpatchRect, pThis, 
		Lepra::uint8, 3, 3, MACRO_SPANITERALPHA, MACRO_SPANITERALPHATILED, MACRO_CopyTest24Bit);
}

void SoftwarePainter::DrawImage32BitTest(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWBITMAPALPHA(pImage, pAlpha, x, y, pSubpatchRect, pThis, 
			Lepra::uint8, 4, 4, MACRO_SPANITERALPHA, MACRO_SPANITERALPHATILED, MACRO_CopyTest32Bit);
	}
	else
	{
		MACRO_DRAWBITMAPALPHA(pImage, pAlpha, x, y, pSubpatchRect, pThis, 
			unsigned, 1, 4, MACRO_SPANITERALPHA, MACRO_SPANITERALPHATILED, MACRO_CopyTestUnit);
	}
}

void SoftwarePainter::DrawImage15BitBlend(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_DRAWBITMAPALPHA(pImage, pAlpha, x, y, pSubpatchRect, pThis, 
		Lepra::uint16, 1, 2, MACRO_SPANITERALPHA, MACRO_SPANITERALPHATILED, MACRO_CopyBlend15Bit);
}

void SoftwarePainter::DrawImage16BitBlend(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_DRAWBITMAPALPHA(pImage, pAlpha, x, y, pSubpatchRect, pThis, 
		Lepra::uint16, 1, 2, MACRO_SPANITERALPHA, MACRO_SPANITERALPHATILED, MACRO_CopyBlend16Bit);
}

void SoftwarePainter::DrawImage24BitBlend(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_DRAWBITMAPALPHA(pImage, pAlpha, x, y, pSubpatchRect, pThis, 
		Lepra::uint8, 3, 3, MACRO_SPANITERALPHA, MACRO_SPANITERALPHATILED, MACRO_CopyBlend24Bit);
}

void SoftwarePainter::DrawImage32BitBlend(const Lepra::Canvas& pImage, const Lepra::Canvas* pAlpha, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWBITMAPALPHA(pImage, pAlpha, x, y, pSubpatchRect, pThis, 
			Lepra::uint8, 4, 4, MACRO_SPANITERALPHA, MACRO_SPANITERALPHATILED, MACRO_CopyBlend32Bit);
	}
	else
	{
		MACRO_DRAWBITMAPALPHA(pImage, pAlpha, x, y, pSubpatchRect, pThis, 
			Lepra::uint8, 4, 4, MACRO_SPANITERALPHA, MACRO_SPANITERALPHATILED, MACRO_CopyBlend24Bit);
	}
}

void SoftwarePainter::DrawImage15BitBlend2(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
		Lepra::uint16, 1, 2, MACRO_SPANITER, MACRO_SPANITERTILED, MACRO_CopyBlend215Bit);
}

void SoftwarePainter::DrawImage16BitBlend2(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
		Lepra::uint16, 1, 2, MACRO_SPANITER, MACRO_SPANITERTILED, MACRO_CopyBlend216Bit);
}

void SoftwarePainter::DrawImage24BitBlend2(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
		Lepra::uint8, 3, 3, MACRO_SPANITER, MACRO_SPANITERTILED, MACRO_CopyBlend224Bit);
}

void SoftwarePainter::DrawImage32BitBlend2(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
			Lepra::uint8, 4, 4, MACRO_SPANITER, MACRO_SPANITERTILED, MACRO_CopyBlend232Bit);
	}
	else
	{
		MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
			Lepra::uint8, 4, 4, MACRO_SPANITER, MACRO_SPANITERTILED, MACRO_CopyBlend224Bit);
	}
}

void SoftwarePainter::DrawImage8BitXor(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
		Lepra::uint8, 1, 1, MACRO_SPANITER, MACRO_SPANITERTILED, MACRO_CopyXorUnit);
}

void SoftwarePainter::DrawImage16BitXor(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
		Lepra::uint16, 1, 2, MACRO_SPANITER, MACRO_SPANITERTILED, MACRO_CopyXorUnit);
}

void SoftwarePainter::DrawImage24BitXor(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
		Lepra::uint8, 3, 3, MACRO_SPANITER, MACRO_SPANITERTILED, MACRO_CopyXor24Bit);
}

void SoftwarePainter::DrawImage32BitXor(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
			Lepra::uint8, 4, 4, MACRO_SPANITER, MACRO_SPANITERTILED, MACRO_CopyXor32Bit);
	}
	else
	{
		MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
			unsigned, 1, 4, MACRO_SPANITER, MACRO_SPANITERTILED, MACRO_CopyXorUnit);
	}
}

void SoftwarePainter::DrawImage8BitAdd(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
		Lepra::uint8, 1, 1, MACRO_SPANITER, MACRO_SPANITERTILED, MACRO_CopyAdd8Bit);
}

void SoftwarePainter::DrawImage15BitAdd(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
		Lepra::uint16, 1, 2, MACRO_SPANITER, MACRO_SPANITERTILED, MACRO_CopyAdd15Bit);
}

void SoftwarePainter::DrawImage16BitAdd(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
		Lepra::uint16, 1, 2, MACRO_SPANITER, MACRO_SPANITERTILED, MACRO_CopyAdd16Bit);
}

void SoftwarePainter::DrawImage24BitAdd(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
		Lepra::uint8, 3, 3, MACRO_SPANITER, MACRO_SPANITERTILED, MACRO_CopyAdd24Bit);
}

void SoftwarePainter::DrawImage32BitAdd(const Lepra::Canvas& pImage, const Lepra::Canvas* /*pAlpha*/, int x, int y, const Lepra::PixelRect& pSubpatchRect, SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
			Lepra::uint8, 4, 4, MACRO_SPANITER, MACRO_SPANITERTILED, MACRO_CopyAdd32Bit);
	}
	else
	{
		MACRO_DRAWBITMAP(pImage, x, y, pSubpatchRect, pThis, 
			Lepra::uint8, 4, 4, MACRO_SPANITER, MACRO_SPANITERTILED, MACRO_CopyAdd24Bit);
	}
}

} // End namespace.
