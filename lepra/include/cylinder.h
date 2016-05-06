/*
	Class:  Cylinder
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

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

#include "vector3d.h"
#include "rotationmatrix.h"

namespace lepra {

template<class _TVarType>
class Cylinder {
public:
	enum Alignment {
		kAlignX = 0,
		kAlignY,
		kAlignZ
	};

	Cylinder();
	Cylinder(const Vector3D<_TVarType>& position,
	         _TVarType length,
		 _TVarType radius,
		 Alignment alignment = kAlignX);
	Cylinder(const Vector3D<_TVarType>& position,
		 const RotationMatrix<_TVarType>& rot_mtx,
	         _TVarType length,
		 _TVarType radius,
		 Alignment alignment = kAlignX);

	inline void SetPosition(const Vector3D<_TVarType>& position);
	inline const Vector3D<_TVarType>& GetPosition() const;

	inline void SetRotation(const RotationMatrix<_TVarType>& rot_mtx);
	inline const RotationMatrix<_TVarType>& GetRotation() const;

	inline void SetRadius(const _TVarType radius);
	inline _TVarType GetRadius() const;
	inline _TVarType GetRadiusSquared() const;

	inline void SetLength(_TVarType length);
	inline _TVarType GetLength() const;

	inline void SetAlignment(Alignment alignment);
	inline Alignment GetAlignment() const;
private:
	Vector3D<_TVarType> position_;
	RotationMatrix<_TVarType> rot_mtx_;
	_TVarType length_;
	_TVarType radius_;
	Alignment alignment_;
};

template<class _TVarType> Cylinder<_TVarType>::Cylinder() :
	length_(0),
	radius_(0),
	alignment_(kAlignX) {
}

template<class _TVarType> Cylinder<_TVarType>::Cylinder(const Vector3D<_TVarType>& position,
                                                  _TVarType length,
                                                  _TVarType radius,
                                                  Alignment alignment) :
	position_(position),
	length_(length),
	radius_(radius),
	alignment_(alignment) {
}

template<class _TVarType> Cylinder<_TVarType>::Cylinder(const Vector3D<_TVarType>& position,
                                                  const RotationMatrix<_TVarType>& rot_mtx,
                                                  _TVarType length,
                                                  _TVarType radius,
						  Alignment alignment) :
	position_(position),
	rot_mtx_(rot_mtx),
	length_(length),
	radius_(radius),
	alignment_(alignment) {
}

template<class _TVarType> void Cylinder<_TVarType>::SetPosition(const Vector3D<_TVarType>& position) {
	position_ = position;
}

template<class _TVarType> const Vector3D<_TVarType>& Cylinder<_TVarType>::GetPosition() const {
	return position_;
}

template<class _TVarType> void Cylinder<_TVarType>::SetRotation(const RotationMatrix<_TVarType>& rot_mtx) {
	rot_mtx_ = rot_mtx;
}

template<class _TVarType> const RotationMatrix<_TVarType>& Cylinder<_TVarType>::GetRotation() const {
	return rot_mtx_;
}

template<class _TVarType> void Cylinder<_TVarType>::SetRadius(const _TVarType radius) {
	radius_ = radius;
}

template<class _TVarType> _TVarType Cylinder<_TVarType>::GetRadius() const {
	return radius_;
}

template<class _TVarType> _TVarType Cylinder<_TVarType>::GetRadiusSquared() const {
	return radius_ * radius_;
}

template<class _TVarType> void Cylinder<_TVarType>::SetLength(_TVarType length) {
	length_ = length;
}

template<class _TVarType> _TVarType Cylinder<_TVarType>::GetLength() const {
	return length_;
}

template<class _TVarType> void Cylinder<_TVarType>::SetAlignment(Alignment alignment) {
	alignment_ = alignment;
}

template<class _TVarType> typename Cylinder<_TVarType>::Alignment Cylinder<_TVarType>::GetAlignment() const {
	return alignment_;
}

}

#endif
