/*
	Class:  Sphere
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	This class is used as a representation of a sphere in CollisionDetector3D.
*/

#ifndef LEPRA_SPHERE_H
#define LEPRA_SPHERE_H

#include "Vector3D.h"

#define TEMPLATE template<class _TVarType>
#define QUAL Sphere<_TVarType>

namespace Lepra
{

TEMPLATE
class Sphere
{
public:
	inline Sphere();
	inline Sphere(const Vector3D<_TVarType>& pPosition, _TVarType pRadius);

	inline void SetPosition(const Vector3D<_TVarType>& pPosition);
	inline const Vector3D<_TVarType>& GetPosition() const;

	inline void SetRadius(const _TVarType pRadius);
	inline _TVarType GetRadius() const;
	inline _TVarType GetRadiusSquared() const;

private:
	Vector3D<_TVarType> mPosition;
	_TVarType mRadius;
	_TVarType mRadiusSquared;
};


TEMPLATE QUAL::Sphere() :
	mPosition(0, 0, 0),
	mRadius(0),
	mRadiusSquared(0)
{
}

TEMPLATE QUAL::Sphere(const Vector3D<_TVarType>& pPosition, _TVarType pRadius) :
	mPosition(pPosition),
	mRadius(pRadius),
	mRadiusSquared(pRadius * pRadius)
{
}

TEMPLATE void QUAL::SetPosition(const Vector3D<_TVarType>& pPosition)
{
	mPosition.Set(pPosition);
}

TEMPLATE const Vector3D<_TVarType>& QUAL::GetPosition() const
{
	return mPosition;
}

TEMPLATE void QUAL::SetRadius(const _TVarType pRadius)
{
	mRadius = pRadius;
	mRadiusSquared = pRadius * pRadius;
}

TEMPLATE _TVarType QUAL::GetRadius() const
{
	return mRadius;
}

TEMPLATE _TVarType QUAL::GetRadiusSquared() const
{
	return mRadiusSquared;
}

} // End namespace.

#undef TEMPLATE
#undef QUAL

#endif
