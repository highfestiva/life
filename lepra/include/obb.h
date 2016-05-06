/*
	Class:  OBB
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	OBB = Oriented Bounding Box
	The size vector contains the width (x), the height (y) and the depth (z) of the box.
	All are positive numbers. Each component describes HALF the length of the corresponding side
	of the box. With other words: The size vector is a vector from the center of the box to one
	of the coorners.
*/


#pragma once

#include "vector3d.h"
#include "rotationmatrix.h"

namespace lepra {

template<class _TVarType>
class OBB {
public:
	inline OBB();
	inline OBB(const Vector3D<_TVarType>& position,
	           const Vector3D<_TVarType>& size,
	           const RotationMatrix<_TVarType>& rot_mtx);

	inline void SetPosition(const Vector3D<_TVarType>& position);
	inline const Vector3D<_TVarType>& GetPosition() const;

	inline void SetSize(const Vector3D<_TVarType>& size);
	inline const Vector3D<_TVarType>& GetSize() const;

	inline void SetRotation(const RotationMatrix<_TVarType>& rot_mtx);
	inline const RotationMatrix<_TVarType>& GetRotation() const;

	// The p_puchData parameter must containt the number of bytes
	// returned by GetRawDataSize().
	inline int GetRawDataSize();
	int GetRawData(unsigned char* p_puchData);
	int SetRawData(unsigned char* p_puchData);

private:
	Vector3D<_TVarType> position_;
	Vector3D<_TVarType> size_;
	RotationMatrix<_TVarType> rot_mtx_;
};

template<class _TVarType>
OBB<_TVarType>::OBB() :
	position_(0, 0, 0),
	size_(0, 0, 0) {
}

template<class _TVarType>
OBB<_TVarType>::OBB(const Vector3D<_TVarType>& position,
                    const Vector3D<_TVarType>& size,
                    const RotationMatrix<_TVarType>& rot_mtx) :
	position_(position),
	size_(size),
	rot_mtx_(rot_mtx) {
}

template<class _TVarType>
void OBB<_TVarType>::SetPosition(const Vector3D<_TVarType>& position) {
	position_.Set(position);
}

template<class _TVarType>
const Vector3D<_TVarType>& OBB<_TVarType>::GetPosition() const {
	return position_;
}

template<class _TVarType>
void OBB<_TVarType>::SetSize(const Vector3D<_TVarType>& size) {
	size_.Set(size);
}

template<class _TVarType>
const Vector3D<_TVarType>& OBB<_TVarType>::GetSize() const {
	return size_;
}

template<class _TVarType>
void OBB<_TVarType>::SetRotation(const RotationMatrix<_TVarType>& rot_mtx) {
	rot_mtx_ = rot_mtx;
}

template<class _TVarType>
const RotationMatrix<_TVarType>& OBB<_TVarType>::GetRotation() const {
	return rot_mtx_;
}

template<class _TVarType>
int OBB<_TVarType>::GetRawDataSize() {
	return position_.GetRawDataSize() + size_.GetRawDataSize() + rot_mtx_.GetRawDataSize();
}

template<class _TVarType>
int OBB<_TVarType>::GetRawData(unsigned char* p_puchData) {
	int offset = 0;

	offset += position_.GetRawData(&p_puchData[offset]);
	offset += size_.GetRawData(&p_puchData[offset]);
	offset += rot_mtx_.GetRawData(&p_puchData[offset]);

	return GetRawDataSize();
}

template<class _TVarType>
int OBB<_TVarType>::SetRawData(unsigned char* p_puchData) {
	int offset = 0;

	offset += position_.SetRawData(&p_puchData[offset]);
	offset += size_.SetRawData(&p_puchData[offset]);
	offset += rot_mtx_.SetRawData(&p_puchData[offset]);

	return GetRawDataSize();
}

}
