/*
	Lepra::File:   SoftwarePainter_DrawScanLine.cpp
	Class:  SoftwarePainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/UiSoftwarePainter.h"
#include "UiSoftwarePainter_DrawPixelMacros.h"

namespace UiTbc
{

// Pow2 refers to the dimensions of the texture. If they are a
// power of 2, this macro should be used.
#define MACRO_DRAWSCANLINEPOW2(gradients, eleft, eright, texture, tshift, pThis, type, unitsize, copypixelmacro) \
{ \
	int lXStart = eleft->GetX(); \
	int lWidth = eright->GetX() - lXStart; \
 \
	if (lWidth > 0) \
	{ \
		Lepra::Canvas* lCanvas = pThis->GetCanvas(); \
		type* lDestBits = (type*)lCanvas->GetBuffer() + \
							(eleft->GetY() * lCanvas->GetPitch() + lXStart) * unitsize; \
		type* lTextureBits = (type*)texture->GetBuffer(); \
 \
		unsigned lTWMask = texture->GetWidth() - 1; \
		unsigned lTHMask = texture->GetHeight() - 1; \
 \
		Lepra::FixedPointMath::Fixed16_16 lU; \
		Lepra::FixedPointMath::Fixed16_16 lV; \
		Lepra::FixedPointMath::Fixed16_16 lDeltaU; \
		Lepra::FixedPointMath::Fixed16_16 lDeltaV; \
 \
		lU = Lepra::FixedPointMath::FloatToFixed16_16(eleft->GetU()) + gradients->GetDUDXModifier(); \
		lV = Lepra::FixedPointMath::FloatToFixed16_16(eleft->GetV()) + gradients->GetDVDXModifier(); \
		lDeltaU = gradients->GetUXStep16_16(); \
		lDeltaV = gradients->GetVXStep16_16(); \
 \
		Lepra::uint16 lAlphaValue = (Lepra::uint16)pThis->GetAlphaValue(); \
 \
		for (unsigned i = lWidth; i > 0; i--) \
		{ \
			type* lSrc = &lTextureBits[((((lV >> 16) & lTHMask) << tshift) + ((lU >> 16) & lTWMask)) * unitsize]; \
			copypixelmacro(lDestBits, lSrc, lAlphaValue); \
 \
			lDestBits += unitsize; \
			lU += lDeltaU; \
			lV += lDeltaV; \
		} \
	} \
}

// Pow2 refers to the dimensions of the texture. If they are a
// power of 2, this macro should be used.
#define MACRO_DRAWSCANLINEPOW2ALPHA(gradients, eleft, eright, texture, alpha, tshift, pThis, type, unitsize, copypixelmacro) \
{ \
	int lXStart = eleft->GetX(); \
	int lWidth = eright->GetX() - lXStart; \
 \
	if (lWidth > 0) \
	{ \
		Lepra::Canvas* lCanvas = pThis->GetCanvas(); \
		type* lDestBits = (type*)lCanvas->GetBuffer() + \
							(eleft->GetY() * lCanvas->GetPitch() + lXStart) * unitsize; \
		type* lTextureBits = (type*)texture->GetBuffer(); \
		Lepra::uint8* lAlphaBits = (Lepra::uint8*)alpha->GetBuffer(); \
 \
		unsigned lTWMask = texture->GetWidth() - 1; \
		unsigned lTHMask = texture->GetHeight() - 1; \
 \
		Lepra::FixedPointMath::Fixed16_16 lU; \
		Lepra::FixedPointMath::Fixed16_16 lV; \
		Lepra::FixedPointMath::Fixed16_16 lDeltaU; \
		Lepra::FixedPointMath::Fixed16_16 lDeltaV; \
 \
		lU = Lepra::FixedPointMath::FloatToFixed16_16(eleft->GetU()) + gradients->GetDUDXModifier(); \
		lV = Lepra::FixedPointMath::FloatToFixed16_16(eleft->GetV()) + gradients->GetDVDXModifier(); \
		lDeltaU = gradients->GetUXStep16_16(); \
		lDeltaV = gradients->GetVXStep16_16(); \
 \
		Lepra::uint16 lAlphaValue = (Lepra::uint16)pThis->GetAlphaValue(); \
 \
		for (unsigned i = lWidth; i > 0; i--) \
		{ \
			Lepra::FixedPointMath::Fixed16_16 lUD = (lU >> 16); \
			Lepra::FixedPointMath::Fixed16_16 lVD = (lV >> 16); \
			unsigned lOffset = ((lVD & lTHMask) << tshift) + (lUD & lTWMask); \
 \
			type* lSrc = &lTextureBits[lOffset * unitsize]; \
			Lepra::uint8* lAlpha = &lAlphaBits[lOffset]; \
			copypixelmacro(lDestBits, lSrc, lAlpha, lAlphaValue); \
 \
			lDestBits += unitsize; \
			lU += lDeltaU; \
			lV += lDeltaV; \
		} \
	} \
}

#define MACRO_DRAWSCANLINE(gradients, eleft, eright, texture, pThis, type, unitsize, copypixelmacro) \
{ \
	int lXStart = eleft->GetX(); \
	int lWidth = eright->GetX() - lXStart; \
 \
	if (lWidth > 0) \
	{ \
		Lepra::Canvas* lCanvas = pThis->GetCanvas(); \
		type* lDestBits = (type*)lCanvas->GetBuffer() + \
							(eleft->GetY() * lCanvas->GetPitch() + lXStart) * unitsize; \
		type* lTextureBits = (type*)texture->GetBuffer(); \
 \
		Lepra::FixedPointMath::Fixed16_16 lU; \
		Lepra::FixedPointMath::Fixed16_16 lV; \
		Lepra::FixedPointMath::Fixed16_16 lDeltaU; \
		Lepra::FixedPointMath::Fixed16_16 lDeltaV; \
 \
		lU = Lepra::FixedPointMath::FloatToFixed16_16(eleft->GetU()) + gradients->GetDUDXModifier(); \
		lV = Lepra::FixedPointMath::FloatToFixed16_16(eleft->GetV()) + gradients->GetDVDXModifier(); \
		lDeltaU = gradients->GetUXStep16_16(); \
		lDeltaV = gradients->GetVXStep16_16(); \
 \
		Lepra::uint16 lAlphaValue = (Lepra::uint16)pThis->GetAlphaValue(); \
 \
		for (unsigned i = lWidth; i > 0; i--) \
		{ \
			type* lSrc = &lTextureBits[(((lV >> 16) * texture->GetPitch()) + (lU >> 16)) * unitsize]; \
			copypixelmacro(lDestBits, lSrc, lAlphaValue); \
 \
			lDestBits += unitsize; \
			lU += lDeltaU; \
			lV += lDeltaV; \
		} \
	} \
}

#define MACRO_DRAWSCANLINEALPHA(gradients, eleft, eright, texture, alpha, pThis, type, unitsize, copypixelmacro) \
{ \
	int lXStart = eleft->GetX(); \
	int lWidth = eright->GetX() - lXStart; \
 \
	if (lWidth > 0) \
	{ \
		Lepra::Canvas* lCanvas = pThis->GetCanvas(); \
		type* lDestBits = (type*)lCanvas->GetBuffer() + \
							(eleft->GetY() * lCanvas->GetPitch() + lXStart) * unitsize; \
		type* lTextureBits = (type*)texture->GetBuffer(); \
		Lepra::uint8* lAlphaBits = (Lepra::uint8*)alpha->GetBuffer(); \
 \
		Lepra::FixedPointMath::Fixed16_16 lU; \
		Lepra::FixedPointMath::Fixed16_16 lV; \
		Lepra::FixedPointMath::Fixed16_16 lDeltaU; \
		Lepra::FixedPointMath::Fixed16_16 lDeltaV; \
 \
		lU = Lepra::FixedPointMath::FloatToFixed16_16(eleft->GetU()) + gradients->GetDUDXModifier(); \
		lV = Lepra::FixedPointMath::FloatToFixed16_16(eleft->GetV()) + gradients->GetDVDXModifier(); \
		lDeltaU = gradients->GetUXStep16_16(); \
		lDeltaV = gradients->GetVXStep16_16(); \
 \
		Lepra::uint16 lAlphaValue = (Lepra::uint16)pThis->GetAlphaValue(); \
 \
		for (unsigned i = lWidth; i > 0; i--) \
		{ \
			Lepra::FixedPointMath::Fixed16_16 lUD = (lU >> 16); \
			Lepra::FixedPointMath::Fixed16_16 lVD = (lV >> 16); \
			type* lSrc = &lTextureBits[((lVD * texture->GetPitch()) + lUD) * unitsize]; \
			Lepra::uint8* lAlpha = &lAlphaBits[(lVD * alpha->GetPitch()) + lUD]; \
			copypixelmacro(lDestBits, lSrc, lAlpha, lAlphaValue); \
 \
			lDestBits += unitsize; \
			lU += lDeltaU; \
			lV += lDeltaV; \
		} \
	} \
}

void SoftwarePainter::DrawScanLine8BitNormal(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight,
	const Lepra::Canvas* pTexture, const Lepra::Canvas* /*pAlpha*/, SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
		Lepra::uint8, 1, MACRO_CopyUnit);
}

void SoftwarePainter::DrawScanLine16BitNormal(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight,
	const Lepra::Canvas* pTexture, const Lepra::Canvas* /*pAlpha*/, SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
								Lepra::uint16, 1, MACRO_CopyUnit);
}

void SoftwarePainter::DrawScanLine24BitNormal(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight,
	const Lepra::Canvas* pTexture, const Lepra::Canvas* /*pAlpha*/, SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
								Lepra::uint8, 3, MACRO_Copy24Bit);
}

void SoftwarePainter::DrawScanLine32BitNormal(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight,
	const Lepra::Canvas* pTexture, const Lepra::Canvas* /*pAlpha*/, SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
			Lepra::uint8, 4, MACRO_Copy32Bit);
	}
	else
	{
		MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
			unsigned, 1, MACRO_CopyUnit);
	}
}

void SoftwarePainter::DrawScanLine15BitBlend(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight,
	const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pThis,
		Lepra::uint16, 1, MACRO_CopyBlend15Bit);
}

void SoftwarePainter::DrawScanLine16BitBlend(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight,
	const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pThis,
		Lepra::uint16, 1, MACRO_CopyBlend16Bit);
}

void SoftwarePainter::DrawScanLine24BitBlend(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight,
	const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pThis,
		Lepra::uint8, 3, MACRO_CopyBlend24Bit);
}

void SoftwarePainter::DrawScanLine32BitBlend(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight,
	const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha, SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWSCANLINEALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pThis,
			Lepra::uint8, 4, MACRO_CopyBlend32Bit);
	}
	else
	{
		MACRO_DRAWSCANLINEALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pThis,
			Lepra::uint8, 4, MACRO_CopyBlend24Bit);
	}
}

void SoftwarePainter::DrawScanLine15BitBlend2(const Gradients2DUV* pGradients, Edge2DUV* pLeft, Edge2DUV* pRight,
	const Lepra::Canvas* pTexture, const Lepra::Canvas* /*pAlpha*/, SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
		Lepra::uint16, 1, MACRO_CopyBlend215Bit);
}

void SoftwarePainter::DrawScanLine16BitBlend2(const Gradients2DUV* pGradients, 
											  Edge2DUV* pLeft, 
											  Edge2DUV* pRight,
											  const Lepra::Canvas* pTexture,
											  const Lepra::Canvas* /*pAlpha*/,
											  SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
							Lepra::uint16, 1, MACRO_CopyBlend216Bit);
}

void SoftwarePainter::DrawScanLine24BitBlend2(const Gradients2DUV* pGradients, 
											  Edge2DUV* pLeft, 
											  Edge2DUV* pRight,
											  const Lepra::Canvas* pTexture,
											  const Lepra::Canvas* /*pAlpha*/,
											  SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
							Lepra::uint8, 3, MACRO_CopyBlend224Bit);
}

void SoftwarePainter::DrawScanLine32BitBlend2(const Gradients2DUV* pGradients, 
											  Edge2DUV* pLeft, 
											  Edge2DUV* pRight,
											  const Lepra::Canvas* pTexture,
											  const Lepra::Canvas* /*pAlpha*/,
											  SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
							Lepra::uint8, 4, MACRO_CopyBlend232Bit);
	}
	else
	{
		MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
								Lepra::uint8, 4, MACRO_CopyBlend224Bit);
	}
}

void SoftwarePainter::DrawScanLine8BitTest(const Gradients2DUV* pGradients, 
										   Edge2DUV* pLeft, 
										   Edge2DUV* pRight,
										   const Lepra::Canvas* pTexture,
										   const Lepra::Canvas* pAlpha,
										   SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pThis,
								Lepra::uint8, 1, MACRO_CopyTestUnit);
}

void SoftwarePainter::DrawScanLine16BitTest(const Gradients2DUV* pGradients, 
											Edge2DUV* pLeft, 
											Edge2DUV* pRight,
											const Lepra::Canvas* pTexture,
											const Lepra::Canvas* pAlpha,
											SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pThis,
								Lepra::uint16, 1, MACRO_CopyTestUnit);
}

void SoftwarePainter::DrawScanLine24BitTest(const Gradients2DUV* pGradients, 
											Edge2DUV* pLeft, 
											Edge2DUV* pRight,
											const Lepra::Canvas* pTexture,
											const Lepra::Canvas* pAlpha,
											SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pThis,
								Lepra::uint8, 3, MACRO_CopyTest24Bit);
}

void SoftwarePainter::DrawScanLine32BitTest(const Gradients2DUV* pGradients, 
											Edge2DUV* pLeft, 
											Edge2DUV* pRight,
											const Lepra::Canvas* pTexture,
											const Lepra::Canvas* pAlpha,
											SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWSCANLINEALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pThis,
									Lepra::uint8, 4, MACRO_CopyTest32Bit);
	}
	else
	{
		MACRO_DRAWSCANLINEALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pThis,
									unsigned, 1, MACRO_CopyTestUnit);
	}
}

void SoftwarePainter::DrawScanLine8BitXor(const Gradients2DUV* pGradients, 
										  Edge2DUV* pLeft, 
										  Edge2DUV* pRight,
										  const Lepra::Canvas* pTexture,
										  const Lepra::Canvas* /*pAlpha*/,
										  SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
							Lepra::uint8, 1, MACRO_CopyXorUnit);
}

void SoftwarePainter::DrawScanLine16BitXor(const Gradients2DUV* pGradients, 
										   Edge2DUV* pLeft, 
										   Edge2DUV* pRight,
										   const Lepra::Canvas* pTexture,
										   const Lepra::Canvas* /*pAlpha*/,
										   SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
							Lepra::uint16, 1, MACRO_CopyXorUnit);
}

void SoftwarePainter::DrawScanLine24BitXor(const Gradients2DUV* pGradients, 
										   Edge2DUV* pLeft, 
										   Edge2DUV* pRight,
										   const Lepra::Canvas* pTexture,
										   const Lepra::Canvas* /*pAlpha*/,
										   SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
							Lepra::uint8, 3, MACRO_CopyXor24Bit);
}

void SoftwarePainter::DrawScanLine32BitXor(const Gradients2DUV* pGradients, 
										   Edge2DUV* pLeft, 
										   Edge2DUV* pRight,
										   const Lepra::Canvas* pTexture,
										   const Lepra::Canvas* /*pAlpha*/,
										   SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
								Lepra::uint8, 4, MACRO_CopyXor32Bit);
	}
	else
	{
		MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
								unsigned, 1, MACRO_CopyXorUnit);
	}
}

void SoftwarePainter::DrawScanLine8BitAdd(const Gradients2DUV* pGradients, 
										  Edge2DUV* pLeft, 
										  Edge2DUV* pRight,
										  const Lepra::Canvas* pTexture,
										  const Lepra::Canvas* /*pAlpha*/,
										  SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
							Lepra::uint8, 1, MACRO_CopyAdd8Bit);
}

void SoftwarePainter::DrawScanLine15BitAdd(const Gradients2DUV* pGradients, 
										   Edge2DUV* pLeft, 
										   Edge2DUV* pRight,
										   const Lepra::Canvas* pTexture,
										   const Lepra::Canvas* /*pAlpha*/,
										   SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
							Lepra::uint16, 1, MACRO_CopyAdd15Bit);
}

void SoftwarePainter::DrawScanLine16BitAdd(const Gradients2DUV* pGradients, 
										   Edge2DUV* pLeft, 
										   Edge2DUV* pRight,
										   const Lepra::Canvas* pTexture,
										   const Lepra::Canvas* /*pAlpha*/,
										   SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
							Lepra::uint16, 1, MACRO_CopyAdd16Bit);
}

void SoftwarePainter::DrawScanLine24BitAdd(const Gradients2DUV* pGradients, 
										   Edge2DUV* pLeft, 
										   Edge2DUV* pRight,
										   const Lepra::Canvas* pTexture,
										   const Lepra::Canvas* /*pAlpha*/,
										   SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
							Lepra::uint8, 3, MACRO_CopyAdd24Bit);
}

void SoftwarePainter::DrawScanLine32BitAdd(const Gradients2DUV* pGradients, 
										   Edge2DUV* pLeft, 
										   Edge2DUV* pRight,
										   const Lepra::Canvas* pTexture,
										   const Lepra::Canvas* /*pAlpha*/,
										   SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
								Lepra::uint8, 4, MACRO_CopyAdd32Bit);
	}
	else
	{
		MACRO_DRAWSCANLINE(pGradients, pLeft, pRight, pTexture, pThis,
								Lepra::uint8, 4, MACRO_CopyAdd24Bit);
	}
}









































void SoftwarePainter::DrawScanLinePow28BitNormal(const Gradients2DUV* pGradients, 
												 Edge2DUV* pLeft, 
												 Edge2DUV* pRight,
												 const Lepra::Canvas* pTexture,
												 const Lepra::Canvas* /*pAlpha*/,
												 unsigned pShift, 
												 SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
								Lepra::uint8, 1, MACRO_CopyUnit);
}

void SoftwarePainter::DrawScanLinePow216BitNormal(const Gradients2DUV* pGradients, 
												  Edge2DUV* pLeft, 
												  Edge2DUV* pRight,
												  const Lepra::Canvas* pTexture,
												  const Lepra::Canvas* /*pAlpha*/,
												  unsigned pShift,
												  SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
								Lepra::uint16, 1, MACRO_CopyUnit);
}

void SoftwarePainter::DrawScanLinePow224BitNormal(const Gradients2DUV* pGradients, 
												  Edge2DUV* pLeft, 
												  Edge2DUV* pRight,
												  const Lepra::Canvas* pTexture,
												  const Lepra::Canvas* /*pAlpha*/,
												  unsigned pShift,
												  SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
								Lepra::uint8, 3, MACRO_Copy24Bit);
}

void SoftwarePainter::DrawScanLinePow232BitNormal(const Gradients2DUV* pGradients, 
												  Edge2DUV* pLeft, 
												  Edge2DUV* pRight,
												  const Lepra::Canvas* pTexture,
												  const Lepra::Canvas* /*pAlpha*/,
												  unsigned pShift,
												  SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
									Lepra::uint8, 4, MACRO_Copy32Bit);
	}
	else
	{
		MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
									unsigned, 1, MACRO_CopyUnit);
	}
}

void SoftwarePainter::DrawScanLinePow215BitBlend(const Gradients2DUV* pGradients, 
												 Edge2DUV* pLeft, 
												 Edge2DUV* pRight,
												 const Lepra::Canvas* pTexture,
												 const Lepra::Canvas* pAlpha,
												 unsigned pShift,
												 SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEPOW2ALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pShift, pThis, Lepra::uint16, 1, MACRO_CopyBlend15Bit);
}

void SoftwarePainter::DrawScanLinePow216BitBlend(const Gradients2DUV* pGradients, 
												 Edge2DUV* pLeft, 
												 Edge2DUV* pRight,
												 const Lepra::Canvas* pTexture,
												 const Lepra::Canvas* pAlpha,
												 unsigned pShift,
												 SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEPOW2ALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pShift, pThis,
								Lepra::uint16, 1, MACRO_CopyBlend16Bit);
}

void SoftwarePainter::DrawScanLinePow224BitBlend(const Gradients2DUV* pGradients, 
												 Edge2DUV* pLeft, 
												 Edge2DUV* pRight,
												 const Lepra::Canvas* pTexture,
												 const Lepra::Canvas* pAlpha,
												 unsigned pShift,
												 SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEPOW2ALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pShift, pThis,
								Lepra::uint8, 3, MACRO_CopyBlend24Bit);
}

void SoftwarePainter::DrawScanLinePow232BitBlend(const Gradients2DUV* pGradients, Edge2DUV* pLeft,
	Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* pAlpha,
	unsigned pShift, SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWSCANLINEPOW2ALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pShift, pThis,
			Lepra::uint8, 4, MACRO_CopyBlend32Bit);
	}
	else
	{
		MACRO_DRAWSCANLINEPOW2ALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pShift, pThis,
			Lepra::uint8, 4, MACRO_CopyBlend24Bit);
	}
}

void SoftwarePainter::DrawScanLinePow215BitBlend2(const Gradients2DUV* pGradients, Edge2DUV* pLeft,
	Edge2DUV* pRight, const Lepra::Canvas* pTexture, const Lepra::Canvas* /*pAlpha*/,
	unsigned pShift, SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
							Lepra::uint16, 1, MACRO_CopyBlend215Bit);
}

void SoftwarePainter::DrawScanLinePow216BitBlend2(const Gradients2DUV* pGradients, 
												  Edge2DUV* pLeft, 
												  Edge2DUV* pRight,
												  const Lepra::Canvas* pTexture,
												  const Lepra::Canvas* /*pAlpha*/,
												  unsigned pShift,
												  SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
							Lepra::uint16, 1, MACRO_CopyBlend216Bit);
}

void SoftwarePainter::DrawScanLinePow224BitBlend2(const Gradients2DUV* pGradients, 
												  Edge2DUV* pLeft, 
												  Edge2DUV* pRight,
												  const Lepra::Canvas* pTexture,
												  const Lepra::Canvas* /*pAlpha*/,
												  unsigned pShift,
												  SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
							Lepra::uint8, 3, MACRO_CopyBlend224Bit);
}

void SoftwarePainter::DrawScanLinePow232BitBlend2(const Gradients2DUV* pGradients, 
												  Edge2DUV* pLeft, 
												  Edge2DUV* pRight,
												  const Lepra::Canvas* pTexture,
												  const Lepra::Canvas* /*pAlpha*/,
												  unsigned pShift,
												  SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
								Lepra::uint8, 4, MACRO_CopyBlend232Bit);
	}
	else
	{
		MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
								Lepra::uint8, 4, MACRO_CopyBlend224Bit);
	}
}

void SoftwarePainter::DrawScanLinePow28BitTest(const Gradients2DUV* pGradients, 
											   Edge2DUV* pLeft, 
											   Edge2DUV* pRight,
											   const Lepra::Canvas* pTexture,
											   const Lepra::Canvas* pAlpha,
											   unsigned pShift,
											   SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEPOW2ALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pShift, pThis,
								Lepra::uint8, 1, MACRO_CopyTestUnit);
}

void SoftwarePainter::DrawScanLinePow216BitTest(const Gradients2DUV* pGradients, 
												Edge2DUV* pLeft, 
												Edge2DUV* pRight,
												const Lepra::Canvas* pTexture,
												const Lepra::Canvas* pAlpha,
												unsigned pShift,
												SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEPOW2ALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pShift, pThis,
								Lepra::uint16, 1, MACRO_CopyTestUnit);
}

void SoftwarePainter::DrawScanLinePow224BitTest(const Gradients2DUV* pGradients, 
												Edge2DUV* pLeft, 
												Edge2DUV* pRight,
												const Lepra::Canvas* pTexture,
												const Lepra::Canvas* pAlpha,
												unsigned pShift,
												SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEPOW2ALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pShift, pThis,
								Lepra::uint8, 3, MACRO_CopyTest24Bit);
}

void SoftwarePainter::DrawScanLinePow232BitTest(const Gradients2DUV* pGradients, 
												Edge2DUV* pLeft, 
												Edge2DUV* pRight,
												const Lepra::Canvas* pTexture,
												const Lepra::Canvas* pAlpha,
												unsigned pShift,
												SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWSCANLINEPOW2ALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pShift, pThis,
									Lepra::uint8, 4, MACRO_CopyTest32Bit);
	}
	else
	{
		MACRO_DRAWSCANLINEPOW2ALPHA(pGradients, pLeft, pRight, pTexture, pAlpha, pShift, pThis,
									unsigned, 1, MACRO_CopyTestUnit);
	}
}

void SoftwarePainter::DrawScanLinePow28BitXor(const Gradients2DUV* pGradients, 
											  Edge2DUV* pLeft, 
											  Edge2DUV* pRight,
											  const Lepra::Canvas* pTexture,
											  const Lepra::Canvas* /*pAlpha*/,
											  unsigned pShift,
											  SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
							Lepra::uint8, 1, MACRO_CopyXorUnit);
}

void SoftwarePainter::DrawScanLinePow216BitXor(const Gradients2DUV* pGradients, 
											   Edge2DUV* pLeft, 
											   Edge2DUV* pRight,
											   const Lepra::Canvas* pTexture,
											   const Lepra::Canvas* /*pAlpha*/,
											   unsigned pShift,
											   SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
							Lepra::uint16, 1, MACRO_CopyXorUnit);
}

void SoftwarePainter::DrawScanLinePow224BitXor(const Gradients2DUV* pGradients, 
											   Edge2DUV* pLeft, 
											   Edge2DUV* pRight,
											   const Lepra::Canvas* pTexture,
											   const Lepra::Canvas* /*pAlpha*/,
											   unsigned pShift,
											   SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
							Lepra::uint8, 3, MACRO_CopyXor24Bit);
}

void SoftwarePainter::DrawScanLinePow232BitXor(const Gradients2DUV* pGradients, 
											   Edge2DUV* pLeft, 
											   Edge2DUV* pRight,
											   const Lepra::Canvas* pTexture,
											   const Lepra::Canvas* /*pAlpha*/,
											   unsigned pShift,
											   SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
								Lepra::uint8, 4, MACRO_CopyXor32Bit);
	}
	else
	{
		MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
								unsigned, 1, MACRO_CopyXorUnit);
	}
}

void SoftwarePainter::DrawScanLinePow28BitAdd(const Gradients2DUV* pGradients, 
											  Edge2DUV* pLeft, 
											  Edge2DUV* pRight,
											  const Lepra::Canvas* pTexture,
											  const Lepra::Canvas* /*pAlpha*/,
											  unsigned pShift,
											  SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
							Lepra::uint8, 1, MACRO_CopyAdd8Bit);
}

void SoftwarePainter::DrawScanLinePow215BitAdd(const Gradients2DUV* pGradients, 
											   Edge2DUV* pLeft, 
											   Edge2DUV* pRight,
											   const Lepra::Canvas* pTexture,
											   const Lepra::Canvas* /*pAlpha*/,
											   unsigned pShift,
											   SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
							Lepra::uint16, 1, MACRO_CopyAdd15Bit);
}

void SoftwarePainter::DrawScanLinePow216BitAdd(const Gradients2DUV* pGradients, 
											   Edge2DUV* pLeft, 
											   Edge2DUV* pRight,
											   const Lepra::Canvas* pTexture,
											   const Lepra::Canvas* /*pAlpha*/,
											   unsigned pShift,
											   SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
							Lepra::uint16, 1, MACRO_CopyAdd16Bit);
}

void SoftwarePainter::DrawScanLinePow224BitAdd(const Gradients2DUV* pGradients, 
											   Edge2DUV* pLeft, 
											   Edge2DUV* pRight,
											   const Lepra::Canvas* pTexture,
											   const Lepra::Canvas* /*pAlpha*/,
											   unsigned pShift,
											   SoftwarePainter* pThis)
{
	MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
							Lepra::uint8, 3, MACRO_CopyAdd24Bit);
}

void SoftwarePainter::DrawScanLinePow232BitAdd(const Gradients2DUV* pGradients, 
											   Edge2DUV* pLeft, 
											   Edge2DUV* pRight,
											   const Lepra::Canvas* pTexture,
											   const Lepra::Canvas* /*pAlpha*/,
											   unsigned pShift,
											   SoftwarePainter* pThis)
{
	if (pThis->mIncrementalAlpha == true)
	{
		MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
								Lepra::uint8, 4, MACRO_CopyAdd32Bit);
	}
	else
	{
		MACRO_DRAWSCANLINEPOW2(pGradients, pLeft, pRight, pTexture, pShift, pThis,
								Lepra::uint8, 4, MACRO_CopyAdd24Bit);
	}
}

} // End namespace.
