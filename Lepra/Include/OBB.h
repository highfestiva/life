/*
	Class:  OBB
	Author: Alexander Hugestrand
	Copyright (c) 2002-2005, Alexander Hugestrand

	NOTES:
	
	OBB = Oriented Bounding Box
	The size vector contains the width (x), the height (y) and the depth (z) of the box.
	All are positive numbers. Each component describes HALF the length of the corresponding side
	of the box. With other words: The size vector is a vector from the center of the box to one
	of the coorners.
*/


#ifndef LEPRA_OBB_H
#define LEPRA_OBB_H

#include "Vector3D.h"
#include "RotationMatrix.h"

namespace Lepra
{

template<class _TVarType>
class OBB
{
public:
	inline OBB();
	inline OBB(const Vector3D<_TVarType>& p_Position, 
	           const Vector3D<_TVarType>& p_Size, 
	           const RotationMatrix<_TVarType>& p_RotMtx);

	inline void SetPosition(const Vector3D<_TVarType>& p_Position);
	inline const Vector3D<_TVarType>& GetPosition() const;

	inline void SetSize(const Vector3D<_TVarType>& p_Size);
	inline const Vector3D<_TVarType>& GetSize() const;

	inline void SetRotation(const RotationMatrix<_TVarType>& p_RotMtx);
	inline const RotationMatrix<_TVarType>& GetRotation() const;

	// The p_puchData parameter must containt the number of bytes
	// returned by GetRawDataSize().
	inline int GetRawDataSize();
	int GetRawData(unsigned char* p_puchData);
	int SetRawData(unsigned char* p_puchData);

private:
	Vector3D<_TVarType> m_Position;
	Vector3D<_TVarType> m_Size;
	RotationMatrix<_TVarType> m_RotMtx;
};

template<class _TVarType>
OBB<_TVarType>::OBB() :
	m_Position(0, 0, 0),
	m_Size(0, 0, 0)
{
}

template<class _TVarType>
OBB<_TVarType>::OBB(const Vector3D<_TVarType>& p_Position, 
                    const Vector3D<_TVarType>& p_Size, 
                    const RotationMatrix<_TVarType>& p_RotMtx) :
	m_Position(p_Position),
	m_Size(p_Size),
	m_RotMtx(p_RotMtx)
{
}

template<class _TVarType>
void OBB<_TVarType>::SetPosition(const Vector3D<_TVarType>& p_Position)
{
	m_Position.Set(p_Position);
}

template<class _TVarType>
const Vector3D<_TVarType>& OBB<_TVarType>::GetPosition() const
{
	return m_Position;
}

template<class _TVarType>
void OBB<_TVarType>::SetSize(const Vector3D<_TVarType>& p_Size)
{
	m_Size.Set(p_Size);
}

template<class _TVarType>
const Vector3D<_TVarType>& OBB<_TVarType>::GetSize() const
{
	return m_Size;
}

template<class _TVarType>
void OBB<_TVarType>::SetRotation(const RotationMatrix<_TVarType>& p_RotMtx)
{
	m_RotMtx = p_RotMtx;
}

template<class _TVarType>
const RotationMatrix<_TVarType>& OBB<_TVarType>::GetRotation() const
{
	return m_RotMtx;
}

template<class _TVarType>
int OBB<_TVarType>::GetRawDataSize()
{
	return m_Position.GetRawDataSize() + m_Size.GetRawDataSize() + m_RotMtx.GetRawDataSize();
}

template<class _TVarType>
int OBB<_TVarType>::GetRawData(unsigned char* p_puchData)
{
	int l_iOffset = 0;

	l_iOffset += m_Position.GetRawData(&p_puchData[l_iOffset]);
	l_iOffset += m_Size.GetRawData(&p_puchData[l_iOffset]);
	l_iOffset += m_RotMtx.GetRawData(&p_puchData[l_iOffset]);

	return GetRawDataSize();
}

template<class _TVarType>
int OBB<_TVarType>::SetRawData(unsigned char* p_puchData)
{
	int l_iOffset = 0;

	l_iOffset += m_Position.SetRawData(&p_puchData[l_iOffset]);
	l_iOffset += m_Size.SetRawData(&p_puchData[l_iOffset]);
	l_iOffset += m_RotMtx.SetRawData(&p_puchData[l_iOffset]);

	return GetRawDataSize();
}

} // End namespace.

#endif