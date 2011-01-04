/*
	Class:  OBR
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	OBR - Oriented Bounding Rectangle.
*/

#ifndef LEPRA_OBR_H
#define LEPRA_OBR_H

#include "Vector2D.h"

namespace Lepra
{

template<class _TVarType>
class OBR
{
public:
	OBR(const Vector2D<_TVarType>& pPosition, 
	    const Vector2D<_TVarType>& pSize,
	    _TVarType pOrientation) :
		mPosition(pPosition),
		mSize(pSize),
		mOrientation(pOrientation)
	{
	}

	OBR(const OBR& pOBB) :
		mPosition(pOBB.mPosition),
		mSize(pOBB.mSize),
		mOrientation(pOBB.mOrientation)
	{
	}

	inline void SetPosition(const Vector2D<_TVarType>& pPosition);
	inline const Vector2D<_TVarType>& GetPosition() const;

	inline void SetSize(const Vector2D<_TVarType>& pSize);
	inline const Vector2D<_TVarType>& GetSize() const;

	inline void SetOrientation(const _TVarType pOrientation);
	inline _TVarType GetOrientation() const;

	inline Vector2D<_TVarType> GetExtentX() const; // Vector (Size.x, 0) rotated.
	inline Vector2D<_TVarType> GetExtentY() const; // Vector (0, Size.y) rotated.

	inline _TVarType GetArea() const;

private:

	Vector2D<_TVarType> mPosition;	// The center of the box.
	Vector2D<_TVarType> mSize;	// The "radius", or half size, of the box.
	_TVarType mOrientation;		// The rotation angle in radians.
};

template<class _TVarType>
void OBR<_TVarType>::SetPosition(const Vector2D<_TVarType>& pPosition)
{
	mPosition = pPosition;
}

template<class _TVarType>
const Vector2D<_TVarType>& OBR<_TVarType>::GetPosition() const
{
	return mPosition;
}

template<class _TVarType>
void OBR<_TVarType>::SetSize(const Vector2D<_TVarType>& pSize)
{
	mSize = pSize;
}

template<class _TVarType>
const Vector2D<_TVarType>& OBR<_TVarType>::GetSize() const
{
	return mSize;
}

template<class _TVarType>
void OBR<_TVarType>::SetOrientation(const _TVarType pOrientation)
{
	mOrientation = pOrientation;
}

template<class _TVarType>
_TVarType OBR<_TVarType>::GetOrientation() const
{
	return mOrientation;
}

template<class _TVarType>
Vector2D<_TVarType> OBR<_TVarType>::GetExtentX() const
{
	return Vector2D<_TVarType>(mSize.x * cos(mOrientation), mSize.x * sin(mOrientation));
}

template<class _TVarType>
Vector2D<_TVarType> OBR<_TVarType>::GetExtentY() const
{
	return Vector2D<_TVarType>(-mSize.y * sin(mOrientation), mSize.y * cos(mOrientation));
}

template<class _TVarType>
_TVarType OBR<_TVarType>::GetArea() const
{
	return mSize.x * mSize.y * 4;
}

} // End namespace.

#endif