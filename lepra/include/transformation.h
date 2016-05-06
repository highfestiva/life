
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepratypes.h"
#include "vector3d.h"
#include "rotationmatrix.h"
#include "quaternion.h"

#define TEMPLATE template<class _TVarType>
#define QUAL Transformation<_TVarType>



namespace lepra {



TEMPLATE
class Transformation {
public:
	typedef _TVarType BaseType;

	Quaternion<_TVarType> orientation_;
	Vector3D<_TVarType> position_;

	Transformation();
	Transformation(const RotationMatrix<_TVarType>& orientation,
						  const Vector3D<_TVarType>& position);
	Transformation(const Quaternion<_TVarType>& orientation,
						  const Vector3D<_TVarType>& position);
	Transformation(const _TVarType data[7]);
	Transformation(const Transformation& transformation);

	void SetIdentity();

	RotationMatrix<_TVarType> GetOrientationAsMatrix() const;
	const Quaternion<_TVarType>& GetOrientation() const;
	const Vector3D<_TVarType>& GetPosition() const;
	Quaternion<_TVarType>& GetOrientation();
	Vector3D<_TVarType>& GetPosition();

	void SetPosition(const Vector3D<_TVarType>& position);
	void SetOrientation(const RotationMatrix<_TVarType>& orientation);
	void SetOrientation(const Quaternion<_TVarType>& orientation);

	// Relative to self.
	void MoveForward(_TVarType distance);
	void MoveRight(_TVarType distance);
	void MoveUp(_TVarType distance);

	void MoveBackward(_TVarType distance);
	void MoveLeft(_TVarType distance);
	void MoveDown(_TVarType distance);

	void RotateAroundAnchor(const Vector3D<_TVarType>& anchor, const Vector3D<_TVarType>& axis, _TVarType angle);

	void RotateYaw(_TVarType angle);
	void RotatePitch(_TVarType angle);
	void RotateRoll(_TVarType angle);

	// Relative to world.
	void MoveNorth(_TVarType distance);
	void MoveEast(_TVarType distance);
	void MoveWorldUp(_TVarType distance);

	void MoveSouth(_TVarType distance);
	void MoveWest(_TVarType distance);
	void MoveWorldDown(_TVarType distance);

	void RotateWorldX(_TVarType angle); // Pitch
	void RotateWorldY(_TVarType angle); // Yaw
	void RotateWorldZ(_TVarType angle); // Roll

	void Get(_TVarType matrix[7]) const;
	void GetAs4x4Matrix(_TVarType matrix[16]) const;
	void GetAs4x4TransposeMatrix(_TVarType matrix[16]) const;
	void GetAs4x4TransposeMatrix(_TVarType scale, _TVarType matrix[16]) const;
	void GetAs4x4InverseMatrix(_TVarType matrix[16]) const;
	void GetAs4x4InverseTransposeMatrix(_TVarType matrix[16]) const;

	void GetAs4x4OrientationMatrix(_TVarType matrix[16]) const;
	void GetAs4x4InverseOrientationMatrix(_TVarType matrix[16]) const;

	Vector3D<_TVarType> Transform(const Vector3D<_TVarType>& vector) const;
	Vector3D<_TVarType> InverseTransform(const Vector3D<_TVarType>& vector) const;

	Transformation Transform(const Transformation& transformation) const;
	Transformation InverseTransform(const Transformation& transformation) const;
	void FastInverseTransform(const Transformation& from, const quat inverse, const Transformation& to);

	Transformation Inverse() const;

	void Interpolate(const Transformation& start,
					 const Transformation& end,
					 _TVarType time);

	bool operator == (const Transformation& transformation);
	bool operator != (const Transformation& transformation);

	Transformation& operator = (const Transformation& transformation);

	Transformation& operator += (const Vector3D<_TVarType>& vector);
	Transformation  operator +  (const Vector3D<_TVarType>& vector) const;

	Transformation& operator *= (const Transformation& transformation);
	Transformation  operator *  (const Transformation& transformation) const;

	Transformation& operator /= (const Transformation& transformation);
	Transformation  operator /  (const Transformation& transformation) const;

	// T * V
	Vector3D<_TVarType> operator * (const Vector3D<_TVarType>& vector) const;

	Transformation<float> ToFloat() const;
	Transformation<double> ToDouble() const;
};



TEMPLATE inline Vector3D<_TVarType> operator * (const Vector3D<_TVarType>& vector, const Transformation<_TVarType>& transformation);
TEMPLATE inline const Vector3D<_TVarType>& operator *= (Vector3D<_TVarType>& vector, const Transformation<_TVarType>& transformation);



#include "transformation.inl"



typedef Transformation<float> xform;
typedef Transformation<double> TransformationD;

extern const xform kIdentityTransformationF;
extern const TransformationD kIdentityTransformationD;

#undef TEMPLATE
#undef QUAL



}
