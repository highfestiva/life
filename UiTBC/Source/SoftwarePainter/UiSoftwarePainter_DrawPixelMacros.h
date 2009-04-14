/*
	Class:  SoftwarePainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#ifndef SOFTWAREPAINTER_DRAWPIXELMACROS_H
#define SOFTWAREPAINTER_DRAWPIXELMACROS_H

/*
	"Draw pixel"-macros for all bit rates and render modes.
	First, regular pixel plotting.
*/

#define MACRO_DrawPixelNormal(destPtr, color) \
{ \
	*destPtr = color; \
}
#define MACRO_DrawPixel24BitNormal(destPtr, color) \
{ \
	destPtr[0] = color.mBlue; \
	destPtr[1] = color.mGreen; \
	destPtr[2] = color.mRed; \
}

#define MACRO_DrawPixel32BitNormal(destPtr, color) \
{ \
	destPtr[0] = color.mBlue; \
	destPtr[1] = color.mGreen; \
	destPtr[2] = color.mRed; \
 \
	if (color.mAlpha > destPtr[3]) \
		destPtr[3] = color.mAlpha; \
}

// A dummy macro, used to keep things compatible with the DrawShadedHLine functions...
#define MACRO_DrawPixel8BitFromRGB(destPtr, r, g, b, alpha) \
{ \
	alpha; \
	*lDest = 0; \
}

// r, g and b are assumed to be of type "Lepra::uint16",
// and in the range 0-255.
#define MACRO_DrawPixel15BitFromRGB(destPtr, r, g, b, alpha) \
{ \
	alpha; \
	*lDest = (Lepra::uint16)(((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3)); \
}

#define MACRO_DrawPixel16BitFromRGB(destPtr, r, g, b, alpha) \
{ \
	alpha; \
	*lDest = (Lepra::uint16)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)); \
}

#define MACRO_DrawPixel24BitFromRGB(destPtr, r, g, b, alpha) \
{ \
	alpha; \
	lDest[0] = (Lepra::uint8)b; \
	lDest[1] = (Lepra::uint8)g; \
	lDest[2] = (Lepra::uint8)r; \
}

#define MACRO_DrawPixel32BitFromRGBA(destPtr, r, g, b, a, alpha) \
{ \
	alpha; \
	lDest[0] = (Lepra::uint8)b; \
	lDest[1] = (Lepra::uint8)g; \
	lDest[2] = (Lepra::uint8)r; \
 \
	if (a > lDest[3]) \
		lDest[3] = (Lepra::uint8)a; \
}

// A macro used when drawing bitmaps.
#define MACRO_CopyUnit(destPtr, srcPtr, alpha) \
{ \
	alpha; \
	*destPtr = *srcPtr; \
}

#define MACRO_Copy24Bit(destPtr, srcPtr, alpha) \
{ \
	alpha; \
	destPtr[0] = srcPtr[0]; \
	destPtr[1] = srcPtr[1]; \
	destPtr[2] = srcPtr[2]; \
}

#define MACRO_Copy32Bit(destPtr, srcPtr, alpha) \
{ \
	alpha; \
	destPtr[0] = srcPtr[0]; \
	destPtr[1] = srcPtr[1]; \
	destPtr[2] = srcPtr[2]; \
  \
	if (srcPtr[3] > destPtr[3]) \
	{ \
		destPtr[3] = srcPtr[3]; \
	} \
}

/*
	Xor
*/

#define MACRO_DrawPixelXor(destPtr, color) \
{ \
	*destPtr ^= color; \
}
#define MACRO_DrawPixel24BitXor(destPtr, color) \
{ \
	destPtr[0] ^= color.mBlue; \
	destPtr[1] ^= color.mGreen; \
	destPtr[2] ^= color.mRed; \
}

#define MACRO_DrawPixel32BitXor(destPtr, color) \
{ \
	destPtr[0] ^= color.mBlue; \
	destPtr[1] ^= color.mGreen; \
	destPtr[2] ^= color.mRed; \
 \
	if (color.mAlpha > destPtr[3]) \
		destPtr[3] = color.mAlpha; \
}

// r, g and b are assumed to be of type "Lepra::uint16",
// and in the range 0-255.
#define MACRO_XorPixel15BitFromRGB(destPtr, r, g, b, alpha) \
{ \
	alpha; \
	*lDest ^= (Lepra::uint16)(((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3)); \
}

#define MACRO_XorPixel16BitFromRGB(destPtr, r, g, b, alpha) \
{ \
	alpha; \
	*lDest ^= (Lepra::uint16)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)); \
}

#define MACRO_XorPixel24BitFromRGB(destPtr, r, g, b, alpha) \
{ \
	alpha; \
	lDest[0] ^= (Lepra::uint8)b; \
	lDest[1] ^= (Lepra::uint8)g; \
	lDest[2] ^= (Lepra::uint8)r; \
}

#define MACRO_XorPixel32BitFromRGBA(destPtr, r, g, b, a, alpha) \
{ \
	alpha; \
	lDest[0] ^= (Lepra::uint8)b; \
	lDest[1] ^= (Lepra::uint8)g; \
	lDest[2] ^= (Lepra::uint8)r; \
 \
	if (a > destPtr[3]) \
		destPtr[3] = (Lepra::uint8)a; \
}

#define MACRO_CopyXorUnit(destPtr, srcPtr, alpha) \
{ \
	alpha; \
	*destPtr ^= *srcPtr; \
}

#define MACRO_CopyXor24Bit(destPtr, srcPtr, alpha) \
{ \
	alpha; \
	destPtr[0] ^= srcPtr[0]; \
	destPtr[1] ^= srcPtr[1]; \
	destPtr[2] ^= srcPtr[2]; \
}

#define MACRO_CopyXor32Bit(destPtr, srcPtr, alpha) \
{ \
	alpha; \
	destPtr[0] ^= srcPtr[0]; \
	destPtr[1] ^= srcPtr[1]; \
	destPtr[2] ^= srcPtr[2]; \
 \
	if (srcPtr[3] > destPtr[3]) \
		destPtr[3] = srcPtr[3]; \
}

/*
	Test
*/

#define MACRO_CopyTestUnit(destPtr, srcPtr, alphaPtr, alpha) \
{ \
	if (*alphaPtr >= alpha) \
	{ \
		*destPtr = *srcPtr; \
	} \
}

#define MACRO_CopyTest24Bit(destPtr, srcPtr, alphaPtr, alpha) \
{ \
	if (*alphaPtr >= alpha) \
	{ \
		destPtr[0] = srcPtr[0]; \
		destPtr[1] = srcPtr[1]; \
		destPtr[2] = srcPtr[2]; \
	} \
}

#define MACRO_CopyTest32Bit(destPtr, srcPtr, alphaPtr, alpha) \
{ \
	if (*alphaPtr >= alpha) \
	{ \
		destPtr[0] = srcPtr[0]; \
		destPtr[1] = srcPtr[1]; \
		destPtr[2] = srcPtr[2]; \
 \
		if (*alphaPtr > destPtr[3]) \
		{ \
			destPtr[3] = *alphaPtr; \
		} \
	} \
}

/*
	Blend
*/

#define MACRO_DrawPixel15BitBlend(destPtr, color) \
{ \
	Lepra::uint16 _lColor = *destPtr; \
	Lepra::uint16 _lDR = ((_lColor >> 10) & 31) << 3; \
	Lepra::uint16 _lDG = ((_lColor >> 5) & 31) << 3; \
	Lepra::uint16 _lDB = (_lColor & 31) << 3; \
 \
	Lepra::uint16 _lR = color.mR + ((_lDR * color.mA) >> 8); \
	Lepra::uint16 _lG = color.mG + ((_lDG * color.mA) >> 8); \
	Lepra::uint16 _lB = color.mB + ((_lDB * color.mA) >> 8); \
 \
	*destPtr = ((_lR >> 3) << 10) | ((_lG >> 3) << 5) | (_lB >> 3); \
}

#define MACRO_DrawPixel16BitBlend(destPtr, color) \
{ \
	Lepra::uint16 _lColor = *destPtr; \
	Lepra::uint16 _lDR = ((_lColor >> 11) & 31) << 3; \
	Lepra::uint16 _lDG = ((_lColor >> 5) & 63) << 2; \
	Lepra::uint16 _lDB = (_lColor & 31) << 3; \
 \
	Lepra::uint16 _lR = color.mR + ((_lDR * color.mA) >> 8); \
	Lepra::uint16 _lG = color.mG + ((_lDG * color.mA) >> 8); \
	Lepra::uint16 _lB = color.mB + ((_lDB * color.mA) >> 8); \
 \
	*destPtr = ((_lR >> 3) << 11) | ((_lG >> 2) << 5) | (_lB >> 3); \
}

#define MACRO_DrawPixelBlend(destPtr, color) \
{ \
	Lepra::uint16 _lDB = (Lepra::uint16)destPtr[0]; \
	Lepra::uint16 _lDG = (Lepra::uint16)destPtr[1]; \
	Lepra::uint16 _lDR = (Lepra::uint16)destPtr[2]; \
 \
	destPtr[0] = (Lepra::uint8)(color.mB + ((_lDB * color.mA) >> 8)); \
	destPtr[1] = (Lepra::uint8)(color.mG + ((_lDG * color.mA) >> 8)); \
	destPtr[2] = (Lepra::uint8)(color.mR + ((_lDR * color.mA) >> 8)); \
}

#define MACRO_DrawPixel32BitBlend(destPtr, color) \
{ \
	Lepra::uint16 _lDB = (Lepra::uint16)destPtr[0]; \
	Lepra::uint16 _lDG = (Lepra::uint16)destPtr[1]; \
	Lepra::uint16 _lDR = (Lepra::uint16)destPtr[2]; \
 \
	destPtr[0] = (Lepra::uint8)(color.mB + ((_lDB * color.mA) >> 8)); \
	destPtr[1] = (Lepra::uint8)(color.mG + ((_lDG * color.mA) >> 8)); \
	destPtr[2] = (Lepra::uint8)(color.mR + ((_lDR * color.mA) >> 8)); \
 \
	if ((Lepra::uint8)color.mA > destPtr[3]) \
		destPtr[3] = (Lepra::uint8)color.mA; \
}

#define MACRO_BlendPixel15Bit(destPtr, color, alpha) \
{ \
	Lepra::uint16 _lColor = *destPtr; \
	Lepra::uint16 _lDR = ((_lColor >> 10) & 31) << 3; \
	Lepra::uint16 _lDG = ((_lColor >> 5) & 31) << 3; \
	Lepra::uint16 _lDB = (_lColor & 31) << 3; \
 \
	Lepra::uint16 _lR = (((Lepra::uint16)color.mRed   * alpha) >> 8) + ((_lDR * (255 - alpha)) >> 8); \
	Lepra::uint16 _lG = (((Lepra::uint16)color.mGreen * alpha) >> 8) + ((_lDG * (255 - alpha)) >> 8); \
	Lepra::uint16 _lB = (((Lepra::uint16)color.mBlue  * alpha) >> 8) + ((_lDB * (255 - alpha)) >> 8); \
 \
	*destPtr = ((_lR >> 3) << 10) | ((_lG >> 3) << 5) | (_lB >> 3); \
}

#define MACRO_BlendPixel16Bit(destPtr, color, alpha) \
{ \
	Lepra::uint16 _lColor = *destPtr; \
	Lepra::uint16 _lDR = ((_lColor >> 11) & 31) << 3; \
	Lepra::uint16 _lDG = ((_lColor >> 5) & 63) << 2; \
	Lepra::uint16 _lDB = (_lColor & 31) << 3; \
 \
	Lepra::uint16 _lR = (((Lepra::uint16)color.mRed   * alpha) >> 8) + ((_lDR * (255 - alpha)) >> 8); \
	Lepra::uint16 _lG = (((Lepra::uint16)color.mGreen * alpha) >> 8) + ((_lDG * (255 - alpha)) >> 8); \
	Lepra::uint16 _lB = (((Lepra::uint16)color.mBlue  * alpha) >> 8) + ((_lDB * (255 - alpha)) >> 8); \
 \
	*destPtr = ((_lR >> 3) << 11) | ((_lG >> 2) << 5) | (_lB >> 3); \
}

#define MACRO_BlendPixel24Bit(destPtr, color, alpha) \
{ \
	Lepra::uint16 _lDB = (Lepra::uint16)destPtr[0]; \
	Lepra::uint16 _lDG = (Lepra::uint16)destPtr[1]; \
	Lepra::uint16 _lDR = (Lepra::uint16)destPtr[2]; \
 \
	Lepra::uint16 _lR = (((Lepra::uint16)color.mRed   * alpha) >> 8) + ((_lDR * (255 - alpha)) >> 8); \
	Lepra::uint16 _lG = (((Lepra::uint16)color.mGreen * alpha) >> 8) + ((_lDG * (255 - alpha)) >> 8); \
	Lepra::uint16 _lB = (((Lepra::uint16)color.mBlue  * alpha) >> 8) + ((_lDB * (255 - alpha)) >> 8); \
 \
	destPtr[0] = (Lepra::uint8)_lB; \
	destPtr[1] = (Lepra::uint8)_lG; \
	destPtr[2] = (Lepra::uint8)_lR; \
}

#define MACRO_BlendPixel32Bit(destPtr, color, alpha) \
{ \
	Lepra::uint16 _lDB = (Lepra::uint16)destPtr[0]; \
	Lepra::uint16 _lDG = (Lepra::uint16)destPtr[1]; \
	Lepra::uint16 _lDR = (Lepra::uint16)destPtr[2]; \
 \
	Lepra::uint16 _lR = (((Lepra::uint16)color.mRed   * alpha) >> 8) + ((_lDR * (255 - alpha)) >> 8); \
	Lepra::uint16 _lG = (((Lepra::uint16)color.mGreen * alpha) >> 8) + ((_lDG * (255 - alpha)) >> 8); \
	Lepra::uint16 _lB = (((Lepra::uint16)color.mBlue  * alpha) >> 8) + ((_lDB * (255 - alpha)) >> 8); \
 \
	destPtr[0] = (Lepra::uint8)_lB; \
	destPtr[1] = (Lepra::uint8)_lG; \
	destPtr[2] = (Lepra::uint8)_lR; \
	if (color.mAlpha > lDest[3]) \
		lDest[3] = color.mAlpha; \
}

// Assumes that there is a "pThis" pointer, to fit the number of
// arguments. Used in FillShadeRect().
#define MACRO_BlendPixel15BitFromRGB(destPtr, r, g, b, alpha) \
{ \
	Lepra::uint16 _lColor = *destPtr; \
	Lepra::uint16 _lDR = ((_lColor >> 10) & 31) << 3; \
	Lepra::uint16 _lDG = ((_lColor >> 5) & 31) << 3; \
	Lepra::uint16 _lDB = (_lColor & 31) << 3; \
 \
	Lepra::uint16 _lR = (Lepra::uint16)(((r * alpha) >> 8) + ((_lDR * (255 - alpha)) >> 8)); \
	Lepra::uint16 _lG = (Lepra::uint16)(((g * alpha) >> 8) + ((_lDG * (255 - alpha)) >> 8)); \
	Lepra::uint16 _lB = (Lepra::uint16)(((b * alpha) >> 8) + ((_lDB * (255 - alpha)) >> 8)); \
 \
	*destPtr = ((_lR >> 3) << 10) | ((_lG >> 3) << 5) | (_lB >> 3); \
}

//...used in FillShadeRect().
#define MACRO_BlendPixel16BitFromRGB(destPtr, r, g, b, alpha) \
{ \
	Lepra::uint16 _lColor = *destPtr; \
	Lepra::uint16 _lDR = ((_lColor >> 11) & 31) << 3; \
	Lepra::uint16 _lDG = ((_lColor >> 5) & 63) << 2; \
	Lepra::uint16 _lDB = (_lColor & 31) << 3; \
 \
	Lepra::uint16 _lR = (Lepra::uint16)(((r * alpha) >> 8) + ((_lDR * (255 - alpha)) >> 8)); \
	Lepra::uint16 _lG = (Lepra::uint16)(((g * alpha) >> 8) + ((_lDG * (255 - alpha)) >> 8)); \
	Lepra::uint16 _lB = (Lepra::uint16)(((b * alpha) >> 8) + ((_lDB * (255 - alpha)) >> 8)); \
 \
	*destPtr = ((_lR >> 3) << 11) | ((_lG >> 2) << 5) | (_lB >> 3); \
}

//...used in FillShadeRect().
#define MACRO_BlendPixel24BitFromRGB(destPtr, r, g, b, alpha) \
{ \
	Lepra::uint16 _lDB = (Lepra::uint16)destPtr[0]; \
	Lepra::uint16 _lDG = (Lepra::uint16)destPtr[1]; \
	Lepra::uint16 _lDR = (Lepra::uint16)destPtr[2]; \
 \
	Lepra::uint16 _lR = (Lepra::uint16)(((r * alpha) >> 8) + ((_lDR * (255 - alpha)) >> 8)); \
	Lepra::uint16 _lG = (Lepra::uint16)(((g * alpha) >> 8) + ((_lDG * (255 - alpha)) >> 8)); \
	Lepra::uint16 _lB = (Lepra::uint16)(((b * alpha) >> 8) + ((_lDB * (255 - alpha)) >> 8)); \
 \
	destPtr[0] = (Lepra::uint8)_lB; \
	destPtr[1] = (Lepra::uint8)_lG; \
	destPtr[2] = (Lepra::uint8)_lR; \
}

#define MACRO_BlendPixel32BitFromRGBA(destPtr, r, g, b, a, alpha) \
{ \
	Lepra::uint16 _lAlpha = (alpha * (Lepra::uint16)a) >> 8; \
	Lepra::uint16 _lDB = (Lepra::uint16)destPtr[0]; \
	Lepra::uint16 _lDG = (Lepra::uint16)destPtr[1]; \
	Lepra::uint16 _lDR = (Lepra::uint16)destPtr[2]; \
 \
	Lepra::uint16 _lR = (Lepra::uint16)(((r * _lAlpha) >> 8) + ((_lDR * (255 - _lAlpha)) >> 8)); \
	Lepra::uint16 _lG = (Lepra::uint16)(((g * _lAlpha) >> 8) + ((_lDG * (255 - _lAlpha)) >> 8)); \
	Lepra::uint16 _lB = (Lepra::uint16)(((b * _lAlpha) >> 8) + ((_lDB * (255 - _lAlpha)) >> 8)); \
 \
	destPtr[0] = (Lepra::uint8)_lB; \
	destPtr[1] = (Lepra::uint8)_lG; \
	destPtr[2] = (Lepra::uint8)_lR; \
 \
	if (a > destPtr[3]) \
		destPtr[3] = (Lepra::uint8)a; \
}

#define MACRO_CopyBlend15Bit(destPtr, srcPtr, alphaPtr, alpha) \
{ \
	Lepra::uint16 _lAlpha = (alpha * (Lepra::uint16)*alphaPtr) >> 8; \
 \
	Lepra::uint16 _lSColor = *srcPtr; \
	Lepra::uint16 _lSR = ((_lSColor >> 10) & 31) << 3; \
	Lepra::uint16 _lSG = ((_lSColor >> 5) & 31) << 3; \
	Lepra::uint16 _lSB = (_lSColor & 31) << 3; \
 \
	Lepra::uint16 _lDColor = *destPtr; \
	Lepra::uint16 _lDR = ((_lDColor >> 10) & 31) << 3; \
	Lepra::uint16 _lDG = ((_lDColor >> 5) & 31) << 3; \
	Lepra::uint16 _lDB = (_lDColor & 31) << 3; \
 \
	Lepra::uint16 _lR = ((_lSR * _lAlpha) >> 8) + ((_lDR * (255 - _lAlpha)) >> 8); \
	Lepra::uint16 _lG = ((_lSG * _lAlpha) >> 8) + ((_lDG * (255 - _lAlpha)) >> 8); \
	Lepra::uint16 _lB = ((_lSB * _lAlpha) >> 8) + ((_lDB * (255 - _lAlpha)) >> 8); \
 \
	*destPtr = ((_lR >> 3) << 10) | ((_lG >> 3) << 5) | (_lB >> 3); \
}

#define MACRO_CopyBlend16Bit(destPtr, srcPtr, alphaPtr, alpha) \
{ \
	Lepra::uint16 _lAlpha = (alpha * (Lepra::uint16)*alphaPtr) >> 8; \
 \
	Lepra::uint16 _lSColor = *srcPtr; \
	Lepra::uint16 _lSR = ((_lSColor >> 11) & 31) << 3; \
	Lepra::uint16 _lSG = ((_lSColor >> 5) & 63) << 2; \
	Lepra::uint16 _lSB = (_lSColor & 31) << 3; \
 \
	Lepra::uint16 _lDColor = *destPtr; \
	Lepra::uint16 _lDR = ((_lDColor >> 11) & 31) << 3; \
	Lepra::uint16 _lDG = ((_lDColor >> 5) & 63) << 2; \
	Lepra::uint16 _lDB = (_lDColor & 31) << 3; \
 \
	Lepra::uint16 _lR = ((_lSR * _lAlpha) >> 8) + ((_lDR * (255 - _lAlpha)) >> 8); \
	Lepra::uint16 _lG = ((_lSG * _lAlpha) >> 8) + ((_lDG * (255 - _lAlpha)) >> 8); \
	Lepra::uint16 _lB = ((_lSB * _lAlpha) >> 8) + ((_lDB * (255 - _lAlpha)) >> 8); \
 \
	*destPtr = ((_lR >> 3) << 11) | ((_lG >> 2) << 5) | (_lB >> 3); \
}

#define MACRO_CopyBlend24Bit(destPtr, srcPtr, alphaPtr, alpha) \
{ \
	Lepra::uint16 _lAlpha = (alpha * (Lepra::uint16)*alphaPtr) >> 8; \
 \
	Lepra::uint16 _lSB = srcPtr[0]; \
	Lepra::uint16 _lSG = srcPtr[1]; \
	Lepra::uint16 _lSR = srcPtr[2]; \
 \
	Lepra::uint16 _lDB = destPtr[0]; \
	Lepra::uint16 _lDG = destPtr[1]; \
	Lepra::uint16 _lDR = destPtr[2]; \
 \
	Lepra::uint16 _lR = ((_lSR * _lAlpha) >> 8) + ((_lDR * (255 - _lAlpha)) >> 8); \
	Lepra::uint16 _lG = ((_lSG * _lAlpha) >> 8) + ((_lDG * (255 - _lAlpha)) >> 8); \
	Lepra::uint16 _lB = ((_lSB * _lAlpha) >> 8) + ((_lDB * (255 - _lAlpha)) >> 8); \
 \
	destPtr[0] = (Lepra::uint8)_lB; \
	destPtr[1] = (Lepra::uint8)_lG; \
	destPtr[2] = (Lepra::uint8)_lR; \
}

#define MACRO_CopyBlend32Bit(destPtr, srcPtr, alphaPtr, alpha) \
{ \
	Lepra::uint16 _lAlpha = (alpha * (Lepra::uint16)*alphaPtr) >> 8; \
 \
	Lepra::uint16 _lSB = srcPtr[0]; \
	Lepra::uint16 _lSG = srcPtr[1]; \
	Lepra::uint16 _lSR = srcPtr[2]; \
 \
	Lepra::uint16 _lDB = destPtr[0]; \
	Lepra::uint16 _lDG = destPtr[1]; \
	Lepra::uint16 _lDR = destPtr[2]; \
 \
	Lepra::uint16 _lR = ((_lSR * _lAlpha) >> 8) + ((_lDR * (255 - _lAlpha)) >> 8); \
	Lepra::uint16 _lG = ((_lSG * _lAlpha) >> 8) + ((_lDG * (255 - _lAlpha)) >> 8); \
	Lepra::uint16 _lB = ((_lSB * _lAlpha) >> 8) + ((_lDB * (255 - _lAlpha)) >> 8); \
 \
	destPtr[0] = (Lepra::uint8)_lB; \
	destPtr[1] = (Lepra::uint8)_lG; \
	destPtr[2] = (Lepra::uint8)_lR; \
 \
	if ((Lepra::uint8)_lAlpha > destPtr[3]) \
		destPtr[3] = (Lepra::uint8)_lAlpha; \
}


#define MACRO_CopyBlend215Bit(destPtr, srcPtr, alpha) \
{ \
	Lepra::uint16 _lSColor = *srcPtr; \
	Lepra::uint16 _lSR = ((_lSColor >> 10) & 31) << 3; \
	Lepra::uint16 _lSG = ((_lSColor >> 5) & 31) << 3; \
	Lepra::uint16 _lSB = (_lSColor & 31) << 3; \
 \
	Lepra::uint16 _lDColor = *destPtr; \
	Lepra::uint16 _lDR = ((_lDColor >> 10) & 31) << 3; \
	Lepra::uint16 _lDG = ((_lDColor >> 5) & 31) << 3; \
	Lepra::uint16 _lDB = (_lDColor & 31) << 3; \
 \
	Lepra::uint16 _lR = ((_lSR * alpha) >> 8) + ((_lDR * (255 - alpha)) >> 8); \
	Lepra::uint16 _lG = ((_lSG * alpha) >> 8) + ((_lDG * (255 - alpha)) >> 8); \
	Lepra::uint16 _lB = ((_lSB * alpha) >> 8) + ((_lDB * (255 - alpha)) >> 8); \
 \
	*destPtr = ((_lR >> 3) << 10) | ((_lG >> 3) << 5) | (_lB >> 3); \
}

#define MACRO_CopyBlend216Bit(destPtr, srcPtr, alpha) \
{ \
	Lepra::uint16 _lSColor = *srcPtr; \
	Lepra::uint16 _lSR = ((_lSColor >> 11) & 31) << 3; \
	Lepra::uint16 _lSG = ((_lSColor >> 5) & 63) << 2; \
	Lepra::uint16 _lSB = (_lSColor & 31) << 3; \
 \
	Lepra::uint16 _lDColor = *destPtr; \
	Lepra::uint16 _lDR = ((_lDColor >> 11) & 31) << 3; \
	Lepra::uint16 _lDG = ((_lDColor >> 5) & 63) << 2; \
	Lepra::uint16 _lDB = (_lDColor & 31) << 3; \
 \
	Lepra::uint16 _lR = ((_lSR * alpha) >> 8) + ((_lDR * (255 - alpha)) >> 8); \
	Lepra::uint16 _lG = ((_lSG * alpha) >> 8) + ((_lDG * (255 - alpha)) >> 8); \
	Lepra::uint16 _lB = ((_lSB * alpha) >> 8) + ((_lDB * (255 - alpha)) >> 8); \
 \
	*destPtr = ((_lR >> 3) << 11) | ((_lG >> 2) << 5) | (_lB >> 3); \
}

#define MACRO_CopyBlend224Bit(destPtr, srcPtr, alpha) \
{ \
	Lepra::uint16 _lSB = srcPtr[0]; \
	Lepra::uint16 _lSG = srcPtr[1]; \
	Lepra::uint16 _lSR = srcPtr[2]; \
 \
	Lepra::uint16 _lDB = destPtr[0]; \
	Lepra::uint16 _lDG = destPtr[1]; \
	Lepra::uint16 _lDR = destPtr[2]; \
 \
	Lepra::uint16 _lR = ((_lSR * alpha) >> 8) + ((_lDR * (255 - alpha)) >> 8); \
	Lepra::uint16 _lG = ((_lSG * alpha) >> 8) + ((_lDG * (255 - alpha)) >> 8); \
	Lepra::uint16 _lB = ((_lSB * alpha) >> 8) + ((_lDB * (255 - alpha)) >> 8); \
 \
	destPtr[0] = (Lepra::uint8)_lB; \
	destPtr[1] = (Lepra::uint8)_lG; \
	destPtr[2] = (Lepra::uint8)_lR; \
}

#define MACRO_CopyBlend232Bit(destPtr, srcPtr, alpha) \
{ \
	Lepra::uint16 _lSB = srcPtr[0]; \
	Lepra::uint16 _lSG = srcPtr[1]; \
	Lepra::uint16 _lSR = srcPtr[2]; \
 \
	Lepra::uint16 _lDB = destPtr[0]; \
	Lepra::uint16 _lDG = destPtr[1]; \
	Lepra::uint16 _lDR = destPtr[2]; \
 \
	Lepra::uint16 _lR = ((_lSR * alpha) >> 8) + ((_lDR * (255 - alpha)) >> 8); \
	Lepra::uint16 _lG = ((_lSG * alpha) >> 8) + ((_lDG * (255 - alpha)) >> 8); \
	Lepra::uint16 _lB = ((_lSB * alpha) >> 8) + ((_lDB * (255 - alpha)) >> 8); \
 \
	destPtr[0] = (Lepra::uint8)_lB; \
	destPtr[1] = (Lepra::uint8)_lG; \
	destPtr[2] = (Lepra::uint8)_lR; \
 \
	if ((Lepra::uint8)alpha > destPtr[3]) \
		destPtr[3] = (Lepra::uint8)alpha; \
}
/*
	Add
*/

#define MACRO_DrawPixel8BitAdd(destPtr, color) \
{ \
	Lepra::uint16 _lCol = (Lepra::uint16)*destPtr + (Lepra::uint16)color; \
 	*destPtr = _lCol > 255 ? 255 : (Lepra::uint8)_lCol; \
}

#define MACRO_DrawPixel15BitAdd(destPtr, color) \
{ \
	Lepra::uint16 _lColor = *destPtr; \
	Lepra::uint16 _lDR = ((_lColor >> 10) & 31) + (Lepra::uint16)color.mRed; \
	Lepra::uint16 _lDG = ((_lColor >> 5) & 31) + (Lepra::uint16)color.mGreen; \
	Lepra::uint16 _lDB = (_lColor & 31) + (Lepra::uint16)color.mBlue; \
 \
	Lepra::uint16 _lR = _lDR > 31 ? 31 : _lDR; \
	Lepra::uint16 _lG = _lDG > 31 ? 31 : _lDG; \
	Lepra::uint16 _lB = _lDB > 31 ? 31 : _lDB; \
 \
	*destPtr = (_lR << 10) | (_lG << 5) | _lB; \
}

#define MACRO_DrawPixel16BitAdd(destPtr, color) \
{ \
	Lepra::uint16 _lColor = *destPtr; \
	Lepra::uint16 _lDR = ((_lColor >> 11) & 31) + (Lepra::uint16)color.mRed; \
	Lepra::uint16 _lDG = ((_lColor >> 5) & 63) + (Lepra::uint16)color.mGreen; \
	Lepra::uint16 _lDB = (_lColor & 31) + (Lepra::uint16)color.mBlue; \
 \
	Lepra::uint16 _lR = _lDR > 31 ? 31 : _lDR; \
	Lepra::uint16 _lG = _lDG > 63 ? 63 : _lDG; \
	Lepra::uint16 _lB = _lDB > 31 ? 31 : _lDB; \
 \
	*destPtr = (_lR << 11) | (_lG << 5) | _lB; \
}

#define MACRO_DrawPixelAdd(destPtr, color) \
{ \
	int _lDB = destPtr[0]; \
	int _lDG = destPtr[1]; \
	int _lDR = destPtr[2]; \
 \
 	_lDR += color.mRed; \
	_lDG += color.mGreen; \
	_lDB += color.mBlue; \
 \
	destPtr[0] = _lDB > 255 ? 255 : (Lepra::uint8)_lDB; \
	destPtr[1] = _lDG > 255 ? 255 : (Lepra::uint8)_lDG; \
	destPtr[2] = _lDR > 255 ? 255 : (Lepra::uint8)_lDR; \
}

#define MACRO_DrawPixelAdd32Bit(destPtr, color) \
{ \
	int _lDB = destPtr[0]; \
	int _lDG = destPtr[1]; \
	int _lDR = destPtr[2]; \
 \
 	_lDR += color.mRed; \
	_lDG += color.mGreen; \
	_lDB += color.mBlue; \
 \
	destPtr[0] = _lDB > 255 ? 255 : (Lepra::uint8)_lDB; \
	destPtr[1] = _lDG > 255 ? 255 : (Lepra::uint8)_lDG; \
	destPtr[2] = _lDR > 255 ? 255 : (Lepra::uint8)_lDR; \
 \
	if (color.mAlpha > destPtr[3]) \
		destPtr[3] = color.mAlpha; \
}

// Assumes that there is a "pThis" pointer, to fit the number of
// arguments. Used in FillShadeRect().
#define MACRO_AddPixel15BitFromRGB(destPtr, r, g, b, alpha) \
{ \
	alpha; \
	Lepra::uint16 _lColor = *destPtr; \
	Lepra::uint16 _lDR = (Lepra::uint16)(((_lColor >> 10) & 31) + (r >> 3)); \
	Lepra::uint16 _lDG = (Lepra::uint16)(((_lColor >> 5) & 31) + (g >> 3)); \
	Lepra::uint16 _lDB = (Lepra::uint16)((_lColor & 31) + (b >> 3)); \
 \
	Lepra::uint16 _lR = _lDR > 31 ? 31 : _lDR; \
	Lepra::uint16 _lG = _lDG > 31 ? 31 : _lDG; \
	Lepra::uint16 _lB = _lDB > 31 ? 31 : _lDB; \
 \
	*destPtr = (_lR << 10) | (_lG << 5) | _lB; \
}

#define MACRO_AddPixel16BitFromRGB(destPtr, r, g, b, alpha) \
{ \
	alpha; \
	Lepra::uint16 _lColor = *destPtr; \
	Lepra::uint16 _lDR = (Lepra::uint16)(((_lColor >> 11) & 31) + (r >> 3)); \
	Lepra::uint16 _lDG = (Lepra::uint16)(((_lColor >> 5) & 63) + (g >> 2)); \
	Lepra::uint16 _lDB = (Lepra::uint16)((_lColor & 31) + (b >> 3)); \
 \
	Lepra::uint16 _lR = _lDR > 31 ? 31 : _lDR; \
	Lepra::uint16 _lG = _lDG > 63 ? 63 : _lDG; \
	Lepra::uint16 _lB = _lDB > 31 ? 31 : _lDB; \
 \
	*destPtr = (_lR << 11) | (_lG << 5) | _lB; \
}

#define MACRO_AddPixel24BitFromRGB(destPtr, r, g, b, alpha) \
{ \
	alpha; \
	Lepra::uint16 _lDB = (Lepra::uint16)(destPtr[0] + b); \
	Lepra::uint16 _lDG = (Lepra::uint16)(destPtr[1] + g); \
	Lepra::uint16 _lDR = (Lepra::uint16)(destPtr[2] + r); \
 \
	destPtr[0] = _lDB > 255? 255 : (Lepra::uint8)_lDB; \
	destPtr[1] = _lDG > 255? 255 : (Lepra::uint8)_lDG; \
	destPtr[2] = _lDR > 255? 255 : (Lepra::uint8)_lDR; \
}

#define MACRO_AddPixel32BitFromRGBA(destPtr, r, g, b, a, alpha) \
{ \
	alpha; \
	Lepra::uint16 _lDB = (Lepra::uint16)(destPtr[0] + b); \
	Lepra::uint16 _lDG = (Lepra::uint16)(destPtr[1] + g); \
	Lepra::uint16 _lDR = (Lepra::uint16)(destPtr[2] + r); \
 \
	destPtr[0] = _lDB > 255? 255 : (Lepra::uint8)_lDB; \
	destPtr[1] = _lDG > 255? 255 : (Lepra::uint8)_lDG; \
	destPtr[2] = _lDR > 255? 255 : (Lepra::uint8)_lDR; \
 \
	if ((Lepra::uint8)a > destPtr[3]) \
		destPtr[3] = (Lepra::uint8)a; \
}

#define MACRO_CopyAdd8Bit(destPtr, srcPtr, alpha) \
{ \
	alpha; \
	Lepra::uint16 _lColor = (Lepra::uint16)*destPtr + (Lepra::uint16)*srcPtr; \
 	*destPtr = (Lepra::uint8)(_lColor > 255 ? 255 : _lColor); \
}

#define MACRO_CopyAdd15Bit(destPtr, srcPtr, alpha) \
{ \
	alpha; \
	Lepra::uint16 _lSColor = *srcPtr; \
	Lepra::uint16 _lDColor = *destPtr; \
 \
	Lepra::uint16 _lSR = ((_lSColor >> 10) & 31); \
	Lepra::uint16 _lSG = ((_lSColor >> 5) & 31); \
	Lepra::uint16 _lSB = (_lSColor & 31); \
 \
	Lepra::uint16 _lDR = ((_lDColor >> 10) & 31) + _lSR; \
	Lepra::uint16 _lDG = ((_lDColor >> 5) & 31) + _lSG; \
	Lepra::uint16 _lDB = (_lDColor & 31) + _lSB; \
 \
	Lepra::uint16 _lR = _lDR > 31 ? 31 : _lDR; \
	Lepra::uint16 _lG = _lDG > 31 ? 31 : _lDG; \
	Lepra::uint16 _lB = _lDB > 31 ? 31 : _lDB; \
 \
	*destPtr = (_lR << 10) | (_lG << 5) | _lB; \
}

#define MACRO_CopyAdd16Bit(destPtr, srcPtr, alpha) \
{ \
	alpha; \
	Lepra::uint16 _lSColor = *srcPtr; \
	Lepra::uint16 _lDColor = *destPtr; \
 \
	Lepra::uint16 _lSR = ((_lSColor >> 11) & 31); \
	Lepra::uint16 _lSG = ((_lSColor >> 5) & 63); \
	Lepra::uint16 _lSB = (_lSColor & 31); \
 \
	Lepra::uint16 _lDR = ((_lDColor >> 11) & 31) + _lSR; \
	Lepra::uint16 _lDG = ((_lDColor >> 5) & 63) + _lSG; \
	Lepra::uint16 _lDB = (_lDColor & 31) + _lSB; \
 \
	Lepra::uint16 _lR = _lDR > 31 ? 31 : _lDR; \
	Lepra::uint16 _lG = _lDG > 63 ? 63 : _lDG; \
	Lepra::uint16 _lB = _lDB > 31 ? 31 : _lDB; \
 \
	*destPtr = (_lR << 11) | (_lG << 5) | _lB; \
}

#define MACRO_CopyAdd24Bit(destPtr, srcPtr, alpha) \
{ \
	alpha; \
	int _lDB = (int)destPtr[0] + (int)srcPtr[0]; \
	int _lDG = (int)destPtr[1] + (int)srcPtr[1]; \
	int _lDR = (int)destPtr[2] + (int)srcPtr[2]; \
 \
	destPtr[0] = _lDB > 255 ? 255 : (Lepra::uint8)_lDB; \
	destPtr[1] = _lDG > 255 ? 255 : (Lepra::uint8)_lDG; \
	destPtr[2] = _lDR > 255 ? 255 : (Lepra::uint8)_lDR; \
}

#define MACRO_CopyAdd32Bit(destPtr, srcPtr, alpha) \
{ \
	alpha; \
	int _lDB = (int)destPtr[0] + (int)srcPtr[0]; \
	int _lDG = (int)destPtr[1] + (int)srcPtr[1]; \
	int _lDR = (int)destPtr[2] + (int)srcPtr[2]; \
 \
	destPtr[0] = _lDB > 255 ? 255 : (Lepra::uint8)_lDB; \
	destPtr[1] = _lDG > 255 ? 255 : (Lepra::uint8)_lDG; \
	destPtr[2] = _lDR > 255 ? 255 : (Lepra::uint8)_lDR; \
 \
	if (srcPtr[3] > destPtr[3]) \
		destPtr[3] = srcPtr[3]; \
}

#endif