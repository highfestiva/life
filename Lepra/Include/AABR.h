/*
	Class:  AABR
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	AABR = Axis Aligned Bounding Rect

	This is the 2D-version of AABB.

	The size vector contains half(!) the width (x), the height (y) of the rect.
	All are positive numbers. With other words: The size vector is a 
	vector from the center of the rect to corner (xmax, ymax).
*/

#ifndef AABR_H
#define AABR_H

#include "Vector2D.h"

#define AABR_TEMPLATE template<class _TVarType>
#define AABR_QUAL AABR<_TVarType>

namespace Lepra
{

AABR_TEMPLATE class AABR
{
public:
	inline AABR();
	inline AABR(_TVarType pX, _TVarType pY, _TVarType pWidth, _TVarType pHeight);
	inline AABR(const Vector2D<_TVarType>& pPosition, const Vector2D<_TVarType>& pSize);

	// The position is always the center of the box.
	inline void SetPosition(const Vector2D<_TVarType>& pPosition);
	inline const Vector2D<_TVarType>& GetPosition() const;
	inline void Move(const Vector2D<_TVarType>& pOffset);

	inline void SetSize(const Vector2D<_TVarType>& pSize);
	inline const Vector2D<_TVarType>& GetSize() const;

	inline _TVarType GetArea() const;

private:
	Vector2D<_TVarType> mPosition;
	Vector2D<_TVarType> mSize;
};

AABR_TEMPLATE AABR_QUAL::AABR() :
	mPosition(0, 0),
	mSize(0, 0)
{
}

AABR_TEMPLATE AABR_QUAL::AABR(_TVarType pX, _TVarType pY, _TVarType pWidth, _TVarType pHeight):
	mPosition(pX, pY),
	mSize(pWidth, pHeight)
{
}

AABR_TEMPLATE AABR_QUAL::AABR(const Vector2D<_TVarType>& pPosition, const Vector2D<_TVarType>& pSize):
	mPosition(pPosition),
	mSize(pSize)
{
}

AABR_TEMPLATE void AABR_QUAL::SetPosition(const Vector2D<_TVarType>& pPosition)
{
	mPosition.Set(pPosition);
}

AABR_TEMPLATE const Vector2D<_TVarType>& AABR_QUAL::GetPosition() const
{
	return mPosition;
}

AABR_TEMPLATE void AABR_QUAL::Move(const Vector2D<_TVarType>& pOffset)
{
	mPosition += pOffset;
}

AABR_TEMPLATE void AABR<_TVarType>::SetSize(const Vector2D<_TVarType>& pSize)
{
	mSize.Set(pSize);
}

AABR_TEMPLATE const Vector2D<_TVarType>& AABR_QUAL::GetSize() const
{
	return mSize;
}

AABR_TEMPLATE _TVarType AABR_QUAL::GetArea() const
{
	return mSize.x * mSize.y * 4;
}

} // End namespace.

#undef AABR_TEMPLATE

#endif