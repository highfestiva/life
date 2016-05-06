/*
	Class:  AABB
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	AABB = Axis Aligned Bounding Box
	The size vector contains half(!) the width (x), the height (y) and the depth (z)
	of the box.	All are positive numbers. With other words: The size vector is a
	vector from the center of the box to corner (xmax, ymax, zmax).
*/

#pragma once

#include "vector3d.h"

#define TEMPLATE template<class _TVarType>
#define QUAL AABB<_TVarType>

namespace lepra {

TEMPLATE class AABB {
public:
	inline AABB();
	inline AABB(const Vector3D<_TVarType>& position, const Vector3D<_TVarType>& size);

	// The position is always the center of the box.
	inline void SetPosition(const Vector3D<_TVarType>& position);
	inline const Vector3D<_TVarType>& GetPosition() const;
	inline void Move(const Vector3D<_TVarType>& offset);

	inline void SetSize(const Vector3D<_TVarType>& size);
	inline const Vector3D<_TVarType>& GetSize() const;

	// The data parameter must containt the number of bytes
	// returned by GetRawDataSize().
	inline int GetRawDataSize();
	int GetRawData(uint8* data);
	void SetRawData(uint8* data);

private:
	Vector3D<_TVarType> position_;
	Vector3D<_TVarType> size_;
};

TEMPLATE QUAL::AABB() :
	position_(0, 0, 0),
	size_(0, 0, 0) {
}

TEMPLATE QUAL::AABB(const Vector3D<_TVarType>& position, const Vector3D<_TVarType>& size) :
	position_(position),
	size_(size) {
}

TEMPLATE void QUAL::SetPosition(const Vector3D<_TVarType>& position) {
	position_.Set(position);
}

TEMPLATE const Vector3D<_TVarType>& QUAL::GetPosition() const {
	return position_;
}

TEMPLATE void QUAL::Move(const Vector3D<_TVarType>& offset) {
	position_ += offset;
}

TEMPLATE void QUAL::SetSize(const Vector3D<_TVarType>& size) {
	size_.Set(size);
}

TEMPLATE const Vector3D<_TVarType>& QUAL::GetSize() const {
	return size_;
}

TEMPLATE int QUAL::GetRawDataSize() {
	return sizeof(Vector3D<_TVarType>) * 2;
}

TEMPLATE int QUAL::GetRawData(uint8* data) {
	_TVarType* _data = (_TVarType*)data;

	_data[0] = position_.x;
	_data[1] = position_.y;
	_data[2] = position_.z;
	_data[3] = size_.x;
	_data[4] = size_.y;
	_data[5] = size_.z;

	return GetRawDataSize();
}

TEMPLATE void QUAL::SetRawData(uint8* data) {
	_TVarType* _data = (_TVarType*)data;

	position_.x = _data[0];
	position_.y = _data[1];
	position_.z = _data[2];
	size_.x = _data[3];
	size_.y = _data[4];
	size_.z = _data[5];
}

}

#undef TEMPLATE
#undef QUAL
