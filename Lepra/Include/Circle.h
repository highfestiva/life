/*
	Class:  Circle
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#ifndef CIRCLE_H
#define CIRCLE_H

#include "Vector2D.h"
#include "Math.h"

#define CIRCLE_TEMPLATE template<class _TVarType>
#define CIRCLE_QUAL Circle<_TVarType>

namespace Lepra
{

CIRCLE_TEMPLATE
class Circle
{
public:
	inline Circle();
	inline Circle(const Vector2D<_TVarType>& pPosition, _TVarType pRadius);

	inline void SetPosition(const Vector2D<_TVarType>& pPosition);
	inline const Vector2D<_TVarType>& GetPosition() const;

	inline void SetRadius(const _TVarType pRadius);
	inline _TVarType GetRadius() const;
	inline _TVarType GetRadiusSquared() const;

	inline _TVarType GetArea() const;

private:
	Vector2D<_TVarType> mPosition;
	_TVarType mRadius;
	_TVarType mRadiusSquared;
};


CIRCLE_TEMPLATE CIRCLE_QUAL::Circle() :
	mPosition(0, 0),
	mRadius(0),
	mRadiusSquared(0)
{
}

CIRCLE_TEMPLATE CIRCLE_QUAL::Circle(const Vector2D<_TVarType>& pPosition, _TVarType pRadius) :
	mPosition(pPosition),
	mRadius(pRadius),
	mRadiusSquared(pRadius * pRadius)
{
}

CIRCLE_TEMPLATE void CIRCLE_QUAL::SetPosition(const Vector2D<_TVarType>& pPosition)
{
	mPosition.Set(pPosition);
}

CIRCLE_TEMPLATE const Vector2D<_TVarType>& CIRCLE_QUAL::GetPosition() const
{
	return mPosition;
}

CIRCLE_TEMPLATE void CIRCLE_QUAL::SetRadius(const _TVarType pRadius)
{
	mRadius = pRadius;
	mRadiusSquared = pRadius * pRadius;
}

CIRCLE_TEMPLATE _TVarType CIRCLE_QUAL::GetRadius() const
{
	return mRadius;
}

CIRCLE_TEMPLATE _TVarType CIRCLE_QUAL::GetRadiusSquared() const
{
	return mRadiusSquared;
}

CIRCLE_TEMPLATE _TVarType CIRCLE_QUAL::GetArea() const
{
	return mRadiusSquared * PIF;
}

} // End namespace.

#undef CIRCLE_TEMPLATE
#undef CIRCLE_QUAL

#endif
