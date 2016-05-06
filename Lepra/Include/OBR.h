/*
	Class:  OBR
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	OBR - Oriented Bounding Rectangle.
*/

#pragma once

#include "vector2d.h"

namespace lepra {

template<class _TVarType>
class OBR {
public:
	OBR(const Vector2D<_TVarType>& position,
	    const Vector2D<_TVarType>& size,
	    _TVarType orientation) :
		position_(position),
		size_(size),
		orientation_(orientation) {
	}

	OBR(const OBR& obb) :
		position_(obb.position_),
		size_(obb.size_),
		orientation_(obb.orientation_) {
	}

	inline void SetPosition(const Vector2D<_TVarType>& position);
	inline const Vector2D<_TVarType>& GetPosition() const;

	inline void SetSize(const Vector2D<_TVarType>& size);
	inline const Vector2D<_TVarType>& GetSize() const;

	inline void SetOrientation(const _TVarType orientation);
	inline _TVarType GetOrientation() const;

	inline Vector2D<_TVarType> GetExtentX() const; // Vector (Size.x, 0) rotated.
	inline Vector2D<_TVarType> GetExtentY() const; // Vector (0, Size.y) rotated.

	inline _TVarType GetArea() const;

private:

	Vector2D<_TVarType> position_;	// The center of the box.
	Vector2D<_TVarType> size_;	// The "radius", or half size, of the box.
	_TVarType orientation_;		// The rotation angle in radians.
};

template<class _TVarType>
void OBR<_TVarType>::SetPosition(const Vector2D<_TVarType>& position) {
	position_ = position;
}

template<class _TVarType>
const Vector2D<_TVarType>& OBR<_TVarType>::GetPosition() const {
	return position_;
}

template<class _TVarType>
void OBR<_TVarType>::SetSize(const Vector2D<_TVarType>& size) {
	size_ = size;
}

template<class _TVarType>
const Vector2D<_TVarType>& OBR<_TVarType>::GetSize() const {
	return size_;
}

template<class _TVarType>
void OBR<_TVarType>::SetOrientation(const _TVarType orientation) {
	orientation_ = orientation;
}

template<class _TVarType>
_TVarType OBR<_TVarType>::GetOrientation() const {
	return orientation_;
}

template<class _TVarType>
Vector2D<_TVarType> OBR<_TVarType>::GetExtentX() const {
	return Vector2D<_TVarType>(size_.x * cos(orientation_), size_.x * sin(orientation_));
}

template<class _TVarType>
Vector2D<_TVarType> OBR<_TVarType>::GetExtentY() const {
	return Vector2D<_TVarType>(-size_.y * sin(orientation_), size_.y * cos(orientation_));
}

template<class _TVarType>
_TVarType OBR<_TVarType>::GetArea() const {
	return size_.x * size_.y * 4;
}

}
