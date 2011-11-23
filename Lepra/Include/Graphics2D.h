
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games




#pragma once

#include "GammaLookup.h"
#include "Math.h"



namespace Lepra
{



class PixelCoord
{
public:
	PixelCoord() :
		x(0),
		y(0)
	{
	}

	PixelCoord(int pX, int pY) :
		x(pX),
		y(pY)
	{
	}

	PixelCoord(const PixelCoord& pCoords) :
		x(pCoords.x),
		y(pCoords.y)
	{
	}

	inline PixelCoord operator = (const PixelCoord& pCoords);
	inline PixelCoord operator += (const PixelCoord& pCoords);
	inline PixelCoord operator -= (const PixelCoord& pCoords);
	inline PixelCoord operator + (const PixelCoord& pCoords) const;
	inline PixelCoord operator - (const PixelCoord& pCoords) const;
	inline PixelCoord operator * (int pFactor) const;
	inline PixelCoord operator / (int pDividend) const;

	inline bool operator == (const PixelCoord& pCoords) const;
	inline bool operator != (const PixelCoord& pCoords) const;

	inline float GetDistance(const PixelCoord& pCoords) const;

	int x, y;
};

PixelCoord PixelCoord::operator = (const PixelCoord& pCoords)
{
	x = pCoords.x;
	y = pCoords.y;
	return *this;
}

PixelCoord PixelCoord::operator += (const PixelCoord& pCoords)
{
	x += pCoords.x;
	y += pCoords.y;
	return *this;
}

PixelCoord PixelCoord::operator -= (const PixelCoord& pCoords)
{
	x -= pCoords.x;
	y -= pCoords.y;
	return *this;
}

PixelCoord PixelCoord::operator + (const PixelCoord& pCoords) const
{
	PixelCoord lTemp(*this);
	lTemp += pCoords;
	return lTemp;
}

PixelCoord PixelCoord::operator - (const PixelCoord& pCoords) const
{
	PixelCoord lTemp(*this);
	lTemp -= pCoords;
	return lTemp;
}

PixelCoord PixelCoord::operator*(int pFactor) const
{
	PixelCoord lTemp(*this);
	lTemp.x *= pFactor;
	lTemp.y *= pFactor;
	return lTemp;
}

PixelCoord PixelCoord::operator/(int pDividend) const
{
	PixelCoord lTemp(*this);
	lTemp.x /= pDividend;
	lTemp.y /= pDividend;
	return lTemp;
}

bool PixelCoord::operator == (const PixelCoord& pCoords) const
{
	return (x == pCoords.x && y == pCoords.y);
}

bool PixelCoord::operator != (const PixelCoord& pCoords) const
{
	return (x != pCoords.x || y != pCoords.y);
}

float PixelCoord::GetDistance(const PixelCoord& pCoords) const
{
	int lDX = x-pCoords.x;
	int lDY = y-pCoords.y;
	return (::sqrt((float)(lDX*lDX + lDY*lDY)));
}



class PixelRect
{
public:
	inline PixelRect();
	inline PixelRect(const PixelRect& pRect);
	inline PixelRect(const PixelCoord& pTopLeft, const PixelCoord& pBottomRight);
	inline PixelRect(int pLeft, int pTop, int pRight, int pBottom);

	inline int GetWidth() const;
	inline int GetHeight() const;
	inline PixelCoord GetSize() const;

	inline int GetCenterX() const;
	inline int GetCenterY() const;

	inline bool IsInside(int x, int y) const;
	inline bool Overlap(const PixelRect& pRect) const;
	inline bool VerticalOverlap(const PixelRect& pRect) const;
	inline bool HorizontalOverlap(const PixelRect& pRect) const;

	inline PixelRect GetOverlap(const PixelRect& pRect) const;

	inline void Shrink(int pNumPixels);
	inline void Enlarge(int pNumPixels);
	inline void Offset(int x, int y);
	inline void Set(int pLeft, int pTop, int pRight, int pBottom);

	inline bool operator==(const PixelRect& pRect) const;

	int mTop;
	int mBottom;
	int mLeft;
	int mRight;
};

PixelRect::PixelRect()
{
	mTop = 0;
	mBottom = 0;
	mLeft = 0;
	mRight = 0;
}

PixelRect::PixelRect(const PixelRect& pRect) :
	mTop(pRect.mTop),
	mBottom(pRect.mBottom),
	mLeft(pRect.mLeft),
	mRight(pRect.mRight)
{
}

PixelRect::PixelRect(const PixelCoord& pTopLeft, const PixelCoord& pBottomRight) :
	mTop(pTopLeft.y),
	mBottom(pBottomRight.y),
	mLeft(pTopLeft.x),
	mRight(pBottomRight.x)
{
}

PixelRect::PixelRect(int pLeft, int pTop, int pRight, int pBottom) :
	mTop(pTop),
	mBottom(pBottom),
	mLeft(pLeft),
	mRight(pRight)
{
}

int PixelRect::GetWidth() const
{
	return (mRight - mLeft);
}

int PixelRect::GetHeight() const
{
	return (mBottom - mTop);
}

PixelCoord PixelRect::GetSize() const
{
	return (PixelCoord(GetWidth(), GetHeight()));
}

int PixelRect::GetCenterX() const
{
	return mLeft + (GetWidth() >> 1);
}

int PixelRect::GetCenterY() const
{
	return mTop + (GetHeight() >> 1);
}

bool PixelRect::IsInside(int x, int y) const
{
	if (x >= mLeft && x < mRight && y >= mTop && y < mBottom)
	{
		return true;
	}

	return false;
}

bool PixelRect::Overlap(const PixelRect& pRect) const
{
	if (pRect.mLeft > mRight ||
	   pRect.mRight < mLeft ||
	   pRect.mTop > mBottom ||
	   pRect.mBottom < mTop)
	{
		return false;
	}

	return true;
}

bool PixelRect::VerticalOverlap(const PixelRect& pRect) const
{
	if (pRect.mTop > mBottom ||
	   pRect.mBottom < mTop)
	{
		return false;
	}

	return true;
}

bool PixelRect::HorizontalOverlap(const PixelRect& pRect) const
{
	if (pRect.mLeft > mRight ||
	   pRect.mRight < mLeft)
	{
		return false;
	}

	return true;
}

PixelRect PixelRect::GetOverlap(const PixelRect& pRect) const
{
	PixelRect lRect(*this);
	if (pRect.mLeft > lRect.mLeft)
	{
		lRect.mLeft = pRect.mLeft;
	}
	if (pRect.mRight < lRect.mRight)
	{
		lRect.mRight = pRect.mRight;
	}
	if (pRect.mTop > lRect.mTop)
	{
		lRect.mTop = pRect.mTop;
	}
	if (pRect.mBottom < lRect.mBottom)
	{
		lRect.mBottom = pRect.mBottom;
	}
	return lRect;
}

void PixelRect::Set(int pLeft, int pTop, int pRight, int pBottom)
{
	mTop = pTop;
	mBottom = pBottom;
	mLeft = pLeft;
	mRight = pRight;
}

bool PixelRect::operator==(const PixelRect& pRect) const
{
	return (mTop == pRect.mTop && mBottom == pRect.mBottom &&
		mLeft == pRect.mLeft && mRight == pRect.mRight);
}

void PixelRect::Shrink(int pNumPixels)
{
	mTop		+= pNumPixels;
	mBottom	-= pNumPixels;
	mLeft		+= pNumPixels;
	mRight	-= pNumPixels;
}

void PixelRect::Enlarge(int pNumPixels)
{
	Shrink(-pNumPixels);
}

void PixelRect::Offset(int x, int y)
{
	mTop		+= y;
	mBottom	+= y;
	mLeft		+= x;
	mRight	+= x;
}


#define CLEAR_COLOR	Color(0, 0, 0, 0);
#define BLACK		Color(0, 0, 0)
#define OFF_BLACK	Color(1, 1, 1)
#define WHITE		Color(255, 255, 255)
#define GRAY		Color(128, 128, 128)
#define RED		Color(255, 0, 0)
#define GREEN		Color(0, 255, 0)
#define BLUE		Color(0, 0, 255)
#define YELLOW		Color(255, 255, 0)
#define MAGENTA		Color(255, 0, 255)
#define PURPLE		Color(128, 0, 255)
#define CYAN		Color(0, 255, 255)
#define ORANGE		Color(255, 150, 0)
#define BROWN		Color(128, 80, 0)
#define PINK		Color(255, 128, 128)

#define DARK_GRAY	Color(64, 64, 64)
#define DARK_RED	Color(128, 0, 0)
#define DARK_GREEN	Color(0, 128, 0)
#define DARK_BLUE	Color(0, 0, 128)
#define DARK_YELLOW	Color(128, 128, 0)
#define DARK_MAGENTA	Color(128, 0, 128)
#define DARK_CYAN	Color(0, 128, 128)

#define LIGHT_GRAY	Color(192, 192, 192)
#define LIGHT_RED	Color(255, 64, 64)
#define LIGHT_GREEN	Color(64, 255, 64)
#define LIGHT_BLUE	Color(64, 64, 255)
#define LIGHT_YELLOW	Color(255, 255, 64)
#define LIGHT_MAGENTA	Color(255, 64, 255)
#define LIGHT_CYAN	Color(64, 255, 255)

class Color
{
public:
	
	inline Color();
	inline Color(const Color& pColor);
	inline Color(uint8 pRed, uint8 pGreen, uint8 pBlue);
	inline Color(uint8 pRed, uint8 pGreen, uint8 pBlue, uint8 pColorIndex);

	// Interpolation constructor.
	inline Color(const Color& pColor1, const Color& pColor2, float t);

	// Set()-overloads for all standard types. This makes life easier when setting the color,
	// because you don't have to typecast all variables to uint8 all the time.
	// The float overloads assumes that the rgb values are in the range [0, 1].
	// All integer types in the range [0, 255].
	inline void Set(int8 pRed, int8 pGreen, int8 pBlue, int8 pColorIndex);
	inline void Set(uint8 pRed, uint8 pGreen, uint8 pBlue, uint8 pColorIndex);
	inline void Set(int16   pRed,  int16   pGreen,  int16   pBlue,  int16   pColorIndex);
	inline void Set(uint16  pRed, uint16  pGreen, uint16  pBlue, uint16  pColorIndex);
	inline void Set(int32 pRed, int32 pGreen, int32 pBlue, int32 pColorIndex);
	inline void Set(uint32 pRed, uint32 pGreen, uint32 pBlue, uint32 pColorIndex);
	inline void Set(int64   pRed,  int64   pGreen,  int64   pBlue,  int64   pColorIndex);
	inline void Set(uint64  pRed, uint64  pGreen, uint64  pBlue, uint64  pColorIndex);
	inline void Set(float pRed,  float pGreen,  float pBlue,  float pColorIndex);

	inline float GetRf() const;
	inline float GetGf() const;
	inline float GetBf() const;
	inline float GetAf() const;
	inline uint32 To32() const;

	inline Color operator = (const Color& pColor);
	
	// The following operators will operate on all color components 
	// (Red, Green, Blue) but NOT(!) the alpha channel.
	inline Color operator += (const Color& pColor);
	inline Color operator + (const Color& pColor) const;
	inline Color operator -= (const Color& pColor);
	inline Color operator - (const Color& pColor) const;

	inline Color operator += (int pValue);
	inline Color operator +  (int pValue) const;
	inline Color operator -= (int pValue);
	inline Color operator -  (int pValue) const;
	inline Color operator *= (int pScalar);
	inline Color operator *  (int pScalar) const;
	inline Color operator /= (int pScalar);
	inline Color operator /  (int pScalar) const;
	inline Color operator *= (float pScalar);
	inline Color operator *  (float pScalar) const;
	inline Color operator /= (float pScalar);
	inline Color operator /  (float pScalar) const;

	inline bool operator==(const Color& pOther) const;
	inline bool operator!=(const Color& pOther) const;

	inline int SumRgb() const;

	uint8 mRed;
	uint8 mGreen;
	uint8 mBlue;
	uint8 mAlpha;	// Only used in 8-bit color mode.

private:
	inline int Max(int v1, int v2) const;
	inline int Min(int v1, int v2) const;
};

Color::Color()
{
	mRed   = 0;
	mGreen = 0;
	mBlue  = 0;
	mAlpha = 255;	// Treat as alpha.
}

Color::Color(const Color& pColor)
{
	mRed   = pColor.mRed;
	mGreen = pColor.mGreen;
	mBlue  = pColor.mBlue;
	mAlpha = pColor.mAlpha;
}

Color::Color(uint8 pRed, uint8 pGreen, uint8 pBlue)
{
	mRed   = pRed;
	mGreen = pGreen;
	mBlue  = pBlue;
	mAlpha = 255;
}

Color::Color(uint8 pRed, uint8 pGreen, uint8 pBlue, uint8 pColorIndex)
{
	mRed   = pRed;
	mGreen = pGreen;
	mBlue  = pBlue;
	mAlpha = pColorIndex;
}

Color::Color(const Color& pColor1, const Color& pColor2, float t)
{
	const float r1 = GammaLookup::GammaToLinearFloat(pColor1.mRed);
	const float g1 = GammaLookup::GammaToLinearFloat(pColor1.mGreen);
	const float b1 = GammaLookup::GammaToLinearFloat(pColor1.mBlue);
	const float r2 = GammaLookup::GammaToLinearFloat(pColor2.mRed);
	const float g2 = GammaLookup::GammaToLinearFloat(pColor2.mGreen);
	const float b2 = GammaLookup::GammaToLinearFloat(pColor2.mBlue);

	mRed   = GammaLookup::LinearToGamma((int)Math::Lerp(r1, r2, t));
	mGreen = GammaLookup::LinearToGamma((int)Math::Lerp(g1, g2, t));
	mBlue  = GammaLookup::LinearToGamma((int)Math::Lerp(b1, b2, t));
	mAlpha = (uint8)Math::Lerp((float)pColor1.mAlpha, (1/255.0f)*pColor1.mAlpha*pColor2.mAlpha, t);
}

int Color::Max(int v1, int v2) const
{
	return (v1 > v2 ? v1 : v2);
}

int Color::Min(int v1, int v2) const
{
	return (v1 < v2 ? v1 : v2);
}

void Color::Set(int8 pRed, int8 pGreen, int8 pBlue, int8 pColorIndex)
{
	Set((uint8)pRed, (uint8)pGreen, (uint8)pBlue, (uint8)pColorIndex);
}

void Color::Set(uint8 pRed, uint8 pGreen, uint8 pBlue, uint8 pColorIndex)
{
	mRed	= pRed;
	mGreen	= pGreen;
	mBlue	= pBlue;
	mAlpha	= pColorIndex;
}

void Color::Set(int16 pRed, int16 pGreen, int16 pBlue, int16 pColorIndex)
{
	Set((uint8)pRed, (uint8)pGreen, (uint8)pBlue, (uint8)pColorIndex);
}

void Color::Set(uint16 pRed, uint16 pGreen, uint16 pBlue, uint16 pColorIndex)
{
	Set((uint8)pRed, (uint8)pGreen, (uint8)pBlue, (uint8)pColorIndex);
}

void Color::Set(int32 pRed, int32 pGreen, int32 pBlue, int32 pColorIndex)
{
	Set((uint8)pRed, (uint8)pGreen, (uint8)pBlue, (uint8)pColorIndex);
}

void Color::Set(uint32 pRed, uint32 pGreen, uint32 pBlue, uint32 pColorIndex)
{
	Set((uint8)pRed, (uint8)pGreen, (uint8)pBlue, (uint8)pColorIndex);
}

void Color::Set(int64 pRed, int64 pGreen, int64 pBlue, int64 pColorIndex)
{
	Set((uint8)pRed, (uint8)pGreen, (uint8)pBlue, (uint8)pColorIndex);
}

void Color::Set(uint64 pRed, uint64 pGreen, uint64 pBlue, uint64 pColorIndex)
{
	Set((uint8)pRed, (uint8)pGreen, (uint8)pBlue, (uint8)pColorIndex);
}

void Color::Set(float pRed, float pGreen, float pBlue, float pColorIndex)
{
	Set((uint8)(pRed * 255.0f), 
		(uint8)(pGreen * 255.0f), 
		(uint8)(pBlue * 255.0f), 
		(uint8)(pColorIndex * 255.0f)); // Color index treated as alpha channel.
}

float Color::GetRf() const
{
	return (mRed/255.0f);
}

float Color::GetGf() const
{
	return (mGreen/255.0f);
}

float Color::GetBf() const
{
	return (mBlue/255.0f);
}

float Color::GetAf() const
{
	return (mAlpha/255.0f);
}

uint32 Color::To32() const
{
	typedef uint32 u;
	return ((((u)mRed)<<24) + (((u)mGreen)<<16) + (((u)mBlue)<<8) + (u)mAlpha);
}

Color Color::operator = (const Color& pColor)
{
	mRed   = pColor.mRed;
	mGreen = pColor.mGreen;
	mBlue  = pColor.mBlue;
	mAlpha = pColor.mAlpha;
	return *this;
}

Color Color::operator += (const Color& pColor)
{
	mRed   = (uint8)Min(255, (int)mRed   + (int)pColor.mRed);
	mGreen = (uint8)Min(255, (int)mGreen + (int)pColor.mGreen);
	mBlue  = (uint8)Min(255, (int)mBlue  + (int)pColor.mBlue);
	mAlpha = (uint8)Min(255, (int)mAlpha + (int)pColor.mAlpha);
	return *this;
}

Color Color::operator + (const Color& pColor) const
{
	return Color((uint8)Min(255, (int)mRed   + (int)pColor.mRed),
		         (uint8)Min(255, (int)mGreen + (int)pColor.mGreen),
				 (uint8)Min(255, (int)mBlue  + (int)pColor.mBlue),
				 (uint8)Min(255, (int)mAlpha + (int)pColor.mAlpha));
}

Color Color::operator -= (const Color& pColor)
{
	mRed   = (uint8)Max(0, (int)mRed   - (int)pColor.mRed);
	mGreen = (uint8)Max(0, (int)mGreen - (int)pColor.mGreen);
	mBlue  = (uint8)Max(0, (int)mBlue  - (int)pColor.mBlue);
	mAlpha = (uint8)Max(0, (int)mAlpha - (int)pColor.mAlpha);
	return *this;
}

Color Color::operator - (const Color& pColor) const
{
	return Color((uint8)Max(0, (int)mRed   - (int)pColor.mRed),
		         (uint8)Max(0, (int)mGreen - (int)pColor.mGreen),
				 (uint8)Max(0, (int)mBlue  - (int)pColor.mBlue),
				 (uint8)Max(0, (int)mAlpha - (int)pColor.mAlpha));
}

Color Color::operator += (int pValue)
{
	mRed   = (uint8)Max(0, Min(255, (int)mRed   + pValue));
	mGreen = (uint8)Max(0, Min(255, (int)mGreen + pValue));
	mBlue  = (uint8)Max(0, Min(255, (int)mBlue  + pValue));
	mAlpha = (uint8)Max(0, Min(255, (int)mAlpha + pValue));
	return *this;
}

Color Color::operator + (int pValue) const
{
	return Color((uint8)Max(0, Min(255, (int)mRed   + pValue)),
		         (uint8)Max(0, Min(255, (int)mGreen + pValue)),
				 (uint8)Max(0, Min(255, (int)mBlue  + pValue)),
				 mAlpha);
}

Color Color::operator -= (int pValue)
{
	mRed   = (uint8)Min(255, Max(0, (int)mRed   - pValue));
	mGreen = (uint8)Min(255, Max(0, (int)mGreen - pValue));
	mBlue  = (uint8)Min(255, Max(0, (int)mBlue  - pValue));
	return *this;
}

Color Color::operator - (int pValue) const
{
	return Color((uint8)Min(255, Max(0, (int)mRed   - pValue)),
		         (uint8)Min(255, Max(0, (int)mGreen - pValue)),
				 (uint8)Min(255, Max(0, (int)mBlue  - pValue)),
				 mAlpha);
}

Color Color::operator *= (int pScalar)
{
	mRed   = (uint8)Max(0, Min(255, (int)mRed   * pScalar));
	mGreen = (uint8)Max(0, Min(255, (int)mGreen * pScalar));
	mBlue  = (uint8)Max(0, Min(255, (int)mBlue  * pScalar));
	return *this;
}

Color Color::operator * (int pScalar) const
{
	return Color((uint8)Max(0, Min(255, (int)mRed   * pScalar)),
		         (uint8)Max(0, Min(255, (int)mGreen * pScalar)),
				 (uint8)Max(0, Min(255, (int)mBlue  * pScalar)),
				 mAlpha);
}

Color Color::operator /= (int pScalar)
{
	mRed   = (uint8)Max(0, Min(255, (int)mRed   / pScalar));
	mGreen = (uint8)Max(0, Min(255, (int)mGreen / pScalar));
	mBlue  = (uint8)Max(0, Min(255, (int)mBlue  / pScalar));
	return *this;
}

Color Color::operator / (int pScalar) const
{
	return Color((uint8)Max(0, Min(255, (int)mRed   / pScalar)),
		         (uint8)Max(0, Min(255, (int)mGreen / pScalar)),
				 (uint8)Max(0, Min(255, (int)mBlue  / pScalar)),
				 mAlpha);
}

Color Color::operator *= (float pScalar)
{
	mRed   = (uint8)Max(0, Min(255, (int)((float)mRed   * pScalar)));
	mGreen = (uint8)Max(0, Min(255, (int)((float)mGreen * pScalar)));
	mBlue  = (uint8)Max(0, Min(255, (int)((float)mBlue  * pScalar)));
	return *this;
}

Color Color::operator * (float pScalar) const
{
	return Color((uint8)Max(0, Min(255, (int)((float)mRed   * pScalar))),
		         (uint8)Max(0, Min(255, (int)((float)mGreen * pScalar))),
				 (uint8)Max(0, Min(255, (int)((float)mBlue  * pScalar))),
				 mAlpha);
}

Color Color::operator /= (float pScalar)
{
	mRed   = (uint8)Max(0, Min(255, (int)((float)mRed   / pScalar)));
	mGreen = (uint8)Max(0, Min(255, (int)((float)mGreen / pScalar)));
	mBlue  = (uint8)Max(0, Min(255, (int)((float)mBlue  / pScalar)));
	return *this;
}

Color Color::operator / (float pScalar) const
{
	return Color((uint8)Max(0, Min(255, (int)((float)mRed   / pScalar))),
		         (uint8)Max(0, Min(255, (int)((float)mGreen / pScalar))),
				 (uint8)Max(0, Min(255, (int)((float)mBlue  / pScalar))),
				 mAlpha);
}

bool Color::operator==(const Color& pOther) const	
{ 
	return mRed   == pOther.mRed   &&
		   mGreen == pOther.mGreen &&
		   mBlue  == pOther.mBlue  &&
		   mAlpha == pOther.mAlpha;
}

bool Color::operator!=(const Color& pOther) const	
{ 
	return mRed   != pOther.mRed   ||
		   mGreen != pOther.mGreen ||
		   mBlue  != pOther.mBlue  ||
		   mAlpha != pOther.mAlpha;
}



int Color::SumRgb() const
{
	return (mRed+mGreen+mBlue);
}



}
