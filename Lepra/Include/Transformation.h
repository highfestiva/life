
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "LepraTypes.h"
#include "Vector3D.h"
#include "RotationMatrix.h"
#include "Quaternion.h"

#define TEMPLATE template<class _TVarType>
#define QUAL Transformation<_TVarType>



namespace Lepra
{



TEMPLATE
class Transformation
{
public:
	typedef _TVarType BaseType;

	Transformation();
	Transformation(const RotationMatrix<_TVarType>& pOrientation,
						  const Vector3D<_TVarType>& pPosition);
	Transformation(const Quaternion<_TVarType>& pOrientation,
						  const Vector3D<_TVarType>& pPosition);
	Transformation(const _TVarType pData[7]);
	Transformation(const Transformation& pTransformation);
	virtual ~Transformation();

	void SetIdentity();

	RotationMatrix<_TVarType> GetOrientationAsMatrix() const;
	const Quaternion<_TVarType>& GetOrientation() const;
	const Vector3D<_TVarType>& GetPosition() const;
	Quaternion<_TVarType>& GetOrientation();
	Vector3D<_TVarType>& GetPosition();

	void SetPosition(const Vector3D<_TVarType>& pPosition);
	void SetOrientation(const RotationMatrix<_TVarType>& pOrientation);
	void SetOrientation(const Quaternion<_TVarType>& pOrientation);

	// Relative to self.
	void MoveForward(_TVarType pDistance);
	void MoveRight(_TVarType pDistance);
	void MoveUp(_TVarType pDistance);

	void MoveBackward(_TVarType pDistance);
	void MoveLeft(_TVarType pDistance);
	void MoveDown(_TVarType pDistance);

	void RotateAroundAnchor(const Vector3D<_TVarType>& pAnchor, const Vector3D<_TVarType>& pAxis, _TVarType pAngle);

	void RotateYaw(_TVarType pAngle);
	void RotatePitch(_TVarType pAngle);
	void RotateRoll(_TVarType pAngle);

	// Relative to world.
	void MoveNorth(_TVarType pDistance);
	void MoveEast(_TVarType pDistance);
	void MoveWorldUp(_TVarType pDistance);

	void MoveSouth(_TVarType pDistance);
	void MoveWest(_TVarType pDistance);
	void MoveWorldDown(_TVarType pDistance);

	void RotateWorldX(_TVarType pAngle); // Pitch
	void RotateWorldY(_TVarType pAngle); // Yaw
	void RotateWorldZ(_TVarType pAngle); // Roll

	void Get(_TVarType pMatrix[7]) const;
	void GetAs4x4Matrix(_TVarType pMatrix[16]) const;
	void GetAs4x4TransposeMatrix(_TVarType pMatrix[16]) const;
	void GetAs4x4InverseMatrix(_TVarType pMatrix[16]) const;
	void GetAs4x4InverseTransposeMatrix(_TVarType pMatrix[16]) const;

	void GetAs4x4OrientationMatrix(_TVarType pMatrix[16]) const;
	void GetAs4x4InverseOrientationMatrix(_TVarType pMatrix[16]) const;

	Vector3D<_TVarType> Transform(const Vector3D<_TVarType>& pVector) const;
	Vector3D<_TVarType> InverseTransform(const Vector3D<_TVarType>& pVector) const;

	Transformation Transform(const Transformation& pTransformation) const;
	Transformation InverseTransform(const Transformation& pTransformation) const;
	void FastInverseTransform(const Transformation& pFrom, const Transformation& pTo);

	Transformation Inverse() const;

	void Interpolate(const Transformation& pStart,
					 const Transformation& pEnd,
					 _TVarType pTime);

	bool operator == (const Transformation& pTransformation);
	bool operator != (const Transformation& pTransformation);

	Transformation& operator = (const Transformation& pTransformation);

	Transformation& operator += (const Vector3D<_TVarType>& pVector);
	Transformation  operator +  (const Vector3D<_TVarType>& pVector) const;

	Transformation& operator *= (const Transformation& pTransformation);
	Transformation  operator *  (const Transformation& pTransformation) const;

	Transformation& operator /= (const Transformation& pTransformation);
	Transformation  operator /  (const Transformation& pTransformation) const;

	// T * V
	Vector3D<_TVarType> operator * (const Vector3D<_TVarType>& pVector) const;

	Transformation<float> ToFloat() const;
	Transformation<double> ToDouble() const;

protected:
private:
	Quaternion<_TVarType> mOrientation;
	Vector3D<_TVarType> mPosition;
};



TEMPLATE inline Vector3D<_TVarType> operator * (const Vector3D<_TVarType>& pVector, const Transformation<_TVarType>& pTransformation);
TEMPLATE inline const Vector3D<_TVarType>& operator *= (Vector3D<_TVarType>& pVector, const Transformation<_TVarType>& pTransformation);



#include "Transformation.inl"



typedef Transformation<float> TransformationF;
typedef Transformation<double> TransformationD;

const Transformation<float> gIdentityTransformationF;
const Transformation<double> gIdentityTransformationD;

#undef TEMPLATE
#undef QUAL



}
