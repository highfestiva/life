/*
	Class:  Sphere
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	This class is used as a representation of a sphere in CollisionDetector3D.
*/

#pragma once

#include "vector3d.h"

#define TEMPLATE template<class _TVarType>
#define QUAL Sphere<_TVarType>

namespace lepra {

TEMPLATE
class Sphere {
public:
	inline Sphere();
	inline Sphere(const Vector3D<_TVarType>& position, _TVarType radius);

	inline void SetPosition(const Vector3D<_TVarType>& position);
	inline const Vector3D<_TVarType>& GetPosition() const;

	inline void SetRadius(const _TVarType radius);
	inline _TVarType GetRadius() const;
	inline _TVarType GetRadiusSquared() const;

private:
	Vector3D<_TVarType> position_;
	_TVarType radius_;
	_TVarType radius_squared_;
};


TEMPLATE QUAL::Sphere() :
	position_(0, 0, 0),
	radius_(0),
	radius_squared_(0) {
}

TEMPLATE QUAL::Sphere(const Vector3D<_TVarType>& position, _TVarType radius) :
	position_(position),
	radius_(radius),
	radius_squared_(radius * radius) {
}

TEMPLATE void QUAL::SetPosition(const Vector3D<_TVarType>& position) {
	position_.Set(position);
}

TEMPLATE const Vector3D<_TVarType>& QUAL::GetPosition() const {
	return position_;
}

TEMPLATE void QUAL::SetRadius(const _TVarType radius) {
	radius_ = radius;
	radius_squared_ = radius * radius;
}

TEMPLATE _TVarType QUAL::GetRadius() const {
	return radius_;
}

TEMPLATE _TVarType QUAL::GetRadiusSquared() const {
	return radius_squared_;
}

}

#undef TEMPLATE
#undef QUAL
