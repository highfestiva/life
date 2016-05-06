/*
	Class:  Quaternion
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	Quaternions are used to represent rotation (orientation). There has
	been discussions about the goods and bads of both quaternions
	and rotation matrices for ages now, and there seems to be no end to it.

	These are the facts:

	1. Matrices are (when this is written) more common than quaternions, and
	both DirectX and OpenGL use matrices to represent rotation.

	2. Everything that you can do with quaternions can be done using matrices,
	(even correct and smooth interpolation!).

	3. Many of the things that you can do with matrices can be done faster
	using quaternions, (especially interpolation), but matrices may be faster
	on other things.

	4. Many articles on the internet describe algorithms to convert between
	quaternions and	matrices that are erroneous. I have provided functions that
	are safe, and always produce correct results (as correct as possible, read
	the "IMPORTANT" note below).

	To avoid recursive includes of RotationMatrix.h and Quaternion.h I needed
	to put all functions that convert between matrices and quaternions in one
	place, so I decided that this was that place. RotationMatrix has no
	knowledge about quaternions whatsoever. Quaternion provides all
	convertion functionality needed.

	IMPORTANT! If you are experiencing rotational bugs, where your models
	suddenly flip to another orientation than they are supposed to, it's
	probably due to floating point rounding errors. These kind of bugs occur
	when you convert back and forth between quaternions and rotation matrices a
	couple of times, and the rounding errors grow large. The solution is to
	make sure that the rotation matrix is orthogonal before converting it to a
	quaternion. Trying to normalize the quaternion instead (to save CPU cycles)
	won't help much I'm afraid.
*/



#pragma once

#include "lepratypes.h"
#include "math.h"
#include "rotationmatrix.h"

#pragma warning(push)
#pragma warning(disable: 4201)	// Nonstandard extention: unnamed struct.



namespace lepra {



template<class _TVarType>
class Quaternion {
public:
	union {
	_TVarType data_[4];
	struct {
	_TVarType a; // Real part.
	_TVarType b; // Imaginary i
	_TVarType c; // Imaginary j
	_TVarType d; // Imaginary k
	};
	};

	inline Quaternion();
	inline Quaternion(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d);
	inline Quaternion(const _TVarType data[4]);
	inline Quaternion(const Quaternion& quaternion);
	inline Quaternion(const RotationMatrix<_TVarType>& rot_mtx);
	inline Quaternion(_TVarType angle, const Vector3D<_TVarType>& vector);

	inline void Set(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d);
	inline void Set(const _TVarType data[4]);
	inline void Set(const Quaternion& quaternion);
	inline void SetConjugate(const Quaternion& quaternion);
	void Set(const RotationMatrix<_TVarType>& rot_mtx);
	inline void SetIdentity();

	// Angle of rotation around vector (x, y, z).
	inline void Set(_TVarType angle, const Vector3D<_TVarType>& vector);

	RotationMatrix<_TVarType> GetAsRotationMatrix() const;
	void GetAsRotationMatrix(RotationMatrix<_TVarType>& rot_mtx) const;

	inline void Add(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d);
	inline void Add(const Quaternion& quaternion);

	inline void Sub(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d);
	inline void Sub(const Quaternion& quaternion);

	inline void Mul(_TVarType x);
	inline void Div(_TVarType x);

	inline void Mul(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d);
	inline void Mul(const _TVarType data[4]);
	inline void Mul(const Quaternion& quaternion);

	// Special multiplication functions which can be used to optimize some calculations.
	// "this" is A, and the parameter matrix is B.
	inline void AMulInvB(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d);
	inline void AMulInvB(const _TVarType data[4]);
	inline void AMulInvB(const Quaternion& quaternion);
	inline void InvAMulB(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d);
	inline void FastInvAMulB(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d);
	inline void InvAMulB(const _TVarType data[4]);
	inline void InvAMulB(const Quaternion& quaternion);
	inline void InvAMulInvB(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d);
	inline void InvAMulInvB(const _TVarType data[4]);
	inline void InvAMulInvB(const Quaternion& quaternion);

	inline void BMulA(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d);
	inline void BMulA(const _TVarType data[4]);
	inline void BMulA(const Quaternion& quaternion);
	inline void InvBMulA(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d);
	inline void InvBMulA(const _TVarType data[4]);
	inline void InvBMulA(const Quaternion& quaternion);
	inline void BMulInvA(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d);
	inline void BMulInvA(const _TVarType data[4]);
	inline void BMulInvA(const Quaternion& quaternion);
	inline void InvBMulInvA(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d);
	inline void InvBMulInvA(const _TVarType data[4]);
	inline void InvBMulInvA(const Quaternion& quaternion);

	inline void Div(_TVarType _a, _TVarType _b, _TVarType _c, _TVarType _d);
	inline void Div(const _TVarType data[4]);
	inline void Div(const Quaternion& quaternion);

	inline Vector3D<_TVarType> GetRotatedVector(const Vector3D<_TVarType>& vector) const;
	inline void FastRotatedVector(const Quaternion& inverse, Vector3D<_TVarType>& target, const Vector3D<_TVarType>& source) const;
	inline Vector3D<_TVarType> GetInverseRotatedVector(const Vector3D<_TVarType>& vector) const;
	inline void FastInverseRotatedVector(const Quaternion& inverse, Vector3D<_TVarType>& target, const Vector3D<_TVarType>& source) const;

	inline void MakeConjugate();
	inline Quaternion GetConjugate() const;

	inline void MakeInverse();
	inline Quaternion GetInverse() const;

	inline void MakeSquare();
	inline Quaternion GetSquare() const;

	inline _TVarType GetNorm() const;
	inline _TVarType GetMagnitude() const;

	inline void Negate();
	inline void Normalize(_TVarType length = (_TVarType)1.0);

	// Slerp = Spherical Linear intERPolation.
	// 0 <= t <= 1
	void Slerp(const Quaternion<_TVarType>& from,
		   const Quaternion<_TVarType>& to,
		   _TVarType t);

	void RotateAroundWorldX(_TVarType angle);
	void RotateAroundWorldY(_TVarType angle);
	void RotateAroundWorldZ(_TVarType angle);

	void RotateAroundOwnX(_TVarType angle);
	void RotateAroundOwnY(_TVarType angle);
	void RotateAroundOwnZ(_TVarType angle);

	// Vector in world coordinates.
	void RotateAroundVector(const Vector3D<_TVarType>& vector, _TVarType angle);

	// Returns the rotation vector and angle that this quaternion represents.
	// The rotation vector isn't normalized.
	inline void GetRotationVector(Vector3D<_TVarType>& vector) const;
	inline Vector3D<_TVarType> GetRotationVector() const;
	_TVarType GetRotationAngle() const;
	void GetEulerAngles(Vector3D<_TVarType>& angles) const;
	void GetEulerAngles(_TVarType& yaw, _TVarType& pitch, _TVarType& roll) const;
	void SetEulerAngles(const Vector3D<_TVarType>& angles);
	void SetEulerAngles(_TVarType yaw, _TVarType pitch, _TVarType roll);

	// Axis X = Right
	// Axis Y = Up
	// Axis Z = Forward
	Vector3D<_TVarType> GetAxisX() const;
	Vector3D<_TVarType> GetAxisY() const;
	Vector3D<_TVarType> GetAxisZ() const;
	Vector3D<_TVarType> GetInverseAxisX() const;
	Vector3D<_TVarType> GetInverseAxisY() const;
	Vector3D<_TVarType> GetInverseAxisZ() const;

	inline Quaternion operator= (const Quaternion<_TVarType>& q);
	inline Quaternion operator= (const RotationMatrix<_TVarType>& rot_mtx);

	inline bool operator== (const Quaternion<_TVarType>& q) const;
	inline bool operator!= (const Quaternion<_TVarType>& q) const;

	inline Quaternion operator+= (const Quaternion<_TVarType>& q);
	inline Quaternion operator+ (const Quaternion<_TVarType>& q) const;
	inline Quaternion operator-= (const Quaternion<_TVarType>& q);
	inline Quaternion operator- (const Quaternion<_TVarType>& q) const;
	inline Quaternion operator- () const;

	// Multiplication
	inline Quaternion operator* (const Quaternion<_TVarType>& q) const;
	inline Quaternion operator*= (const Quaternion<_TVarType>& q);
	inline Quaternion operator* (_TVarType scalar) const;
	inline Quaternion operator*= (_TVarType scalar);

	inline Vector3D<_TVarType> operator* (const Vector3D<_TVarType>& v) const;
	// Division
	inline Quaternion operator/ (const Quaternion<_TVarType>& q) const;
	inline Quaternion operator/= (const Quaternion<_TVarType>& q);
	inline Quaternion operator/ (_TVarType scalar) const;
	inline Quaternion operator/= (_TVarType scalar);

	// The conjugate.
	inline Quaternion operator* () const;

	Quaternion<float> ToFloat() const;
	Quaternion<double> ToDouble() const;
};



#include "quaternion.inl"

typedef Quaternion<float32> quat;
typedef Quaternion<float64> QuaternionD;

extern const quat kIdentityQuaternionF;
extern const QuaternionD kIdentityQuaternionD;



#pragma warning(pop)



}
