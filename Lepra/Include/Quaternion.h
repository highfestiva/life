/*
	Class:  Quaternion
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

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

#include "LepraTypes.h"
#include "Math.h"
#include "RotationMatrix.h"



namespace Lepra
{



template<class _TVarType>
class Quaternion
{
public:
	
	inline Quaternion();
	inline Quaternion(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD);
	inline Quaternion(const Quaternion& pQuaternion);
	inline Quaternion(const RotationMatrix<_TVarType>& pRotMtx);
	inline Quaternion(_TVarType pAngle, const Vector3D<_TVarType>& pVector);
	inline virtual ~Quaternion();

	inline void Set(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD);
	inline void Set(const Quaternion& pQuaternion);
	inline void SetConjugate(const Quaternion& pQuaternion);
	void Set(const RotationMatrix<_TVarType>& pRotMtx);
	inline void SetIdentity();

	// Angle of rotation around vector (pX, pY, pZ).
	inline void Set(_TVarType pAngle, const Vector3D<_TVarType>& pVector);

	inline _TVarType GetA() const;
	inline _TVarType GetB() const;
	inline _TVarType GetC() const;
	inline _TVarType GetD() const;

	RotationMatrix<_TVarType> GetAsRotationMatrix() const;
	void GetAsRotationMatrix(RotationMatrix<_TVarType>& pRotMtx) const;

	inline void Add(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD);
	inline void Add(const Quaternion& pQuaternion);

	inline void Sub(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD);
	inline void Sub(const Quaternion& pQuaternion);

	inline void Mul(_TVarType pX);
	inline void Div(_TVarType pX);

	inline void Mul(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD);
	inline void Mul(const Quaternion& pQuaternion);

	// Special multiplication functions which can be used to optimize some calculations.
	// "this" is A, and the parameter matrix is B.
	inline void AMulInvB(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD);
	inline void AMulInvB(const Quaternion& pQuaternion);
	inline void InvAMulB(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD);
	inline void InvAMulB(const Quaternion& pQuaternion);
	inline void InvAMulInvB(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD);
	inline void InvAMulInvB(const Quaternion& pQuaternion);

	inline void BMulA(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD);
	inline void BMulA(const Quaternion& pQuaternion);
	inline void InvBMulA(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD);
	inline void InvBMulA(const Quaternion& pQuaternion);
	inline void BMulInvA(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD);
	inline void BMulInvA(const Quaternion& pQuaternion);
	inline void InvBMulInvA(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD);
	inline void InvBMulInvA(const Quaternion& pQuaternion);

	inline void Div(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD);
	inline void Div(const Quaternion& pQuaternion);

	inline Vector3D<_TVarType> GetRotatedVector(const Vector3D<_TVarType>& pVector) const;
	inline Vector3D<_TVarType> GetInverseRotatedVector(const Vector3D<_TVarType>& pVector) const;

	inline void MakeConjugate();
	inline Quaternion GetConjugate() const;

	inline void MakeInverse();
	inline Quaternion GetInverse() const;

	inline void MakeSquare();
	inline Quaternion GetSquare() const;

	inline _TVarType GetNorm() const;
	inline _TVarType GetMagnitude() const;

	inline void Negate();
	inline void Normalize(_TVarType pLength = (_TVarType)1.0);

	// Slerp = Spherical Linear intERPolation. 
	// 0 <= pT <= 1
	void Slerp(const Quaternion<_TVarType>& pFrom,
		   const Quaternion<_TVarType>& pTo,
		   _TVarType pT);

	void RotateAroundWorldX(_TVarType pAngle);
	void RotateAroundWorldY(_TVarType pAngle);
	void RotateAroundWorldZ(_TVarType pAngle);

	void RotateAroundOwnX(_TVarType pAngle);
	void RotateAroundOwnY(_TVarType pAngle);
	void RotateAroundOwnZ(_TVarType pAngle);

	// Vector in world coordinates.
	void RotateAroundVector(const Vector3D<_TVarType>& pVector, _TVarType pAngle);

	// Returns the rotation vector and angle that this quaternion represents.
	// The rotation vector isn't normalized.
	inline void GetRotationVector(Vector3D<_TVarType>& pVector) const;
	inline Vector3D<_TVarType> GetRotationVector() const;
	_TVarType GetRotationAngle() const;
	void GetEulerAngles(Vector3D<_TVarType>& pAngles) const;
	void GetEulerAngles(_TVarType& pYaw, _TVarType& pPitch, _TVarType& pRoll) const;
	void SetEulerAngles(const Vector3D<_TVarType>& pAngles);
	void SetEulerAngles(_TVarType pYaw, _TVarType pPitch, _TVarType pRoll);

	// Axis X = Right
	// Axis Y = Up
	// Axis Z = Forward
	Vector3D<_TVarType> GetAxisX() const;
	Vector3D<_TVarType> GetAxisY() const;
	Vector3D<_TVarType> GetAxisZ() const;
	Vector3D<_TVarType> GetInverseAxisX() const;
	Vector3D<_TVarType> GetInverseAxisY() const;
	Vector3D<_TVarType> GetInverseAxisZ() const;

	inline Quaternion operator= (const Quaternion<_TVarType>& pQ);
	inline Quaternion operator= (const RotationMatrix<_TVarType>& pRotMtx);

	inline bool operator== (const Quaternion<_TVarType>& pQ) const;
	inline bool operator!= (const Quaternion<_TVarType>& pQ) const;
	
	inline Quaternion operator+= (const Quaternion<_TVarType>& pQ);
	inline Quaternion operator+ (const Quaternion<_TVarType>& pQ) const;
	inline Quaternion operator-= (const Quaternion<_TVarType>& pQ);
	inline Quaternion operator- (const Quaternion<_TVarType>& pQ) const;
	inline Quaternion operator- () const;

	// Multiplication
	inline Quaternion operator* (const Quaternion<_TVarType>& pQ) const;
	inline Quaternion operator*= (const Quaternion<_TVarType>& pQ);
	inline Quaternion operator* (_TVarType pScalar) const;
	inline Quaternion operator*= (_TVarType pScalar);

	inline Vector3D<_TVarType> operator* (const Vector3D<_TVarType>& pV) const;
	// Division
	inline Quaternion operator/ (const Quaternion<_TVarType>& pQ) const;
	inline Quaternion operator/= (const Quaternion<_TVarType>& pQ);
	inline Quaternion operator/ (_TVarType pScalar) const;
	inline Quaternion operator/= (_TVarType pScalar);

	// The conjugate.
	inline Quaternion operator* () const;

	Quaternion<float> ToFloat() const;
	Quaternion<double> ToDouble() const;

protected:

	_TVarType mA; // Real part.
	_TVarType mB; // Imaginary i
	_TVarType mC; // Imaginary j
	_TVarType mD; // Imaginary k
};



#include "Quaternion.inl"

typedef Quaternion<float32> QuaternionF;
typedef Quaternion<float64> QuaternionD;

}
