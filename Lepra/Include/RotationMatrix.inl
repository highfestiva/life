/*
	Class:  RotationMatrix
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

TEMPLATE QUAL::RotationMatrix()
{
	MakeIdentity();
}

TEMPLATE QUAL::RotationMatrix(_TVarType p11, _TVarType p12, _TVarType p13,
			      _TVarType p21, _TVarType p22, _TVarType p23,
			      _TVarType p31, _TVarType p32, _TVarType p33)
{
	mMatrix[0] = p11;
	mMatrix[1] = p12;
	mMatrix[2] = p13;

	mMatrix[3] = p21;
	mMatrix[4] = p22;
	mMatrix[5] = p23;

	mMatrix[6] = p31;
	mMatrix[7] = p32;
	mMatrix[8] = p33;
}

TEMPLATE QUAL::RotationMatrix(const Vector3D<_TVarType>& pVectorX, 
			      const Vector3D<_TVarType>& pVectorY, 
			      const Vector3D<_TVarType>& pVectorZ)
{
	mMatrix[0] = pVectorX.x;
	mMatrix[1] = pVectorY.x;
	mMatrix[2] = pVectorZ.x;

	mMatrix[3] = pVectorX.y;
	mMatrix[4] = pVectorY.y;
	mMatrix[5] = pVectorZ.y;

	mMatrix[6] = pVectorX.z;
	mMatrix[7] = pVectorY.z;
	mMatrix[8] = pVectorZ.z;
}

TEMPLATE QUAL::RotationMatrix(const Vector3D<_TVarType>& pVectorX, 
			      const Vector3D<_TVarType>& pVectorY)
{
	Vector3D<_TVarType> lVectorX(pVectorX);
	lVectorX.Normalize();

	Vector3D<_TVarType> lVectorY(pVectorY);
	lVectorY -= lVectorX * lVectorX.Dot(lVectorY);
	lVectorY.Normalize();
	
	Vector3D<_TVarType> lVectorZ(lVectorX, lVectorY);

	mMatrix[0] = lVectorX.x;
	mMatrix[1] = lVectorY.x;
	mMatrix[2] = lVectorZ.x;

	mMatrix[3] = lVectorX.y;
	mMatrix[4] = lVectorY.y;
	mMatrix[5] = lVectorZ.y;

	mMatrix[6] = lVectorX.z;
	mMatrix[7] = lVectorY.z;
	mMatrix[8] = lVectorZ.z;
}

TEMPLATE QUAL::RotationMatrix(const RotationMatrix& pMatrix, bool pMakeInverse /* = false */)
{
	if (pMakeInverse == false)
	{
		mMatrix[0] = pMatrix.mMatrix[0];
		mMatrix[1] = pMatrix.mMatrix[1];
		mMatrix[2] = pMatrix.mMatrix[2];

		mMatrix[3] = pMatrix.mMatrix[3];
		mMatrix[4] = pMatrix.mMatrix[4];
		mMatrix[5] = pMatrix.mMatrix[5];

		mMatrix[6] = pMatrix.mMatrix[6];
		mMatrix[7] = pMatrix.mMatrix[7];
		mMatrix[8] = pMatrix.mMatrix[8];
	}
	else
	{
		mMatrix[0] = pMatrix.mMatrix[0];
		mMatrix[1] = pMatrix.mMatrix[3];
		mMatrix[2] = pMatrix.mMatrix[6];

		mMatrix[3] = pMatrix.mMatrix[1];
		mMatrix[4] = pMatrix.mMatrix[4];
		mMatrix[5] = pMatrix.mMatrix[7];

		mMatrix[6] = pMatrix.mMatrix[2];
		mMatrix[7] = pMatrix.mMatrix[5];
		mMatrix[8] = pMatrix.mMatrix[8];
	}
}

TEMPLATE QUAL::~RotationMatrix()
{
}

TEMPLATE void QUAL::MakeIdentity()
{
	mMatrix[0] = 1.0f;
	mMatrix[1] = 0.0f;
	mMatrix[2] = 0.0f;

	mMatrix[3] = 0.0f;
	mMatrix[4] = 1.0f;
	mMatrix[5] = 0.0f;

	mMatrix[6] = 0.0f;
	mMatrix[7] = 0.0f;
	mMatrix[8] = 1.0f;
}

TEMPLATE void QUAL::MakeInverse()
{
	_TVarType lTemp;

	lTemp = mMatrix[3];
	mMatrix[3] = mMatrix[1];
	mMatrix[1] = lTemp;

	lTemp = mMatrix[6];
	mMatrix[6] = mMatrix[2];
	mMatrix[2] = lTemp;

	lTemp = mMatrix[7];
	mMatrix[7] = mMatrix[5];
	mMatrix[5] = lTemp;
}

TEMPLATE QUAL QUAL::GetInverse() const
{
	RotationMatrix lRotMtx(*this);
	lRotMtx.MakeInverse();

	return lRotMtx;
}

TEMPLATE Vector3D<_TVarType> QUAL::GetAxisX() const
{
	return Vector3D<_TVarType>(mMatrix[0], mMatrix[3], mMatrix[6]);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetAxisY() const
{
	return Vector3D<_TVarType>(mMatrix[1], mMatrix[4], mMatrix[7]);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetAxisZ() const
{
	return Vector3D<_TVarType>(mMatrix[2], mMatrix[5], mMatrix[8]);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetAxisByIndex(int pIndex)
{
	switch(pIndex)
	{
	case 0:
		return Vector3D<_TVarType>(mMatrix[0], mMatrix[3], mMatrix[6]);
	case 1:
		return Vector3D<_TVarType>(mMatrix[1], mMatrix[4], mMatrix[7]);
	case 2:
		return Vector3D<_TVarType>(mMatrix[2], mMatrix[5], mMatrix[8]);
	default:
		return Vector3D<_TVarType>(0, 0, 0);
	};
}

TEMPLATE Vector3D<_TVarType> QUAL::GetInverseAxisX() const
{
	return Vector3D<_TVarType>(mMatrix[0], mMatrix[1], mMatrix[2]);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetInverseAxisY() const
{
	return Vector3D<_TVarType>(mMatrix[3], mMatrix[4], mMatrix[5]);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetInverseAxisZ() const
{
	return Vector3D<_TVarType>(mMatrix[6], mMatrix[7], mMatrix[8]);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetInverseAxisByIndex(int pIndex)
{
	switch(pIndex)
	{
	case 0:
		return Vector3D<_TVarType>(mMatrix[0], mMatrix[1], mMatrix[2]);
	case 1:
		return Vector3D<_TVarType>(mMatrix[3], mMatrix[4], mMatrix[5]);
	case 2:
		return Vector3D<_TVarType>(mMatrix[6], mMatrix[7], mMatrix[8]);
	default:
		return Vector3D<_TVarType>(0, 0, 0);
	};
}

TEMPLATE void QUAL::SetAxisX(const Vector3D<_TVarType>& pAxisX)
{
	mMatrix[0] = pAxisX.x;
	mMatrix[3] = pAxisX.y;
	mMatrix[6] = pAxisX.z;
}

TEMPLATE void QUAL::SetAxisY(const Vector3D<_TVarType>& pAxisY)
{
	mMatrix[1] = pAxisY.x;
	mMatrix[4] = pAxisY.y;
	mMatrix[7] = pAxisY.z;
}

TEMPLATE void QUAL::SetAxisZ(const Vector3D<_TVarType>& pAxisZ)
{
	mMatrix[2] = pAxisZ.x;
	mMatrix[5] = pAxisZ.y;
	mMatrix[8] = pAxisZ.z;
}

TEMPLATE Vector3D<_TVarType> QUAL::GetRotatedVector(const Vector3D<_TVarType>& pVector) const
{
	// Normal rotation requires inverted rotation axes.
	return Vector3D<_TVarType>
	       (
		mMatrix[0] * pVector.x + mMatrix[1] * pVector.y + mMatrix[2] * pVector.z,
		mMatrix[3] * pVector.x + mMatrix[4] * pVector.y + mMatrix[5] * pVector.z,
		mMatrix[6] * pVector.x + mMatrix[7] * pVector.y + mMatrix[8] * pVector.z
	       );
}

TEMPLATE Vector3D<_TVarType> QUAL::GetInverseRotatedVector(const Vector3D<_TVarType>& pVector) const
{
	// Inverse rotation requires normal rotation axes.
	return Vector3D<_TVarType>
	       (
		mMatrix[0] * pVector.x + mMatrix[3] * pVector.y + mMatrix[6] * pVector.z,
		mMatrix[1] * pVector.x + mMatrix[4] * pVector.y + mMatrix[7] * pVector.z,
		mMatrix[2] * pVector.x + mMatrix[5] * pVector.y + mMatrix[8] * pVector.z
	       );
}

TEMPLATE _TVarType QUAL::GetElement(int pIndex) const
{
	return mMatrix[pIndex];
}

TEMPLATE _TVarType QUAL::GetTrace() const
{
	return mMatrix[0] + mMatrix[4] + mMatrix[8];
}

TEMPLATE bool QUAL::operator == (const RotationMatrix& pMatrix)
{
	return mMatrix[0] == pMatrix.mMatrix[0] &&
	       mMatrix[1] == pMatrix.mMatrix[1] &&
	       mMatrix[2] == pMatrix.mMatrix[2] &&
	       mMatrix[3] == pMatrix.mMatrix[3] &&
	       mMatrix[4] == pMatrix.mMatrix[4] &&
	       mMatrix[5] == pMatrix.mMatrix[5] &&
	       mMatrix[6] == pMatrix.mMatrix[6] &&
	       mMatrix[7] == pMatrix.mMatrix[7] &&
	       mMatrix[8] == pMatrix.mMatrix[8];
}

TEMPLATE bool QUAL::operator != (const RotationMatrix& pMatrix)
{
	return !(*this == pMatrix);
}

TEMPLATE const RotationMatrix<_TVarType>& QUAL::operator =  (const RotationMatrix& pMatrix)
{
	Set(pMatrix);
	return *this;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator +  (const RotationMatrix& pMatrix) const
{
	RotationMatrix lTemp(this);
	lTemp.Add(pMatrix);
	return lTemp;
}

TEMPLATE const RotationMatrix<_TVarType>& QUAL::operator += (const RotationMatrix& pMatrix)
{
	Add(pMatrix);
	return *this;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator -  (const RotationMatrix& pMatrix) const
{
	RotationMatrix lTemp(this);
	lTemp.Sub(pMatrix);
	return lTemp;
}

TEMPLATE const RotationMatrix<_TVarType>& QUAL::operator -= (const RotationMatrix& pMatrix)
{
	Sub(pMatrix);
	return *this;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator *  (const RotationMatrix& pMatrix) const
{
	RotationMatrix lTemp(*this);
	lTemp.Mul(pMatrix);
	return lTemp;
}

TEMPLATE const RotationMatrix<_TVarType>& QUAL::operator *= (const RotationMatrix& pMatrix)
{
	Mul(pMatrix);
	return *this;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator /  (const RotationMatrix& pMatrix) const
{
	RotationMatrix lTemp(*this);
	lTemp.Mul(pMatrix.GetInverse());
	return lTemp;
}

TEMPLATE const RotationMatrix<_TVarType>& QUAL::operator /= (const RotationMatrix& pMatrix)
{
	Mul(pMatrix.GetInverse());
	return *this;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator ! ()
{
	RotationMatrix lTemp(this);
	lTemp.MakeInverse();
	return lTemp;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator - () const
{
	RotationMatrix lTemp(this);
	lTemp.Mul(-1);
	return lTemp;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator +  (const _TVarType pScalar) const
{
	RotationMatrix lTemp(this);
	lTemp.Add(pScalar);
	return lTemp;
}

TEMPLATE const RotationMatrix<_TVarType>& QUAL::operator += (const _TVarType pScalar)
{
	Add(pScalar);
	return *this;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator -  (const _TVarType pScalar) const
{
	RotationMatrix lTemp(this);
	lTemp.Sub(pScalar);
	return lTemp;
}

TEMPLATE const RotationMatrix<_TVarType>& QUAL::operator -= (const _TVarType pScalar)
{
	Sub(pScalar);
	return *this;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator * (const _TVarType pScalar) const
{
	RotationMatrix lTemp(this);
	lTemp.Mul(pScalar);
	return lTemp;
}

TEMPLATE const RotationMatrix<_TVarType>& QUAL::operator *= (const _TVarType pScalar)
{
	Mul(pScalar);
	return *this;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::operator / (const _TVarType pScalar) const
{
	RotationMatrix lTemp(this);
	lTemp.Mul(1.0f / pScalar);
	return lTemp;
}

TEMPLATE const RotationMatrix<_TVarType>& QUAL::operator /= (const _TVarType pScalar)
{
	Mul(1.0f / pScalar);
	return *this;
}

TEMPLATE Vector3D<_TVarType> QUAL::operator * (const Vector3D<_TVarType>& pVector) const
{
	return GetRotatedVector(pVector);
}

TEMPLATE QUAL::operator const _TVarType* () const
{
	return mMatrix;
}

TEMPLATE QUAL::operator _TVarType* ()
{
	return mMatrix;
}

#if !defined(LEPRA_MSVC)

TEMPLATE inline Vector3D<_TVarType> operator * (const Vector3D<_TVarType>& pVector, const RotationMatrix<_TVarType>& pMtx)
{
	return pMtx.GetInverseRotatedVector(pVector);
}

TEMPLATE inline Vector3D<_TVarType>& operator *= (Vector3D<_TVarType>& pVector, const RotationMatrix<_TVarType>& pMtx)
{
	pVector = pMtx.GetInverseRotatedVector(pVector);
	return pVector;
}

#else // LEPRA_MSVC

// Visual Studio .NET can't handle function templates... (.NET 2003 can? .NET 2005 can!).
// Need to implement the functions separately like this.

inline Vector3D<float32> operator * (const Vector3D<float32>& pVector, const RotationMatrix<float32>& pMtx)
{
	return pMtx.GetInverseRotatedVector(pVector);
}

inline Vector3D<float32>& operator *= (Vector3D<float32>& pVector, const RotationMatrix<float32>& pMtx)
{
	pVector = pMtx.GetInverseRotatedVector(pVector);
	return pVector;
}

inline Vector3D<float64> operator * (const Vector3D<float64>& pVector, const RotationMatrix<float64>& pMtx)
{
	return pMtx.GetInverseRotatedVector(pVector);
}

inline Vector3D<float64>& operator *= (Vector3D<float64>& pVector, const RotationMatrix<float64>& pMtx)
{
	pVector = pMtx.GetInverseRotatedVector(pVector);
	return pVector;
}

#endif // !LEPRA_MSVC/LEPRA_MSVC



TEMPLATE void QUAL::RotateAroundWorldX(_TVarType pAngle)
{
	_TVarType lCosA = (_TVarType)cos(pAngle);
	_TVarType lSinA = (_TVarType)sin(pAngle);

	_TVarType lY, lZ;

	lY = mMatrix[3]; // X.y
	lZ = mMatrix[6]; // X.z
	mMatrix[3] = (lCosA * lY - lSinA * lZ);
	mMatrix[6] = (lSinA * lY + lCosA * lZ);

	lY = mMatrix[4]; // Y.y
	lZ = mMatrix[7]; // Y.z
	mMatrix[4] = (lCosA * lY - lSinA * lZ);
	mMatrix[7] = (lSinA * lY + lCosA * lZ);

	lY = mMatrix[5]; // Z.y
	lZ = mMatrix[8]; // Z.z
	mMatrix[5] = (lCosA * lY - lSinA * lZ);
	mMatrix[8] = (lSinA * lY + lCosA * lZ);
}

TEMPLATE void QUAL::RotateAroundWorldY(_TVarType pAngle)
{
	_TVarType lCosA = (_TVarType)cos(pAngle);
	_TVarType lSinA = (_TVarType)sin(pAngle);

	_TVarType lX, lZ;

	lX = mMatrix[0]; // X.x
	lZ = mMatrix[6]; // X.z
	mMatrix[0] = (lSinA * lZ + lCosA * lX);
	mMatrix[6] = (lCosA * lZ - lSinA * lX);

	lX = mMatrix[1]; // Y.x
	lZ = mMatrix[7]; // Y.z
	mMatrix[1] = (lSinA * lZ + lCosA * lX);
	mMatrix[7] = (lCosA * lZ - lSinA * lX);

	lX = mMatrix[2]; // Z.x
	lZ = mMatrix[8]; // Z.z
	mMatrix[2] = (lSinA * lZ + lCosA * lX);
	mMatrix[8] = (lCosA * lZ - lSinA * lX);
}

TEMPLATE void QUAL::RotateAroundWorldZ(_TVarType pAngle)
{
	_TVarType lCosA = (_TVarType)cos(pAngle);
	_TVarType lSinA = (_TVarType)sin(pAngle);

	_TVarType lX, lY;

	lX = mMatrix[0]; // X.x
	lY = mMatrix[3]; // X.y
	mMatrix[0] = lCosA * lX - lSinA * lY;
	mMatrix[3] = lSinA * lX + lCosA * lY;

	lX = mMatrix[1]; // Y.x
	lY = mMatrix[4]; // Y.y
	mMatrix[1] = lCosA * lX - lSinA * lY;
	mMatrix[4] = lSinA * lX + lCosA * lY;

	lX = mMatrix[2]; // Z.x
	lY = mMatrix[5]; // Z.y
	mMatrix[2] = lCosA * lX - lSinA * lY;
	mMatrix[5] = lSinA * lX + lCosA * lY;
}

TEMPLATE void QUAL::RotateAroundOwnX(_TVarType pAngle)
{
	_TVarType lCosA = (_TVarType)cos(pAngle);
	_TVarType lSinA = (_TVarType)sin(pAngle);

	Mul(RotationMatrix(1,  0,       0,
			   0,  lCosA,-lSinA,
			   0,  lSinA, lCosA));
}

TEMPLATE void QUAL::RotateAroundOwnY(_TVarType pAngle)
{
	_TVarType lCosA = (_TVarType)cos(pAngle);
	_TVarType lSinA = (_TVarType)sin(pAngle);

	Mul(RotationMatrix( lCosA,  0,  lSinA,
			    0,        1,  0,
			   -lSinA,  0,  lCosA));
}

TEMPLATE void QUAL::RotateAroundOwnZ(_TVarType pAngle)
{
	_TVarType lCosA = (_TVarType)cos(pAngle);
	_TVarType lSinA = (_TVarType)sin(pAngle);

	Mul(RotationMatrix(lCosA, -lSinA, 0,
			   lSinA,  lCosA, 0,
			   0,        0,       1));
}

TEMPLATE void QUAL::RotateAroundVector(const Vector3D<_TVarType>& pVector, _TVarType pAngle)
{
	if (pVector.IsNullVector() == true)
	{
		return;
	}

	Vector3D<_TVarType> lXAxis(pVector);
	Vector3D<_TVarType> lYAxis;
	Vector3D<_TVarType> lZAxis;

	// Generate a y- and a z-axis.

	lXAxis.Normalize();

	// Check if pVector is parallel to the world y-axis. No matter what, the resulting
	// coordinate system will always have the same orientation.
	_TVarType lAbsDot = (_TVarType)fabs(lXAxis.Dot(Vector3D<_TVarType>(0.0f, 1.0f, 0.0f)));

	if ((1.0f - lAbsDot) > 1e-6f)
	{
		// It wasn't parallel. Generate the z-axis first.
		lZAxis.CrossUnit(lXAxis, Vector3D<_TVarType>(0.0f, 1.0f, 0.0f));
		lYAxis.CrossUnit(lZAxis, lXAxis);
	}
	else
	{
		// It was parallel. Generate the y-axis first.
		lYAxis.CrossUnit(Vector3D<_TVarType>(0.0f, 0.0f, 1.0f), lXAxis);
		lZAxis.CrossUnit(lXAxis, lYAxis);
	}

	RotationMatrix lMtx(lXAxis, lYAxis, lZAxis);

	InvBMulA(lMtx);	// this = Mtx' * this

	RotateAroundWorldX(pAngle);

	BMulA(lMtx);	// this = Mtx * this
}

TEMPLATE void QUAL::GetRotationVector(Vector3D<_TVarType>& pVector, _TVarType& pAngle) const
{
	_TVarType lCosA = (mMatrix[0] + mMatrix[4] + mMatrix[8] - (_TVarType)1.0) * (_TVarType)0.5;
	pAngle = (_TVarType)acos(lCosA);

	pVector.x = mMatrix[7] - mMatrix[5];
	pVector.y = mMatrix[2] - mMatrix[6];
	pVector.z = mMatrix[3] - mMatrix[1];

	if (pVector.IsNullVector() == true)
	{
		// We can get a null vector in two cases:
		// 1. The matrix is the identity matrix, in which case we don't care about the rotation vector.
		// 2. The matrix is rotated exactly 180 degrees compared to the identity matrix,
		//    and we need to find the rotation vector by checking the rotation axes.

		const _TVarType lEpsilonOne = 1.0f - 1e-6f;

		// AxisX.x
		if (mMatrix[0] >= lEpsilonOne)
		{
			pVector.x = 1.0f;
			pVector.y = 0.0f;
			pVector.z = 0.0f;
		}
		// AxisY.y
		else if(mMatrix[4] >= lEpsilonOne)
		{
			pVector.x = 0.0f;
			pVector.y = 1.0f;
			pVector.z = 0.0f;
		}
		// AxisZ.z
		else if(mMatrix[8] >= lEpsilonOne)
		{
			pVector.x = 0.0f;
			pVector.y = 0.0f;
			pVector.z = 1.0f;
		}
		else
		{
			// ???
			pVector.x = 1.0f;
			pVector.y = 1.0f;
			pVector.z = 1.0f;
		}
	}
}

TEMPLATE void QUAL::Reorthogonalize()
{

	//
	// Step 1. (Fix the x-axis)
	//

	// Normalize x-axis
	_TVarType lInvLength = (_TVarType)(1.0 / sqrt(mMatrix[0] * mMatrix[0] + mMatrix[1] * mMatrix[1] + mMatrix[2] * mMatrix[2]));

	mMatrix[0] *= lInvLength;
	mMatrix[1] *= lInvLength;
	mMatrix[2] *= lInvLength;

	//
	// Step 2. (Fix the y-axis)
	//

	// Calculate the dot product between the x- and the y-axis.
	_TVarType lDot = mMatrix[0] * mMatrix[3] + mMatrix[1] * mMatrix[4] + mMatrix[2] * mMatrix[5];

	// Make the y-axis orthogonal to the x-axis.
	mMatrix[3] -= lDot * mMatrix[0];
	mMatrix[4] -= lDot * mMatrix[1];
	mMatrix[5] -= lDot * mMatrix[2];

	// Normalize y-axis
	lInvLength = (_TVarType)(1.0 / sqrt(mMatrix[3] * mMatrix[3] + mMatrix[4] * mMatrix[4] + mMatrix[5] * mMatrix[5]));

	mMatrix[3] *= lInvLength;
	mMatrix[4] *= lInvLength;
	mMatrix[5] *= lInvLength;

	//
	// Step 3. (Fix the z-axis)
	//

	// Calculate the dot product between the x- and the z-axis.
	lDot = mMatrix[0] * mMatrix[6] + mMatrix[1] * mMatrix[7] + mMatrix[2] * mMatrix[8];

	// Make the z-axis orthogonal to the x-axis.
	mMatrix[6] -= lDot * mMatrix[0];
	mMatrix[7] -= lDot * mMatrix[1];
	mMatrix[8] -= lDot * mMatrix[2];

	// Calculate the dot product between the y- and the z-axis.
	lDot = mMatrix[3] * mMatrix[6] + mMatrix[4] * mMatrix[7] + mMatrix[5] * mMatrix[8];

	// Make the z-axis orthogonal to the y-axis.
	mMatrix[6] -= lDot * mMatrix[3];
	mMatrix[7] -= lDot * mMatrix[4];
	mMatrix[8] -= lDot * mMatrix[5];

	// Normalize z-axis
	lInvLength = (_TVarType)(1.0 / sqrt(mMatrix[6] * mMatrix[6] + mMatrix[7] * mMatrix[7] + mMatrix[8] * mMatrix[8]));

	mMatrix[6] *= lInvLength;
	mMatrix[7] *= lInvLength;
	mMatrix[8] *= lInvLength;
}

TEMPLATE void QUAL::Get3x3Array(_TVarType* pArray) const
{
	pArray[0] = mMatrix[0];
	pArray[1] = mMatrix[1];
	pArray[2] = mMatrix[2];

	pArray[3] = mMatrix[3];
	pArray[4] = mMatrix[4];
	pArray[5] = mMatrix[5];

	pArray[6] = mMatrix[6];
	pArray[7] = mMatrix[7];
	pArray[8] = mMatrix[8];
}

TEMPLATE void QUAL::GetInverse3x3Array(_TVarType* pArray) const
{
	pArray[0] = mMatrix[0];
	pArray[1] = mMatrix[3];
	pArray[2] = mMatrix[6];

	pArray[3] = mMatrix[1];
	pArray[4] = mMatrix[4];
	pArray[5] = mMatrix[7];

	pArray[6] = mMatrix[2];
	pArray[7] = mMatrix[5];
	pArray[8] = mMatrix[8];
}

TEMPLATE void QUAL::Get4x4Array(_TVarType* pArray) const
{
	pArray[0] = mMatrix[0];
	pArray[1] = mMatrix[1];
	pArray[2] = mMatrix[2];
	pArray[3]  = 0.0f;

	pArray[4] = mMatrix[3];
	pArray[5] = mMatrix[4];
	pArray[6] = mMatrix[5];
	pArray[7]  = 0.0f;

	pArray[8]  = mMatrix[6];
	pArray[9]  = mMatrix[7];
	pArray[10] = mMatrix[8];
	pArray[11] = 0.0f;

	pArray[12] = 0.0f;
	pArray[13] = 0.0f;
	pArray[14] = 0.0f;
	pArray[15] = 1.0f;
}

TEMPLATE void QUAL::GetInverse4x4Array(_TVarType* pArray) const
{
	pArray[0]  = mMatrix[0];
	pArray[1]  = mMatrix[3];
	pArray[2]  = mMatrix[6];
	pArray[3]  = 0.0f;

	pArray[4]  = mMatrix[1];
	pArray[5]  = mMatrix[4];
	pArray[6]  = mMatrix[7];
	pArray[7]  = 0.0f;

	pArray[8]  = mMatrix[2];
	pArray[9]  = mMatrix[5];
	pArray[10] = mMatrix[8];
	pArray[11] = 0.0f;

	pArray[12] = 0.0f;
	pArray[13] = 0.0f;
	pArray[14] = 0.0f;
	pArray[15] = 1.0f;
}

TEMPLATE void QUAL::Mul(const RotationMatrix& pMatrix)
{
	_TVarType lTemp[9];

	lTemp[0] = mMatrix[0] * pMatrix.mMatrix[0] +
		     mMatrix[1] * pMatrix.mMatrix[3] +
		     mMatrix[2] * pMatrix.mMatrix[6];
	lTemp[1] = mMatrix[0] * pMatrix.mMatrix[1] +
		     mMatrix[1] * pMatrix.mMatrix[4] +
		     mMatrix[2] * pMatrix.mMatrix[7];
	lTemp[2] = mMatrix[0] * pMatrix.mMatrix[2] +
		     mMatrix[1] * pMatrix.mMatrix[5] +
		     mMatrix[2] * pMatrix.mMatrix[8];

	lTemp[3] = mMatrix[3] * pMatrix.mMatrix[0] +
		     mMatrix[4] * pMatrix.mMatrix[3] +
		     mMatrix[5] * pMatrix.mMatrix[6];
	lTemp[4] = mMatrix[3] * pMatrix.mMatrix[1] +
		     mMatrix[4] * pMatrix.mMatrix[4] +
		     mMatrix[5] * pMatrix.mMatrix[7];
	lTemp[5] = mMatrix[3] * pMatrix.mMatrix[2] +
		     mMatrix[4] * pMatrix.mMatrix[5] +
		     mMatrix[5] * pMatrix.mMatrix[8];

	lTemp[6] = mMatrix[6] * pMatrix.mMatrix[0] +
		     mMatrix[7] * pMatrix.mMatrix[3] +
		     mMatrix[8] * pMatrix.mMatrix[6];
	lTemp[7] = mMatrix[6] * pMatrix.mMatrix[1] +
		     mMatrix[7] * pMatrix.mMatrix[4] +
		     mMatrix[8] * pMatrix.mMatrix[7];
	lTemp[8] = mMatrix[6] * pMatrix.mMatrix[2] +
		     mMatrix[7] * pMatrix.mMatrix[5] +
		     mMatrix[8] * pMatrix.mMatrix[8];

	mMatrix[0] = lTemp[0];
	mMatrix[1] = lTemp[1];
	mMatrix[2] = lTemp[2];
	mMatrix[3] = lTemp[3];
	mMatrix[4] = lTemp[4];
	mMatrix[5] = lTemp[5];
	mMatrix[6] = lTemp[6];
	mMatrix[7] = lTemp[7];
	mMatrix[8] = lTemp[8];
}

TEMPLATE void QUAL::Mul(const RotationMatrix& pMatrix1, const RotationMatrix& pMatrix2)
{
	mMatrix[0] = pMatrix1.mMatrix[0] * pMatrix2.mMatrix[0] +
			pMatrix1.mMatrix[1] * pMatrix2.mMatrix[3] +
			pMatrix1.mMatrix[2] * pMatrix2.mMatrix[6];
	mMatrix[1] = pMatrix1.mMatrix[0] * pMatrix2.mMatrix[1] +
			pMatrix1.mMatrix[1] * pMatrix2.mMatrix[4] +
			pMatrix1.mMatrix[2] * pMatrix2.mMatrix[7];
	mMatrix[2] = pMatrix1.mMatrix[0] * pMatrix2.mMatrix[2] +
			pMatrix1.mMatrix[1] * pMatrix2.mMatrix[5] +
			pMatrix1.mMatrix[2] * pMatrix2.mMatrix[8];

	mMatrix[3] = pMatrix1.mMatrix[3] * pMatrix2.mMatrix[0] +
			pMatrix1.mMatrix[4] * pMatrix2.mMatrix[3] +
			pMatrix1.mMatrix[5] * pMatrix2.mMatrix[6];
	mMatrix[4] = pMatrix1.mMatrix[3] * pMatrix2.mMatrix[1] +
			pMatrix1.mMatrix[4] * pMatrix2.mMatrix[4] +
			pMatrix1.mMatrix[5] * pMatrix2.mMatrix[7];
	mMatrix[5] = pMatrix1.mMatrix[3] * pMatrix2.mMatrix[2] +
			pMatrix1.mMatrix[4] * pMatrix2.mMatrix[5] +
			pMatrix1.mMatrix[5] * pMatrix2.mMatrix[8];

	mMatrix[6] = pMatrix1.mMatrix[6] * pMatrix2.mMatrix[0] +
			pMatrix1.mMatrix[7] * pMatrix2.mMatrix[3] +
			pMatrix1.mMatrix[8] * pMatrix2.mMatrix[6];
	mMatrix[7] = pMatrix1.mMatrix[6] * pMatrix2.mMatrix[1] +
			pMatrix1.mMatrix[7] * pMatrix2.mMatrix[4] +
			pMatrix1.mMatrix[8] * pMatrix2.mMatrix[7];
	mMatrix[8] = pMatrix1.mMatrix[6] * pMatrix2.mMatrix[2] +
			pMatrix1.mMatrix[7] * pMatrix2.mMatrix[5] +
			pMatrix1.mMatrix[8] * pMatrix2.mMatrix[8];
}

TEMPLATE void QUAL::Mul(_TVarType pScalar)
{
	mMatrix[0] *= pScalar;
	mMatrix[1] *= pScalar;
	mMatrix[2] *= pScalar;

	mMatrix[3] *= pScalar;
	mMatrix[4] *= pScalar;
	mMatrix[5] *= pScalar;

	mMatrix[6] *= pScalar;
	mMatrix[7] *= pScalar;
	mMatrix[8] *= pScalar;
}

TEMPLATE void QUAL::InvAMulB(const RotationMatrix& pMatrix)
{
	_TVarType lTemp[9];

	lTemp[0] = mMatrix[0] * pMatrix.mMatrix[0] +
		     mMatrix[3] * pMatrix.mMatrix[3] +
		     mMatrix[6] * pMatrix.mMatrix[6];
	lTemp[1] = mMatrix[0] * pMatrix.mMatrix[1] +
		     mMatrix[3] * pMatrix.mMatrix[4] +
		     mMatrix[6] * pMatrix.mMatrix[7];
	lTemp[2] = mMatrix[0] * pMatrix.mMatrix[2] +
		     mMatrix[3] * pMatrix.mMatrix[5] +
		     mMatrix[6] * pMatrix.mMatrix[8];

	lTemp[3] = mMatrix[1] * pMatrix.mMatrix[0] +
		     mMatrix[4] * pMatrix.mMatrix[3] +
		     mMatrix[7] * pMatrix.mMatrix[6];
	lTemp[4] = mMatrix[1] * pMatrix.mMatrix[1] +
		     mMatrix[4] * pMatrix.mMatrix[4] +
		     mMatrix[7] * pMatrix.mMatrix[7];
	lTemp[5] = mMatrix[1] * pMatrix.mMatrix[2] +
		     mMatrix[4] * pMatrix.mMatrix[5] +
		     mMatrix[7] * pMatrix.mMatrix[8];

	lTemp[6] = mMatrix[2] * pMatrix.mMatrix[0] +
		     mMatrix[5] * pMatrix.mMatrix[3] +
		     mMatrix[8] * pMatrix.mMatrix[6];
	lTemp[7] = mMatrix[2] * pMatrix.mMatrix[1] +
		     mMatrix[5] * pMatrix.mMatrix[4] +
		     mMatrix[8] * pMatrix.mMatrix[7];
	lTemp[8] = mMatrix[2] * pMatrix.mMatrix[2] +
		     mMatrix[5] * pMatrix.mMatrix[5] +
		     mMatrix[8] * pMatrix.mMatrix[8];

	mMatrix[0] = lTemp[0];
	mMatrix[1] = lTemp[1];
	mMatrix[2] = lTemp[2];
	mMatrix[3] = lTemp[3];
	mMatrix[4] = lTemp[4];
	mMatrix[5] = lTemp[5];
	mMatrix[6] = lTemp[6];
	mMatrix[7] = lTemp[7];
	mMatrix[8] = lTemp[8];
}

TEMPLATE void QUAL::InvAMulInvB(const RotationMatrix& pMatrix)
{
	_TVarType lTemp[9];

	lTemp[0] = mMatrix[0] * pMatrix.mMatrix[0] +
		     mMatrix[3] * pMatrix.mMatrix[1] +
		     mMatrix[6] * pMatrix.mMatrix[2];
	lTemp[1] = mMatrix[0] * pMatrix.mMatrix[3] +
		     mMatrix[3] * pMatrix.mMatrix[4] +
		     mMatrix[6] * pMatrix.mMatrix[5];
	lTemp[2] = mMatrix[0] * pMatrix.mMatrix[6] +
		     mMatrix[3] * pMatrix.mMatrix[7] +
		     mMatrix[6] * pMatrix.mMatrix[8];

	lTemp[3] = mMatrix[1] * pMatrix.mMatrix[0] +
		     mMatrix[4] * pMatrix.mMatrix[1] +
		     mMatrix[7] * pMatrix.mMatrix[2];
	lTemp[4] = mMatrix[1] * pMatrix.mMatrix[3] +
		     mMatrix[4] * pMatrix.mMatrix[4] +
		     mMatrix[7] * pMatrix.mMatrix[5];
	lTemp[5] = mMatrix[1] * pMatrix.mMatrix[6] +
		     mMatrix[4] * pMatrix.mMatrix[7] +
		     mMatrix[7] * pMatrix.mMatrix[8];

	lTemp[6] = mMatrix[2] * pMatrix.mMatrix[0] +
		     mMatrix[5] * pMatrix.mMatrix[1] +
		     mMatrix[8] * pMatrix.mMatrix[2];
	lTemp[7] = mMatrix[2] * pMatrix.mMatrix[3] +
		     mMatrix[5] * pMatrix.mMatrix[4] +
		     mMatrix[8] * pMatrix.mMatrix[5];
	lTemp[8] = mMatrix[2] * pMatrix.mMatrix[6] +
		     mMatrix[5] * pMatrix.mMatrix[7] +
		     mMatrix[8] * pMatrix.mMatrix[8];

	mMatrix[0] = lTemp[0];
	mMatrix[1] = lTemp[1];
	mMatrix[2] = lTemp[2];
	mMatrix[3] = lTemp[3];
	mMatrix[4] = lTemp[4];
	mMatrix[5] = lTemp[5];
	mMatrix[6] = lTemp[6];
	mMatrix[7] = lTemp[7];
	mMatrix[8] = lTemp[8];
}

TEMPLATE void QUAL::AMulInvB(const RotationMatrix& pMatrix)
{
	_TVarType lTemp[9];

	lTemp[0] = mMatrix[0] * pMatrix.mMatrix[0] +
		     mMatrix[1] * pMatrix.mMatrix[1] +
		     mMatrix[2] * pMatrix.mMatrix[2];
	lTemp[1] = mMatrix[0] * pMatrix.mMatrix[3] +
		     mMatrix[1] * pMatrix.mMatrix[4] +
		     mMatrix[2] * pMatrix.mMatrix[5];
	lTemp[2] = mMatrix[0] * pMatrix.mMatrix[6] +
		     mMatrix[1] * pMatrix.mMatrix[7] +
		     mMatrix[2] * pMatrix.mMatrix[8];

	lTemp[3] = mMatrix[3] * pMatrix.mMatrix[0] +
		     mMatrix[4] * pMatrix.mMatrix[1] +
		     mMatrix[5] * pMatrix.mMatrix[2];
	lTemp[4] = mMatrix[3] * pMatrix.mMatrix[3] +
		     mMatrix[4] * pMatrix.mMatrix[4] +
		     mMatrix[5] * pMatrix.mMatrix[5];
	lTemp[5] = mMatrix[3] * pMatrix.mMatrix[6] +
		     mMatrix[4] * pMatrix.mMatrix[7] +
		     mMatrix[5] * pMatrix.mMatrix[8];

	lTemp[6] = mMatrix[6] * pMatrix.mMatrix[0] +
		     mMatrix[7] * pMatrix.mMatrix[1] +
		     mMatrix[8] * pMatrix.mMatrix[2];
	lTemp[7] = mMatrix[6] * pMatrix.mMatrix[3] +
		     mMatrix[7] * pMatrix.mMatrix[4] +
		     mMatrix[8] * pMatrix.mMatrix[5];
	lTemp[8] = mMatrix[6] * pMatrix.mMatrix[6] +
		     mMatrix[7] * pMatrix.mMatrix[7] +
		     mMatrix[8] * pMatrix.mMatrix[8];

	mMatrix[0] = lTemp[0];
	mMatrix[1] = lTemp[1];
	mMatrix[2] = lTemp[2];
	mMatrix[3] = lTemp[3];
	mMatrix[4] = lTemp[4];
	mMatrix[5] = lTemp[5];
	mMatrix[6] = lTemp[6];
	mMatrix[7] = lTemp[7];
	mMatrix[8] = lTemp[8];
}



TEMPLATE void QUAL::BMulA(const RotationMatrix& pMatrix)
{
	_TVarType lTemp[9];

	lTemp[0] = pMatrix.mMatrix[0] * mMatrix[0] +
		     pMatrix.mMatrix[1] * mMatrix[3] +
		     pMatrix.mMatrix[2] * mMatrix[6];
	lTemp[1] = pMatrix.mMatrix[0] * mMatrix[1] +
		     pMatrix.mMatrix[1] * mMatrix[4] +
		     pMatrix.mMatrix[2] * mMatrix[7];
	lTemp[2] = pMatrix.mMatrix[0] * mMatrix[2] +
		     pMatrix.mMatrix[1] * mMatrix[5] +
		     pMatrix.mMatrix[2] * mMatrix[8];

	lTemp[3] = pMatrix.mMatrix[3] * mMatrix[0] +
		     pMatrix.mMatrix[4] * mMatrix[3] +
		     pMatrix.mMatrix[5] * mMatrix[6];
	lTemp[4] = pMatrix.mMatrix[3] * mMatrix[1] +
		     pMatrix.mMatrix[4] * mMatrix[4] +
		     pMatrix.mMatrix[5] * mMatrix[7];
	lTemp[5] = pMatrix.mMatrix[3] * mMatrix[2] +
		     pMatrix.mMatrix[4] * mMatrix[5] +
		     pMatrix.mMatrix[5] * mMatrix[8];

	lTemp[6] = pMatrix.mMatrix[6] * mMatrix[0] +
		     pMatrix.mMatrix[7] * mMatrix[3] +
		     pMatrix.mMatrix[8] * mMatrix[6];
	lTemp[7] = pMatrix.mMatrix[6] * mMatrix[1] +
		     pMatrix.mMatrix[7] * mMatrix[4] +
		     pMatrix.mMatrix[8] * mMatrix[7];
	lTemp[8] = pMatrix.mMatrix[6] * mMatrix[2] +
		     pMatrix.mMatrix[7] * mMatrix[5] +
		     pMatrix.mMatrix[8] * mMatrix[8];

	mMatrix[0] = lTemp[0];
	mMatrix[1] = lTemp[1];
	mMatrix[2] = lTemp[2];
	mMatrix[3] = lTemp[3];
	mMatrix[4] = lTemp[4];
	mMatrix[5] = lTemp[5];
	mMatrix[6] = lTemp[6];
	mMatrix[7] = lTemp[7];
	mMatrix[8] = lTemp[8];
}



TEMPLATE void QUAL::BMulInvA(const RotationMatrix& pMatrix)
{
	_TVarType lTemp[9];

	lTemp[0] = pMatrix.mMatrix[0] * mMatrix[0] +
		     pMatrix.mMatrix[1] * mMatrix[3] +
		     pMatrix.mMatrix[2] * mMatrix[6];
	lTemp[1] = pMatrix.mMatrix[0] * mMatrix[1] +
		     pMatrix.mMatrix[1] * mMatrix[4] +
		     pMatrix.mMatrix[2] * mMatrix[7];
	lTemp[2] = pMatrix.mMatrix[0] * mMatrix[2] +
		     pMatrix.mMatrix[1] * mMatrix[5] +
		     pMatrix.mMatrix[2] * mMatrix[8];

	lTemp[3] = pMatrix.mMatrix[3] * mMatrix[0] +
		     pMatrix.mMatrix[4] * mMatrix[3] +
		     pMatrix.mMatrix[5] * mMatrix[6];
	lTemp[4] = pMatrix.mMatrix[3] * mMatrix[1] +
		     pMatrix.mMatrix[4] * mMatrix[4] +
		     pMatrix.mMatrix[5] * mMatrix[7];
	lTemp[5] = pMatrix.mMatrix[3] * mMatrix[2] +
		     pMatrix.mMatrix[4] * mMatrix[5] +
		     pMatrix.mMatrix[5] * mMatrix[8];

	lTemp[6] = pMatrix.mMatrix[6] * mMatrix[0] +
		     pMatrix.mMatrix[7] * mMatrix[3] +
		     pMatrix.mMatrix[8] * mMatrix[6];
	lTemp[7] = pMatrix.mMatrix[6] * mMatrix[1] +
		     pMatrix.mMatrix[7] * mMatrix[4] +
		     pMatrix.mMatrix[8] * mMatrix[7];
	lTemp[8] = pMatrix.mMatrix[6] * mMatrix[2] +
		     pMatrix.mMatrix[7] * mMatrix[5] +
		     pMatrix.mMatrix[8] * mMatrix[8];

	mMatrix[0] = lTemp[0];
	mMatrix[1] = lTemp[1];
	mMatrix[2] = lTemp[2];
	mMatrix[3] = lTemp[3];
	mMatrix[4] = lTemp[4];
	mMatrix[5] = lTemp[5];
	mMatrix[6] = lTemp[6];
	mMatrix[7] = lTemp[7];
	mMatrix[8] = lTemp[8];
}



TEMPLATE void QUAL::InvBMulInvA(const RotationMatrix& pMatrix)
{
	_TVarType lTemp[9];

	lTemp[0] = pMatrix.mMatrix[0] * mMatrix[0] +
		     pMatrix.mMatrix[3] * mMatrix[1] +
		     pMatrix.mMatrix[6] * mMatrix[2];
	lTemp[1] = pMatrix.mMatrix[0] * mMatrix[3] +
		     pMatrix.mMatrix[3] * mMatrix[4] +
		     pMatrix.mMatrix[6] * mMatrix[5];
	lTemp[2] = pMatrix.mMatrix[0] * mMatrix[6] +
		     pMatrix.mMatrix[3] * mMatrix[7] +
		     pMatrix.mMatrix[6] * mMatrix[8];

	lTemp[3] = pMatrix.mMatrix[1] * mMatrix[0] +
		     pMatrix.mMatrix[4] * mMatrix[1] +
		     pMatrix.mMatrix[7] * mMatrix[2];
	lTemp[4] = pMatrix.mMatrix[1] * mMatrix[3] +
		     pMatrix.mMatrix[4] * mMatrix[4] +
		     pMatrix.mMatrix[7] * mMatrix[5];
	lTemp[5] = pMatrix.mMatrix[1] * mMatrix[6] +
		     pMatrix.mMatrix[4] * mMatrix[7] +
		     pMatrix.mMatrix[7] * mMatrix[8];

	lTemp[6] = pMatrix.mMatrix[2] * mMatrix[0] +
		     pMatrix.mMatrix[5] * mMatrix[1] +
		     pMatrix.mMatrix[8] * mMatrix[2];
	lTemp[7] = pMatrix.mMatrix[2] * mMatrix[3] +
		     pMatrix.mMatrix[5] * mMatrix[4] +
		     pMatrix.mMatrix[8] * mMatrix[5];
	lTemp[8] = pMatrix.mMatrix[2] * mMatrix[6] +
		     pMatrix.mMatrix[5] * mMatrix[7] +
		     pMatrix.mMatrix[8] * mMatrix[8];

	mMatrix[0] = lTemp[0];
	mMatrix[1] = lTemp[1];
	mMatrix[2] = lTemp[2];
	mMatrix[3] = lTemp[3];
	mMatrix[4] = lTemp[4];
	mMatrix[5] = lTemp[5];
	mMatrix[6] = lTemp[6];
	mMatrix[7] = lTemp[7];
	mMatrix[8] = lTemp[8];
}



TEMPLATE void QUAL::InvBMulA(const RotationMatrix& pMatrix)
{
	_TVarType lTemp[9];

	lTemp[0] = pMatrix.mMatrix[0] * mMatrix[0] +
		     pMatrix.mMatrix[3] * mMatrix[3] +
		     pMatrix.mMatrix[6] * mMatrix[6];
	lTemp[1] = pMatrix.mMatrix[0] * mMatrix[1] +
		     pMatrix.mMatrix[3] * mMatrix[4] +
		     pMatrix.mMatrix[6] * mMatrix[7];
	lTemp[2] = pMatrix.mMatrix[0] * mMatrix[2] +
		     pMatrix.mMatrix[3] * mMatrix[5] +
		     pMatrix.mMatrix[6] * mMatrix[8];

	lTemp[3] = pMatrix.mMatrix[1] * mMatrix[0] +
		     pMatrix.mMatrix[4] * mMatrix[3] +
		     pMatrix.mMatrix[7] * mMatrix[6];
	lTemp[4] = pMatrix.mMatrix[1] * mMatrix[1] +
		     pMatrix.mMatrix[4] * mMatrix[4] +
		     pMatrix.mMatrix[7] * mMatrix[7];
	lTemp[5] = pMatrix.mMatrix[1] * mMatrix[2] +
		     pMatrix.mMatrix[4] * mMatrix[5] +
		     pMatrix.mMatrix[7] * mMatrix[8];

	lTemp[6] = pMatrix.mMatrix[2] * mMatrix[0] +
		     pMatrix.mMatrix[5] * mMatrix[3] +
		     pMatrix.mMatrix[8] * mMatrix[6];
	lTemp[7] = pMatrix.mMatrix[2] * mMatrix[1] +
		     pMatrix.mMatrix[5] * mMatrix[4] +
		     pMatrix.mMatrix[8] * mMatrix[7];
	lTemp[8] = pMatrix.mMatrix[2] * mMatrix[2] +
		     pMatrix.mMatrix[5] * mMatrix[5] +
		     pMatrix.mMatrix[8] * mMatrix[8];

	mMatrix[0] = lTemp[0];
	mMatrix[1] = lTemp[1];
	mMatrix[2] = lTemp[2];
	mMatrix[3] = lTemp[3];
	mMatrix[4] = lTemp[4];
	mMatrix[5] = lTemp[5];
	mMatrix[6] = lTemp[6];
	mMatrix[7] = lTemp[7];
	mMatrix[8] = lTemp[8];
}



TEMPLATE void QUAL::Add(const RotationMatrix& pMatrix)
{
	mMatrix[0] += pMatrix.mMatrix[0];
	mMatrix[1] += pMatrix.mMatrix[1];
	mMatrix[2] += pMatrix.mMatrix[2];

	mMatrix[3] += pMatrix.mMatrix[3];
	mMatrix[4] += pMatrix.mMatrix[4];
	mMatrix[5] += pMatrix.mMatrix[5];

	mMatrix[6] += pMatrix.mMatrix[6];
	mMatrix[7] += pMatrix.mMatrix[7];
	mMatrix[8] += pMatrix.mMatrix[8];
}

TEMPLATE void QUAL::Add(const RotationMatrix& pMatrix1, const RotationMatrix& pMatrix2)
{
	mMatrix[0] = pMatrix1.mMatrix[0] + pMatrix2.mMatrix[0];
	mMatrix[1] = pMatrix1.mMatrix[1] + pMatrix2.mMatrix[1];
	mMatrix[2] = pMatrix1.mMatrix[2] + pMatrix2.mMatrix[2];

	mMatrix[3] = pMatrix1.mMatrix[3] + pMatrix2.mMatrix[3];
	mMatrix[4] = pMatrix1.mMatrix[4] + pMatrix2.mMatrix[4];
	mMatrix[5] = pMatrix1.mMatrix[5] + pMatrix2.mMatrix[5];

	mMatrix[6] = pMatrix1.mMatrix[6] + pMatrix2.mMatrix[6];
	mMatrix[7] = pMatrix1.mMatrix[7] + pMatrix2.mMatrix[7];
	mMatrix[8] = pMatrix1.mMatrix[8] + pMatrix2.mMatrix[8];
}

TEMPLATE void QUAL::Add(_TVarType pScalar)
{
	mMatrix[0] += pScalar;
	mMatrix[1] += pScalar;
	mMatrix[2] += pScalar;

	mMatrix[3] += pScalar;
	mMatrix[4] += pScalar;
	mMatrix[5] += pScalar;

	mMatrix[6] += pScalar;
	mMatrix[7] += pScalar;
	mMatrix[8] += pScalar;
}

TEMPLATE void QUAL::Sub(const RotationMatrix& pMatrix)
{
	mMatrix[0] -= pMatrix.mMatrix[0];
	mMatrix[1] -= pMatrix.mMatrix[1];
	mMatrix[2] -= pMatrix.mMatrix[2];

	mMatrix[3] -= pMatrix.mMatrix[3];
	mMatrix[4] -= pMatrix.mMatrix[4];
	mMatrix[5] -= pMatrix.mMatrix[5];

	mMatrix[6] -= pMatrix.mMatrix[6];
	mMatrix[7] -= pMatrix.mMatrix[7];
	mMatrix[8] -= pMatrix.mMatrix[8];
}

TEMPLATE void QUAL::Sub(const RotationMatrix& pMatrix1, const RotationMatrix& pMatrix2)
{
	mMatrix[0] = pMatrix1.mMatrix[0] - pMatrix2.mMatrix[0];
	mMatrix[1] = pMatrix1.mMatrix[1] - pMatrix2.mMatrix[1];
	mMatrix[2] = pMatrix1.mMatrix[2] - pMatrix2.mMatrix[2];

	mMatrix[3] = pMatrix1.mMatrix[3] - pMatrix2.mMatrix[3];
	mMatrix[4] = pMatrix1.mMatrix[4] - pMatrix2.mMatrix[4];
	mMatrix[5] = pMatrix1.mMatrix[5] - pMatrix2.mMatrix[5];

	mMatrix[6] = pMatrix1.mMatrix[6] - pMatrix2.mMatrix[6];
	mMatrix[7] = pMatrix1.mMatrix[7] - pMatrix2.mMatrix[7];
	mMatrix[8] = pMatrix1.mMatrix[8] - pMatrix2.mMatrix[8];
}

TEMPLATE void QUAL::Sub(_TVarType pScalar)
{
	mMatrix[0] -= pScalar;
	mMatrix[1] -= pScalar;
	mMatrix[2] -= pScalar;

	mMatrix[3] -= pScalar;
	mMatrix[4] -= pScalar;
	mMatrix[5] -= pScalar;

	mMatrix[6] -= pScalar;
	mMatrix[7] -= pScalar;
	mMatrix[8] -= pScalar;
}

TEMPLATE void QUAL::Set(const RotationMatrix& pMatrix)
{
	mMatrix[0] = pMatrix.mMatrix[0];
	mMatrix[1] = pMatrix.mMatrix[1];
	mMatrix[2] = pMatrix.mMatrix[2];

	mMatrix[3] = pMatrix.mMatrix[3];
	mMatrix[4] = pMatrix.mMatrix[4];
	mMatrix[5] = pMatrix.mMatrix[5];

	mMatrix[6] = pMatrix.mMatrix[6];
	mMatrix[7] = pMatrix.mMatrix[7];
	mMatrix[8] = pMatrix.mMatrix[8];
}

TEMPLATE void QUAL::Set(_TVarType* pMatrix)
{
	mMatrix[0] = pMatrix[0];
	mMatrix[1] = pMatrix[1];
	mMatrix[2] = pMatrix[2];

	mMatrix[3] = pMatrix[3];
	mMatrix[4] = pMatrix[4];
	mMatrix[5] = pMatrix[5];

	mMatrix[6] = pMatrix[6];
	mMatrix[7] = pMatrix[7];
	mMatrix[8] = pMatrix[8];
}

TEMPLATE void QUAL::Set(_TVarType p11, _TVarType p12, _TVarType p13,
			_TVarType p21, _TVarType p22, _TVarType p23,
			_TVarType p31, _TVarType p32, _TVarType p33)
{
	mMatrix[0] = p11;
	mMatrix[1] = p12;
	mMatrix[2] = p13;

	mMatrix[3] = p21;
	mMatrix[4] = p22;
	mMatrix[5] = p23;

	mMatrix[6] = p31;
	mMatrix[7] = p32;
	mMatrix[8] = p33;
}

TEMPLATE int QUAL::GetRawDataSize()
{
	return sizeof(_TVarType) * 9;
}

TEMPLATE int QUAL::GetRawData(uint8* pData)
{
	_TVarType* lData = (_TVarType*)pData;

	lData[0] = mMatrix[0];
	lData[1] = mMatrix[1];
	lData[2] = mMatrix[2];
	lData[3] = mMatrix[3];
	lData[4] = mMatrix[4];
	lData[5] = mMatrix[5];
	lData[6] = mMatrix[6];
	lData[7] = mMatrix[7];
	lData[8] = mMatrix[8];

	return GetRawDataSize();
}

TEMPLATE int QUAL::SetRawData(uint8* pData)
{
	_TVarType* lData = (_TVarType*)pData;

	mMatrix[0] = lData[0];
	mMatrix[1] = lData[1];
	mMatrix[2] = lData[2];
	mMatrix[3] = lData[3];
	mMatrix[4] = lData[4];
	mMatrix[5] = lData[5];
	mMatrix[6] = lData[6];
	mMatrix[7] = lData[7];
	mMatrix[8] = lData[8];

	return GetRawDataSize();
}

TEMPLATE RotationMatrix<float> QUAL::ToFloat() const
{
	return RotationMatrix<float>((float)mMatrix[0], (float)mMatrix[1], (float)mMatrix[2],
				     (float)mMatrix[3], (float)mMatrix[4], (float)mMatrix[5],
				     (float)mMatrix[6], (float)mMatrix[7], (float)mMatrix[8]);
}

TEMPLATE RotationMatrix<double> QUAL::ToDouble() const
{
	return RotationMatrix<double>((double)mMatrix[0], (double)mMatrix[1], (double)mMatrix[2],
				      (double)mMatrix[3], (double)mMatrix[4], (double)mMatrix[5],
				      (double)mMatrix[6], (double)mMatrix[7], (double)mMatrix[8]);
}
