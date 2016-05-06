/*
	Class:  AABR
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	AABR = Axis Aligned Bounding Rect

	This is the 2D-version of AABB.

	The size vector contains half(!) the width (x), the height (y) of the rect.
	All are positive numbers. With other words: The size vector is a
	vector from the center of the rect to corner (xmax, ymax).
*/

#pragma once

#include "vector2d.h"

#define AABR_TEMPLATE template<class _TVarType>
#define AABR_QUAL AABR<_TVarType>

namespace lepra {

AABR_TEMPLATE class AABR {
public:
	inline AABR();
	inline AABR(_TVarType _x, _TVarType _y, _TVarType width, _TVarType height);
	inline AABR(const Vector2D<_TVarType>& position, const Vector2D<_TVarType>& size);

	// The position is always the center of the box.
	inline void SetPosition(const Vector2D<_TVarType>& position);
	inline const Vector2D<_TVarType>& GetPosition() const;
	inline void Move(const Vector2D<_TVarType>& offset);

	inline void SetSize(const Vector2D<_TVarType>& size);
	inline const Vector2D<_TVarType>& GetSize() const;

	inline _TVarType GetArea() const;

private:
	Vector2D<_TVarType> position_;
	Vector2D<_TVarType> size_;
};

AABR_TEMPLATE AABR_QUAL::AABR() :
	position_(0, 0),
	size_(0, 0) {
}

AABR_TEMPLATE AABR_QUAL::AABR(_TVarType _x, _TVarType _y, _TVarType width, _TVarType height):
	position_(_x, _y),
	size_(width, height) {
}

AABR_TEMPLATE AABR_QUAL::AABR(const Vector2D<_TVarType>& position, const Vector2D<_TVarType>& size):
	position_(position),
	size_(size) {
}

AABR_TEMPLATE void AABR_QUAL::SetPosition(const Vector2D<_TVarType>& position) {
	position_.Set(position);
}

AABR_TEMPLATE const Vector2D<_TVarType>& AABR_QUAL::GetPosition() const {
	return position_;
}

AABR_TEMPLATE void AABR_QUAL::Move(const Vector2D<_TVarType>& offset) {
	position_ += offset;
}

AABR_TEMPLATE void AABR<_TVarType>::SetSize(const Vector2D<_TVarType>& size) {
	size_.Set(size);
}

AABR_TEMPLATE const Vector2D<_TVarType>& AABR_QUAL::GetSize() const {
	return size_;
}

AABR_TEMPLATE _TVarType AABR_QUAL::GetArea() const {
	return size_.x * size_.y * 4;
}

}

#undef AABR_TEMPLATE
