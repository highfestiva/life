/*
	Class:  Circle
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#pragma once

#include "vector2d.h"
#include "math.h"

#define CIRCLE_TEMPLATE template<class _TVarType>
#define CIRCLE_QUAL Circle<_TVarType>

namespace lepra {

CIRCLE_TEMPLATE
class Circle {
public:
	inline Circle();
	inline Circle(const Vector2D<_TVarType>& position, _TVarType radius);

	inline void SetPosition(const Vector2D<_TVarType>& position);
	inline const Vector2D<_TVarType>& GetPosition() const;

	inline void SetRadius(const _TVarType radius);
	inline _TVarType GetRadius() const;
	inline _TVarType GetRadiusSquared() const;

	inline _TVarType GetArea() const;

private:
	Vector2D<_TVarType> position_;
	_TVarType radius_;
	_TVarType radius_squared_;
};


CIRCLE_TEMPLATE CIRCLE_QUAL::Circle() :
	position_(0, 0),
	radius_(0),
	radius_squared_(0) {
}

CIRCLE_TEMPLATE CIRCLE_QUAL::Circle(const Vector2D<_TVarType>& position, _TVarType radius) :
	position_(position),
	radius_(radius),
	radius_squared_(radius * radius) {
}

CIRCLE_TEMPLATE void CIRCLE_QUAL::SetPosition(const Vector2D<_TVarType>& position) {
	position_.Set(position);
}

CIRCLE_TEMPLATE const Vector2D<_TVarType>& CIRCLE_QUAL::GetPosition() const {
	return position_;
}

CIRCLE_TEMPLATE void CIRCLE_QUAL::SetRadius(const _TVarType radius) {
	radius_ = radius;
	radius_squared_ = radius * radius;
}

CIRCLE_TEMPLATE _TVarType CIRCLE_QUAL::GetRadius() const {
	return radius_;
}

CIRCLE_TEMPLATE _TVarType CIRCLE_QUAL::GetRadiusSquared() const {
	return radius_squared_;
}

CIRCLE_TEMPLATE _TVarType CIRCLE_QUAL::GetArea() const {
	return radius_squared_ * PIF;
}

}

#undef CIRCLE_TEMPLATE
#undef CIRCLE_QUAL
