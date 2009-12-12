/*
	Class:  RotationMatrix
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	Following standard mathematical notations the rotation matrix consists
	of three orthonormal vectors which are stored as columns. Axes X, Y and
	Z are thus stored as follows:

	[X.x  Y.x  Z.x]
	[X.y  Y.y  Z.y]
	[X.z  Y.z  Z.z]

	And since the matrix itself is stored in a row major order (in memory),
	the array indices are:

	[0  1  2]
	[3  4  5]
	[6  7  8]

	This definition (and by following the standard definition for 
	multiplication) results in the following multiplication order (A is a 
	matrix and û is	a vector):

	û' = A * û   ->   û' is û transformed (rotated) as defined by A.
	û' = û * A   ->   û' is û inverse transformed (rotated backwards).
*/



#pragma once

#include "LepraTypes.h"
#include "Math.h"
#include "Vector3D.h"

#define TEMPLATE template<class _TVarType>
#define QUAL RotationMatrix<_TVarType>



namespace Lepra
{



TEMPLATE
class RotationMatrix
{
public:
	inline RotationMatrix();
	inline RotationMatrix(_TVarType p11, _TVarType p12, _TVarType p13,
			      _TVarType p21, _TVarType p22, _TVarType p23,
			      _TVarType p31, _TVarType p32, _TVarType p33);
	inline RotationMatrix(const Vector3D<_TVarType>& pVectorX, 
			      const Vector3D<_TVarType>& pVectorY, 
			      const Vector3D<_TVarType>& pVectorZ);
	inline RotationMatrix(const RotationMatrix& pMatrix, bool pMakeInverse = false);
	// Constructs the rotation matrix from two arbitrary vectors.
	RotationMatrix(const Vector3D<_TVarType>& pVectorX, 
		       const Vector3D<_TVarType>& pVectorY);

	inline virtual ~RotationMatrix();

	inline void MakeIdentity();
	inline void MakeInverse();

	inline RotationMatrix GetInverse() const;

	void RotateAroundWorldX(_TVarType pAngle);
	void RotateAroundWorldY(_TVarType pAngle);
	void RotateAroundWorldZ(_TVarType pAngle);

	void RotateAroundOwnX(_TVarType pAngle);
	void RotateAroundOwnY(_TVarType pAngle);
	void RotateAroundOwnZ(_TVarType pAngle);

	// Vector in world coordinates.
	void RotateAroundVector(const Vector3D<_TVarType>& pVector, _TVarType pAngle);
	
	// Returns the rotation vector and angle that this matrix represents.
	// M = this matrix, and I = identity matrix, then I.RotateAroundVector(v, a) = M.
	// where v and a are the returned vector and angle respectively.
	void GetRotationVector(Vector3D<_TVarType>& pVector, _TVarType& pAngle) const;

	void Reorthogonalize();

	inline Vector3D<_TVarType> GetAxisX() const;
	inline Vector3D<_TVarType> GetAxisY() const;
	inline Vector3D<_TVarType> GetAxisZ() const;
	inline Vector3D<_TVarType> GetAxisByIndex(int pIndex);
	inline Vector3D<_TVarType> GetInverseAxisX() const;
	inline Vector3D<_TVarType> GetInverseAxisY() const;
	inline Vector3D<_TVarType> GetInverseAxisZ() const;
	inline Vector3D<_TVarType> GetInverseAxisByIndex(int pIndex);

	inline void SetAxisX(const Vector3D<_TVarType>& pAxisX);
	inline void SetAxisY(const Vector3D<_TVarType>& pAxisY);
	inline void SetAxisZ(const Vector3D<_TVarType>& pAxisZ);


	// Multiplication with vectors.
	inline Vector3D<_TVarType> GetRotatedVector(const Vector3D<_TVarType>& pVector) const;
	inline Vector3D<_TVarType> GetInverseRotatedVector(const Vector3D<_TVarType>& pVector) const;



	inline _TVarType GetElement(int pIndex) const;
	inline _TVarType GetTrace() const;



	void Get3x3Array(_TVarType* pArray) const;
	void GetInverse3x3Array(_TVarType* pArray) const;

	void Get4x4Array(_TVarType* pArray) const;
	void GetInverse4x4Array(_TVarType* pArray) const;

	void Mul(const RotationMatrix& pMatrix);
	void Mul(const RotationMatrix& pMatrix1, const RotationMatrix& pMatrix2);
	void Mul(_TVarType pScalar);

	// Special multiplication functions which can be used to optimize some calculations.
	// "this" is A, and the parameter matrix is B.
	void InvAMulB(const RotationMatrix& pMatrix);
	void InvAMulInvB(const RotationMatrix& pMatrix);
	void AMulInvB(const RotationMatrix& pMatrix);

	// Reverse multiplication. Thus, A' = B * A instead of A' = A * B.
	void BMulA(const RotationMatrix& pMatrix);
	void BMulInvA(const RotationMatrix& pMatrix);
	void InvBMulInvA(const RotationMatrix& pMatrix);
	void InvBMulA(const RotationMatrix& pMatrix);


	void Add(const RotationMatrix& pMatrix);
	void Add(const RotationMatrix& pMatrix1, const RotationMatrix& pMatrix2);
	void Add(_TVarType pScalar);

	void Sub(const RotationMatrix& pMatrix);
	void Sub(const RotationMatrix& pMatrix1, const RotationMatrix& pMatrix2);
	void Sub(_TVarType pScalar);

	void Set(const RotationMatrix& pMatrix);
	void Set(_TVarType* pMatrix);
	void Set(_TVarType p11, _TVarType p12, _TVarType p13,
		 _TVarType p21, _TVarType p22, _TVarType p23,
		 _TVarType p31, _TVarType p32, _TVarType p33);

	// Rotate vector...
	inline Vector3D<_TVarType> operator * (const Vector3D<_TVarType>& pVector) const;

	// The pData parameter must containt the number of bytes
	// returned by GetRawDataSize().
	inline int GetRawDataSize();
	int GetRawData(uint8* pData);
	int SetRawData(uint8* pData);


	// Operators
	inline bool operator == (const RotationMatrix& pMatrix);
	inline bool operator != (const RotationMatrix& pMatrix);

	inline const RotationMatrix& operator =  (const RotationMatrix& pMatrix);
	inline const RotationMatrix& operator += (const RotationMatrix& pMatrix);
	inline const RotationMatrix& operator -= (const RotationMatrix& pMatrix);
	inline const RotationMatrix& operator *= (const RotationMatrix& pMatrix);
	inline const RotationMatrix& operator /= (const RotationMatrix& pMatrix);
	inline RotationMatrix operator +  (const RotationMatrix& pMatrix) const;
	inline RotationMatrix operator -  (const RotationMatrix& pMatrix) const;
	inline RotationMatrix operator *  (const RotationMatrix& pMatrix) const;
	inline RotationMatrix operator /  (const RotationMatrix& pMatrix) const;

	inline RotationMatrix operator ! ();
	inline RotationMatrix operator- () const;

	inline const RotationMatrix& operator += (const _TVarType pScalar);
	inline const RotationMatrix& operator -= (const _TVarType pScalar);
	inline const RotationMatrix& operator *= (const _TVarType pScalar);
	inline const RotationMatrix& operator /= (const _TVarType pScalar);
	inline RotationMatrix operator +  (const _TVarType pScalar) const;
	inline RotationMatrix operator -  (const _TVarType pScalar) const;
	inline RotationMatrix operator *  (const _TVarType pScalar) const;
	inline RotationMatrix operator /  (const _TVarType pScalar) const;

	inline operator const _TVarType* () const;
	inline operator _TVarType* ();

	RotationMatrix<float> ToFloat() const;
	RotationMatrix<double> ToDouble() const;
private:

	_TVarType mMatrix[9];
};



TEMPLATE inline Vector3D<_TVarType>  operator *  (const Vector3D<_TVarType>& pVec, const RotationMatrix<_TVarType>& pMtx);
TEMPLATE inline Vector3D<_TVarType>& operator *= (Vector3D<_TVarType>& pVec, const RotationMatrix<_TVarType>& pMtx);



#include "RotationMatrix.inl"

typedef RotationMatrix<float> RotationMatrixF;
typedef RotationMatrix<double> RotationMatrixD;

const RotationMatrixF g3x3IdentityMatrixF(Vector3DF(1, 0, 0), Vector3DF(0, 1, 0), Vector3DF(0, 0, 1));
const RotationMatrixD g3x3IdentityMatrixD(Vector3DD(1, 0, 0), Vector3DD(0, 1, 0), Vector3DD(0, 0, 1));



}

#undef TEMPLATE
#undef QUAL
