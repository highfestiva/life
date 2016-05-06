/*
	Class:  RotationMatrix
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

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

#include "lepratypes.h"
#include "math.h"
#include "vector3d.h"

#define TEMPLATE template<class _TVarType>
#define QUAL RotationMatrix<_TVarType>



namespace lepra {



TEMPLATE
class RotationMatrix {
public:
	_TVarType matrix_[9];

	inline RotationMatrix();
	inline RotationMatrix(_TVarType p11, _TVarType p12, _TVarType p13,
			      _TVarType p21, _TVarType p22, _TVarType p23,
			      _TVarType p31, _TVarType p32, _TVarType p33);
	inline RotationMatrix(const Vector3D<_TVarType>& vector_x,
			      const Vector3D<_TVarType>& vector_y,
			      const Vector3D<_TVarType>& vector_z);
	inline RotationMatrix(const RotationMatrix& matrix, bool make_inverse = false);
	// Constructs the rotation matrix from two arbitrary vectors.
	RotationMatrix(const Vector3D<_TVarType>& vector_x,
		       const Vector3D<_TVarType>& vector_y);

	inline virtual ~RotationMatrix();

	inline void MakeIdentity();
	inline void MakeInverse();

	inline RotationMatrix GetInverse() const;

	void RotateAroundWorldX(_TVarType angle);
	void RotateAroundWorldY(_TVarType angle);
	void RotateAroundWorldZ(_TVarType angle);

	void RotateAroundOwnX(_TVarType angle);
	void RotateAroundOwnY(_TVarType angle);
	void RotateAroundOwnZ(_TVarType angle);

	// Vector in world coordinates.
	void RotateAroundVector(const Vector3D<_TVarType>& vector, _TVarType angle);

	// Returns the rotation vector and angle that this matrix represents.
	// M = this matrix, and I = identity matrix, then I.RotateAroundVector(v, a) = M.
	// where v and a are the returned vector and angle respectively.
	void GetRotationVector(Vector3D<_TVarType>& vector, _TVarType& angle) const;

	void Reorthogonalize();

	inline Vector3D<_TVarType> GetAxisX() const;
	inline Vector3D<_TVarType> GetAxisY() const;
	inline Vector3D<_TVarType> GetAxisZ() const;
	inline Vector3D<_TVarType> GetAxisByIndex(int index);
	inline Vector3D<_TVarType> GetInverseAxisX() const;
	inline Vector3D<_TVarType> GetInverseAxisY() const;
	inline Vector3D<_TVarType> GetInverseAxisZ() const;
	inline Vector3D<_TVarType> GetInverseAxisByIndex(int index);

	inline void SetAxisX(const Vector3D<_TVarType>& axis_x);
	inline void SetAxisY(const Vector3D<_TVarType>& axis_y);
	inline void SetAxisZ(const Vector3D<_TVarType>& axis_z);


	// Multiplication with vectors.
	inline Vector3D<_TVarType> GetRotatedVector(const Vector3D<_TVarType>& vector) const;
	inline Vector3D<_TVarType> GetInverseRotatedVector(const Vector3D<_TVarType>& vector) const;



	inline _TVarType GetElement(int index) const;
	inline _TVarType GetTrace() const;



	void Get3x3Array(_TVarType* array) const;
	void GetInverse3x3Array(_TVarType* array) const;

	void Get4x4Array(_TVarType* array) const;
	void GetInverse4x4Array(_TVarType* array) const;

	void Mul(const RotationMatrix& matrix);
	void Mul(const RotationMatrix& matrix1, const RotationMatrix& matrix2);
	void Mul(_TVarType scalar);

	// Special multiplication functions which can be used to optimize some calculations.
	// "this" is A, and the parameter matrix is B.
	void InvAMulB(const RotationMatrix& matrix);
	void InvAMulInvB(const RotationMatrix& matrix);
	void AMulInvB(const RotationMatrix& matrix);

	// Reverse multiplication. Thus, A' = B * A instead of A' = A * B.
	void BMulA(const RotationMatrix& matrix);
	void BMulInvA(const RotationMatrix& matrix);
	void InvBMulInvA(const RotationMatrix& matrix);
	void InvBMulA(const RotationMatrix& matrix);


	void Add(const RotationMatrix& matrix);
	void Add(const RotationMatrix& matrix1, const RotationMatrix& matrix2);
	void Add(_TVarType scalar);

	void Sub(const RotationMatrix& matrix);
	void Sub(const RotationMatrix& matrix1, const RotationMatrix& matrix2);
	void Sub(_TVarType scalar);

	void Set(const RotationMatrix& matrix);
	void Set(_TVarType* matrix);
	void Set(_TVarType p11, _TVarType p12, _TVarType p13,
		 _TVarType p21, _TVarType p22, _TVarType p23,
		 _TVarType p31, _TVarType p32, _TVarType p33);

	// Rotate vector...
	inline Vector3D<_TVarType> operator * (const Vector3D<_TVarType>& vector) const;

	// The data parameter must containt the number of bytes
	// returned by GetRawDataSize().
	inline int GetRawDataSize();
	int GetRawData(uint8* data);
	int SetRawData(uint8* data);


	// Operators
	inline bool operator == (const RotationMatrix& matrix);
	inline bool operator != (const RotationMatrix& matrix);

	inline const RotationMatrix& operator =  (const RotationMatrix& matrix);
	inline const RotationMatrix& operator += (const RotationMatrix& matrix);
	inline const RotationMatrix& operator -= (const RotationMatrix& matrix);
	inline const RotationMatrix& operator *= (const RotationMatrix& matrix);
	inline const RotationMatrix& operator /= (const RotationMatrix& matrix);
	inline RotationMatrix operator +  (const RotationMatrix& matrix) const;
	inline RotationMatrix operator -  (const RotationMatrix& matrix) const;
	inline RotationMatrix operator *  (const RotationMatrix& matrix) const;
	inline RotationMatrix operator /  (const RotationMatrix& matrix) const;

	inline RotationMatrix operator ! ();
	inline RotationMatrix operator- () const;

	inline const RotationMatrix& operator += (const _TVarType scalar);
	inline const RotationMatrix& operator -= (const _TVarType scalar);
	inline const RotationMatrix& operator *= (const _TVarType scalar);
	inline const RotationMatrix& operator /= (const _TVarType scalar);
	inline RotationMatrix operator +  (const _TVarType scalar) const;
	inline RotationMatrix operator -  (const _TVarType scalar) const;
	inline RotationMatrix operator *  (const _TVarType scalar) const;
	inline RotationMatrix operator /  (const _TVarType scalar) const;

	inline operator const _TVarType* () const;
	inline operator _TVarType* ();

	RotationMatrix<float> ToFloat() const;
	RotationMatrix<double> ToDouble() const;
};



TEMPLATE inline Vector3D<_TVarType>  operator *  (const Vector3D<_TVarType>& vec, const RotationMatrix<_TVarType>& mtx);
TEMPLATE inline Vector3D<_TVarType>& operator *= (Vector3D<_TVarType>& vec, const RotationMatrix<_TVarType>& mtx);



#include "rotationmatrix.inl"

typedef RotationMatrix<float> RotationMatrixF;
typedef RotationMatrix<double> RotationMatrixD;

const RotationMatrixF g3x3IdentityMatrixF(vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1));
const RotationMatrixD g3x3IdentityMatrixD(Vector3DD(1, 0, 0), Vector3DD(0, 1, 0), Vector3DD(0, 0, 1));



}

#undef TEMPLATE
#undef QUAL
