/*
	Class:  Cylinder
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	This class is used as a representation of a cylinder in CollisionDetector3D.
	The interpretation of the members is as follows:

	* Position    - the center of the cylinder.
	* Rotation - the orientation/rotation of the cylinder which (in a non-
	                rotated state) is aligned as described below (see alignment).
	* Length      - HALF the length of the cylinder! Starting at the position,
	                following the alignment axis (see below) "Length" units
		        will end up in the center of the end caps.
	* Radius      - the radius, as usual.
	* Alignment   - The axis along which the cylinder is aligned. This can
	                also be considered "the length axis". This defines the 
			original orientation of the cylinder.
*/

#ifndef LEPRA_CYLINDER
#define LEPRA_CYLINDER

#include "Vector3D.h"
#include "RotationMatrix.h"

namespace Lepra
{

template<class _TVarType>
class Cylinder
{
public:
	enum Alignment
	{
		ALIGN_X = 0,
		ALIGN_Y,
		ALIGN_Z
	};

	Cylinder();
	Cylinder(const Vector3D<_TVarType>& pPosition,
	         _TVarType pLength, 
		 _TVarType pRadius, 
		 Alignment pAlignment = ALIGN_X);
	Cylinder(const Vector3D<_TVarType>& pPosition,
		 const RotationMatrix<_TVarType>& pRotMtx,
	         _TVarType pLength, 
		 _TVarType pRadius, 
		 Alignment pAlignment = ALIGN_X);

	inline void SetPosition(const Vector3D<_TVarType>& pPosition);
	inline const Vector3D<_TVarType>& GetPosition() const;

	inline void SetRotation(const RotationMatrix<_TVarType>& pRotMtx);
	inline const RotationMatrix<_TVarType>& GetRotation() const;

	inline void SetRadius(const _TVarType pRadius);
	inline _TVarType GetRadius() const;
	inline _TVarType GetRadiusSquared() const;

	inline void SetLength(_TVarType pLength);
	inline _TVarType GetLength() const;

	inline void SetAlignment(Alignment pAlignment);
	inline Alignment GetAlignment() const;
private:
	Vector3D<_TVarType> mPosition;
	RotationMatrix<_TVarType> mRotMtx;
	_TVarType mLength;
	_TVarType mRadius;
	Alignment mAlignment;
};

template<class _TVarType> Cylinder<_TVarType>::Cylinder() :
	mLength(0),
	mRadius(0),
	mAlignment(ALIGN_X)
{
}

template<class _TVarType> Cylinder<_TVarType>::Cylinder(const Vector3D<_TVarType>& pPosition,
                                                  _TVarType pLength, 
                                                  _TVarType pRadius, 
                                                  Alignment pAlignment) :
	mPosition(pPosition),
	mLength(pLength),
	mRadius(pRadius),
	mAlignment(pAlignment)
{
}

template<class _TVarType> Cylinder<_TVarType>::Cylinder(const Vector3D<_TVarType>& pPosition,
                                                  const RotationMatrix<_TVarType>& pRotMtx,
                                                  _TVarType pLength, 
                                                  _TVarType pRadius, 
						  Alignment pAlignment) :
	mPosition(pPosition),
	mRotMtx(pRotMtx),
	mLength(pLength),
	mRadius(pRadius),
	mAlignment(pAlignment)
{
}

template<class _TVarType> void Cylinder<_TVarType>::SetPosition(const Vector3D<_TVarType>& pPosition)
{
	mPosition = pPosition;
}

template<class _TVarType> const Vector3D<_TVarType>& Cylinder<_TVarType>::GetPosition() const
{
	return mPosition;
}

template<class _TVarType> void Cylinder<_TVarType>::SetRotation(const RotationMatrix<_TVarType>& pRotMtx)
{
	mRotMtx = pRotMtx;
}

template<class _TVarType> const RotationMatrix<_TVarType>& Cylinder<_TVarType>::GetRotation() const
{
	return mRotMtx;
}

template<class _TVarType> void Cylinder<_TVarType>::SetRadius(const _TVarType pRadius)
{
	mRadius = pRadius;
}

template<class _TVarType> _TVarType Cylinder<_TVarType>::GetRadius() const
{
	return mRadius;
}

template<class _TVarType> _TVarType Cylinder<_TVarType>::GetRadiusSquared() const
{
	return mRadius * mRadius;
}

template<class _TVarType> void Cylinder<_TVarType>::SetLength(_TVarType pLength)
{
	mLength = pLength;
}

template<class _TVarType> _TVarType Cylinder<_TVarType>::GetLength() const
{
	return mLength;
}

template<class _TVarType> void Cylinder<_TVarType>::SetAlignment(Alignment pAlignment)
{
	mAlignment = pAlignment;
}

template<class _TVarType> typename Cylinder<_TVarType>::Alignment Cylinder<_TVarType>::GetAlignment() const
{
	return mAlignment;
}

} // End namespace.

#endif
