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

#include "Vector3D.h"

#define TEMPLATE template<class _TVarType>
#define QUAL AABB<_TVarType>

namespace Lepra
{

TEMPLATE class AABB
{
public:
	inline AABB();
	inline AABB(const Vector3D<_TVarType>& pPosition, const Vector3D<_TVarType>& pSize);

	// The position is always the center of the box.
	inline void SetPosition(const Vector3D<_TVarType>& pPosition);
	inline const Vector3D<_TVarType>& GetPosition() const;
	inline void Move(const Vector3D<_TVarType>& pOffset);

	inline void SetSize(const Vector3D<_TVarType>& pSize);
	inline const Vector3D<_TVarType>& GetSize() const;

	// The pData parameter must containt the number of bytes
	// returned by GetRawDataSize().
	inline int GetRawDataSize();
	int GetRawData(uint8* pData);
	void SetRawData(uint8* pData);

private:
	Vector3D<_TVarType> mPosition;
	Vector3D<_TVarType> mSize;
};

TEMPLATE QUAL::AABB() :
	mPosition(0, 0, 0),
	mSize(0, 0, 0)
{
}

TEMPLATE QUAL::AABB(const Vector3D<_TVarType>& pPosition, const Vector3D<_TVarType>& pSize) :
	mPosition(pPosition),
	mSize(pSize)
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

TEMPLATE void QUAL::Move(const Vector3D<_TVarType>& pOffset)
{
	mPosition += pOffset;
}

TEMPLATE void QUAL::SetSize(const Vector3D<_TVarType>& pSize)
{
	mSize.Set(pSize);
}

TEMPLATE const Vector3D<_TVarType>& QUAL::GetSize() const
{
	return mSize;
}

TEMPLATE int QUAL::GetRawDataSize()
{
	return sizeof(Vector3D<_TVarType>) * 2;
}

TEMPLATE int QUAL::GetRawData(uint8* pData)
{
	_TVarType* lData = (_TVarType*)pData;

	lData[0] = mPosition.x;
	lData[1] = mPosition.y;
	lData[2] = mPosition.z;
	lData[3] = mSize.x;
	lData[4] = mSize.y;
	lData[5] = mSize.z;

	return GetRawDataSize();
}

TEMPLATE void QUAL::SetRawData(uint8* pData)
{
	_TVarType* lData = (_TVarType*)pData;

	mPosition.x = lData[0];
	mPosition.y = lData[1];
	mPosition.z = lData[2];
	mSize.x = lData[3];
	mSize.y = lData[4];
	mSize.z = lData[5];
}

}

#undef TEMPLATE
#undef QUAL
