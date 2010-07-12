/*
	Class:  Quaternion
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#define TEMPLATE template<class _TVarType>
#define QUAL Quaternion<_TVarType>


TEMPLATE QUAL::Quaternion() :
	mA(1),
	mB(0),
	mC(0),
	mD(0)
{
}

TEMPLATE QUAL::Quaternion(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD) :
	mA(pA),
	mB(pB),
	mC(pC),
	mD(pD)
{
}

TEMPLATE QUAL::Quaternion(const Quaternion& pQuaternion) :
	mA(pQuaternion.mA),
	mB(pQuaternion.mB),
	mC(pQuaternion.mC),
	mD(pQuaternion.mD)
{
}

TEMPLATE QUAL::Quaternion(const RotationMatrix<_TVarType>& pRotMtx)
{
	Set(pRotMtx);
}

TEMPLATE QUAL::Quaternion(_TVarType pAngle, const Vector3D<_TVarType>& pVector)
{
	Set(pAngle, pVector);
}

TEMPLATE QUAL::~Quaternion()
{
}

TEMPLATE void QUAL::Set(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	mA = pA;
	mB = pB;
	mC = pC;
	mD = pD;
}

TEMPLATE void QUAL::Set(const Quaternion& pQuaternion)
{
	mA = pQuaternion.mA;
	mB = pQuaternion.mB;
	mC = pQuaternion.mC;
	mD = pQuaternion.mD;
}

TEMPLATE void QUAL::SetConjugate(const Quaternion& pQuaternion)
{
	mA =  pQuaternion.mA;
	mB = -pQuaternion.mB;
	mC = -pQuaternion.mC;
	mD = -pQuaternion.mD;
}

TEMPLATE void QUAL::SetIdentity()
{
	mA = 1;
	mB = 0;
	mC = 0;
	mD = 0;
}

TEMPLATE void QUAL::Set(_TVarType pAngle, const Vector3D<_TVarType>& pVector)
{
	Vector3D<_TVarType> lV(pVector);
	lV.Normalize();

	_TVarType lSinA = (_TVarType)sin(pAngle / (_TVarType)2.0);

	mA = (_TVarType)cos(pAngle / (_TVarType)2.0);
	mB = lSinA * lV.x;
	mC = lSinA * lV.y;
	mD = lSinA * lV.z;

	Normalize();
}

TEMPLATE void QUAL::Set(const RotationMatrix<_TVarType>& pRotMtx)
{
	// This is a fool proof way of converting a matrix into a quaternion.
	// No bugs, no glitches, no unexpected behaviour...
	Vector3D<_TVarType> lV;
	_TVarType lAngle;
	pRotMtx.GetRotationVector(lV, lAngle);
	Set(lAngle, lV);
/*
	//
	// The following method is derived from "Visualizing Quaternions" by 
	// Andrew J. Hanson, and (naturally) it doesn't quite work. Why should it?
	//

	// Check the diagonal.
	_TVarType lTrace = pRotMtx.GetTrace();

	const _TVarType* lMatrix = (const _TVarType*)pRotMtx;

	if (lTrace > 0)
	{
		_TVarType s = (_TVarType)sqrt(lTrace + 1);

		mA = s / (_TVarType)2;
		s = (_TVarType)0.5 / s;

		mB = (lMatrix[5] - lMatrix[7]) * s;
		mC = (lMatrix[6] - lMatrix[2]) * s;
		mD = (lMatrix[1] - lMatrix[3]) * s;
	}
	else // Diagonal is negative.
	{
		int i, j, k;

		i = 0;
		
		if (lMatrix[4] > lMatrix[0])     
			i = 1;

		if (lMatrix[8] > lMatrix[i * 3 + i]) 
			i = 2;

		int lNext[3] = {1, 2, 0};

		j = lNext[i];
		k = lNext[j];

		_TVarType s = (_TVarType)sqrt(lMatrix[i * 3 + i] - (lMatrix[j * 3 + j] + lMatrix[k * 3 + k]) + 1);

		_TVarType lQ[4];
		lQ[i] = s * (_TVarType)0.5;

		if (s != 0)
			s = (_TVarType)0.5 / s;

		lQ[3] = (lMatrix[j * 3 + k] - lMatrix[k * 3 + j]) * s;
		lQ[j] = (lMatrix[i * 3 + j] + lMatrix[j * 3 + i]) * s;
		lQ[k] = (lMatrix[i * 3 + k] - lMatrix[k * 3 + i]) * s;

		mA = lQ[0];
		mB = lQ[1];
		mC = lQ[2];
		mD = lQ[3];
	}
*/
}

TEMPLATE _TVarType QUAL::GetA() const
{
	return mA;
}

TEMPLATE _TVarType QUAL::GetB() const
{
	return mB;
}

TEMPLATE _TVarType QUAL::GetC() const
{
	return mC;
}

TEMPLATE _TVarType QUAL::GetD() const
{
	return mD;
}

TEMPLATE RotationMatrix<_TVarType> QUAL::GetAsRotationMatrix() const
{
	_TVarType lAB, lAC, lAD;
	_TVarType lBB, lCC, lDD;
	_TVarType lBC, lBD, lCD;
	_TVarType lB2, lC2, lD2;

	// Calculate coefficients.
	lB2 = mB + mB;
	lC2 = mC + mC;
	lD2 = mD + mD;

	lBB = mB * lB2;
	lBC = mB * lC2;
	lBD = mB * lD2;

	lCC = mC * lC2;
	lCD = mC * lD2;
	lDD = mD * lD2;

	lAB = mA * lB2;
	lAC = mA * lC2;
	lAD = mA * lD2;

	// And now create the matrix...
	RotationMatrix<_TVarType> lRotMtx
	(
		(_TVarType)1.0 - (lCC + lDD),
		lBC - lAD,
		lBD + lAC,

		lBC + lAD,
		(_TVarType)1.0 - (lBB + lDD),
		lCD - lAB,

		lBD - lAC,
		lCD + lAB,
		(_TVarType)1.0 - (lBB + lCC)
	);
/*	RotationMatrix<_TVarType> lRotMtx
	(
		(_TVarType)1.0 - (lCC + lDD), 
		lBC + lAD,
		lBD - lAC,

		lBC - lAD,
		(_TVarType)1.0 - (lBB + lDD), 
		lCD + lAB,

		lBD + lAC,
		lCD - lAB,
		(_TVarType)1.0 - (lBB + lCC)
	);
*/
	return lRotMtx;
}

TEMPLATE void QUAL::GetAsRotationMatrix(RotationMatrix<_TVarType>& pRotMtx) const
{
	_TVarType lAB, lAC, lAD;
	_TVarType lBB, lCC, lDD;
	_TVarType lBC, lBD, lCD;
	_TVarType lB2, lC2, lD2;

	// Calculate coefficients.
	lB2 = mB + mB;
	lC2 = mC + mC;
	lD2 = mD + mD;

	lBB = mB * lB2;
	lBC = mB * lC2;
	lBD = mB * lD2;

	lCC = mC * lC2;
	lCD = mC * lD2;
	lDD = mD * lD2;

	lAB = mA * lB2;
	lAC = mA * lC2;
	lAD = mA * lD2;

	// And now create the matrix...
	pRotMtx.Set((_TVarType)1.0 - (lCC + lDD),
		     lBC - lAD,
		     lBD + lAC,

		     lBC + lAD,
		     (_TVarType)1.0 - (lBB + lDD),
		     lCD - lAB,

		     lBD - lAC,
		     lCD + lAB,
		     (_TVarType)1.0 - (lBB + lCC)
		    );
/*	pRotMtx.Set
		(
			(_TVarType)1.0 - (lCC + lDD), 
			lBC + lAD,
			lBD - lAC,

			lBC - lAD,
			(_TVarType)1.0 - (lBB + lDD),
			lCD + lAB,

			lBD + lAC,
			lCD - lAB,
			(_TVarType)1.0 - (lBB + lCC)
		);
*/
}

TEMPLATE void QUAL::Add(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	mA += pA;
	mB += pB;
	mC += pC;
	mD += pD;
}

TEMPLATE void QUAL::Add(const Quaternion& pQuaternion)
{
	mA += pQuaternion.mA;
	mB += pQuaternion.mB;
	mC += pQuaternion.mC;
	mD += pQuaternion.mD;
}

TEMPLATE void QUAL::Sub(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	mA -= pA;
	mB -= pB;
	mC -= pC;
	mD -= pD;
}

TEMPLATE void QUAL::Sub(const Quaternion& pQuaternion)
{
	mA -= pQuaternion.mA;
	mB -= pQuaternion.mB;
	mC -= pQuaternion.mC;
	mD -= pQuaternion.mD;
}

TEMPLATE void QUAL::Mul(_TVarType pX)
{
	mA *= pX;
	mB *= pX;
	mC *= pX;
	mD *= pX;
}

TEMPLATE void QUAL::Div(_TVarType pX)
{
	mA /= pX;
	mB /= pX;
	mC /= pX;
	mD /= pX;
}

TEMPLATE void QUAL::Mul(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	_TVarType lA = mA * pA - mB * pB - mC * pC - mD * pD;
	_TVarType lB = mA * pB + mB * pA + mC * pD - mD * pC;
	_TVarType lC = mA * pC + mC * pA + mD * pB - mB * pD;
	_TVarType lD = mA * pD + mD * pA + mB * pC - mC * pB;

	mA = lA;
	mB = lB;
	mC = lC;
	mD = lD;
}

TEMPLATE void QUAL::Mul(const Quaternion& pQuaternion)
{
	Mul(pQuaternion.mA, pQuaternion.mB, pQuaternion.mC, pQuaternion.mD);
}

TEMPLATE void QUAL::AMulInvB(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	_TVarType lA =  mA * pA + mB * pB + mC * pC + mD * pD;
	_TVarType lB = -mA * pB + mB * pA - mC * pD + mD * pC;
	_TVarType lC = -mA * pC + mC * pA - mD * pB + mB * pD;
	_TVarType lD = -mA * pD + mD * pA - mB * pC + mC * pB;

	_TVarType lLRecip = (_TVarType)1.0 / (pA * pA + pB * pB + pC * pC + pD * pD);

	mA = lA * lLRecip;
	mB = lB * lLRecip;
	mC = lC * lLRecip;
	mD = lD * lLRecip;
}

TEMPLATE void QUAL::AMulInvB(const Quaternion& pQuaternion)
{
	AMulInvB(pQuaternion.mA, pQuaternion.mB, pQuaternion.mC, pQuaternion.mD);
}

TEMPLATE void QUAL::InvAMulB(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	_TVarType lA = mA * pA + mB * pB + mC * pC + mD * pD;
	_TVarType lB = mA * pB - mB * pA - mC * pD + mD * pC;
	_TVarType lC = mA * pC - mC * pA - mD * pB + mB * pD;
	_TVarType lD = mA * pD - mD * pA - mB * pC + mC * pB;

	_TVarType lLRecip = (_TVarType)1.0 / GetNorm();

	mA = lA * lLRecip;
	mB = lB * lLRecip;
	mC = lC * lLRecip;
	mD = lD * lLRecip;
}

TEMPLATE void QUAL::InvAMulB(const Quaternion& pQuaternion)
{
	InvAMulB(pQuaternion.mA, pQuaternion.mB, pQuaternion.mC, pQuaternion.mD);
}

TEMPLATE void QUAL::InvAMulInvB(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	_TVarType lA =  mA * pA - mB * pB - mC * pC - mD * pD;
	_TVarType lB = -mA * pB - mB * pA + mC * pD - mD * pC;
	_TVarType lC = -mA * pC - mC * pA + mD * pB - mB * pD;
	_TVarType lD = -mA * pD - mD * pA + mB * pC - mC * pB;

	_TVarType lLRecip = (_TVarType)1.0 / 
		(
			(mA * mA + mB * mB + mC * mC + mD * mD) *
			(pA * pA + pB * pB + pC * pC + pD * pD)
		);

	mA = lA * lLRecip;
	mB = lB * lLRecip;
	mC = lC * lLRecip;
	mD = lD * lLRecip;
}

TEMPLATE void QUAL::InvAMulInvB(const Quaternion& pQuaternion)
{
	InvAMulInvB(pQuaternion.mA, pQuaternion.mB, pQuaternion.mC, pQuaternion.mD);
}


TEMPLATE void QUAL::BMulA(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	_TVarType lA = pA * mA - pB * mB - pC * mC - pD * mD;
	_TVarType lB = pA * mB + pB * mA + pC * mD - pD * mC;
	_TVarType lC = pA * mC + pC * mA + pD * mB - pB * mD;
	_TVarType lD = pA * mD + pD * mA + pB * mC - pC * mB;

	mA = lA;
	mB = lB;
	mC = lC;
	mD = lD;
}

TEMPLATE void QUAL::BMulA(const Quaternion& pQuaternion)
{
	BMulA(pQuaternion.mA, pQuaternion.mB, pQuaternion.mC, pQuaternion.mD);
}


TEMPLATE void QUAL::InvBMulA(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	_TVarType lA = pA * mA + pB * mB + pC * mC + pD * mD;
	_TVarType lB = pA * mB - pB * mA - pC * mD + pD * mC;
	_TVarType lC = pA * mC - pC * mA - pD * mB + pB * mD;
	_TVarType lD = pA * mD - pD * mA - pB * mC + pC * mB;

	_TVarType lLRecip = (_TVarType)1.0 / (pA * pA + pB * pB + pC * pC + pD * pD);

	mA = lA * lLRecip;
	mB = lB * lLRecip;
	mC = lC * lLRecip;
	mD = lD * lLRecip;
}

TEMPLATE void QUAL::InvBMulA(const Quaternion& pQuaternion)
{
	InvBMulA(pQuaternion.mA, pQuaternion.mB, pQuaternion.mC, pQuaternion.mD);
}

TEMPLATE void QUAL::BMulInvA(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	_TVarType lA =  pA * mA + pB * mB + pC * mC + pD * mD;
	_TVarType lB = -pA * mB + pB * mA - pC * mD + pD * mC;
	_TVarType lC = -pA * mC + pC * mA - pD * mB + pB * mD;
	_TVarType lD = -pA * mD + pD * mA - pB * mC + pC * mB;

	_TVarType lLRecip = (_TVarType)1.0 / GetNorm();

	mA = lA * lLRecip;
	mB = lB * lLRecip;
	mC = lC * lLRecip;
	mD = lD * lLRecip;
}

TEMPLATE void QUAL::BMulInvA(const Quaternion& pQuaternion)
{
	BMulInvA(pQuaternion.mA, pQuaternion.mB, pQuaternion.mC, pQuaternion.mD);
}

TEMPLATE void QUAL::InvBMulInvA(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	_TVarType lA =  pA * mA - pB * mB - pC * mC - pD * mD;
	_TVarType lB = -pA * mB - pB * mA + pC * mD - pD * mC;
	_TVarType lC = -pA * mC - pC * mA + pD * mB - pB * mD;
	_TVarType lD = -pA * mD - pD * mA + pB * mC - pC * mB;

	_TVarType lLRecip = (_TVarType)1.0 / 
		(
			(mA * mA + mB * mB + mC * mC + mD * mD) *
			(pA * pA + pB * pB + pC * pC + pD * pD)
		);

	mA = lA * lLRecip;
	mB = lB * lLRecip;
	mC = lC * lLRecip;
	mD = lD * lLRecip;
}

TEMPLATE void QUAL::InvBMulInvA(const Quaternion& pQuaternion)
{
	InvBMulInvA(pQuaternion.mA, pQuaternion.mB, pQuaternion.mC, pQuaternion.mD);
}



TEMPLATE void QUAL::Div(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	// Division is defined as qp', where q = this, and p' is the inverse
	// of the quaternion (pA, pB, pC, pD).
	// Note that we can redefine division as p'q as well, but with other
	// results.
	AMulInvB(pA, pB, pC, pD);

	// Division is defined as q'p, where q' = inverse of this, and p is the
	// quaternion (pA, pB, pC, pD).
	//InvAMulB(pA, pB, pC, pD);
}

TEMPLATE void QUAL::Div(const Quaternion& pQuaternion)
{
	Div(pQuaternion.mA, pQuaternion.mB, pQuaternion.mC, pQuaternion.mD);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetRotatedVector(const Vector3D<_TVarType>& pVector) const
{
	Quaternion lQ(0, pVector.x, pVector.y, pVector.z);
	lQ = (*this) * lQ * GetInverse();	// TODO: assume unit, and use conjugate instead of inverse.

	Vector3D<_TVarType> lV(lQ.mB, lQ.mC, lQ.mD);
	return lV;
}

TEMPLATE Vector3D<_TVarType> QUAL::GetInverseRotatedVector(const Vector3D<_TVarType>& pVector) const
{
	Quaternion lQ(0, pVector.x, pVector.y, pVector.z);
	lQ = GetInverse() * lQ * (*this);	// TODO: assume unit, and use conjugate instead of inverse.
	return Vector3D<_TVarType>(lQ.mB, lQ.mC, lQ.mD);
}

TEMPLATE void QUAL::FastInverseRotatedVector(Vector3D<_TVarType>& pTarget, const Vector3D<_TVarType>& pSource) const
{
	Quaternion<_TVarType> lQ(mA, -mB, -mC, -mD);	// Conjugate.
	lQ.Mul(0, pSource.x, pSource.y, pSource.z);
	lQ.Mul(mA, mB, mC, mD);
	pTarget.x = lQ.mB;
	pTarget.y = lQ.mC;
	pTarget.z = lQ.mD;
}

TEMPLATE void QUAL::MakeConjugate()
{
	mB = -mB;
	mC = -mC;
	mD = -mD;
}

TEMPLATE Quaternion<_TVarType> QUAL::GetConjugate() const
{
	Quaternion lTemp(*this);
	lTemp.MakeConjugate();
	return lTemp;
}

TEMPLATE void QUAL::MakeInverse()
{
	_TVarType lLengthSquared = GetNorm();
	MakeConjugate();
	Div(lLengthSquared);
}

TEMPLATE Quaternion<_TVarType> QUAL::GetInverse() const
{
	Quaternion lTemp(*this);
	lTemp.MakeInverse();
	return lTemp;
}

TEMPLATE void QUAL::MakeSquare()
{
	// Return q*q, where q = this.
	_TVarType l2xA = mA * 2.0f;
	mA  = mA * mA - (mB * mB + mC * mC + mD * mD);
	mB *= l2xA;
	mC *= l2xA;
	mD *= l2xA;
	return *this;
}

TEMPLATE Quaternion<_TVarType> QUAL::GetSquare() const
{
	Quaternion lTemp(*this);
	lTemp.Square();
	return lTemp;
}

TEMPLATE _TVarType QUAL::GetNorm() const
{
	return mA * mA + mB * mB + mC * mC + mD * mD;
}

TEMPLATE _TVarType QUAL::GetMagnitude() const
{
	return sqrt(GetNorm());
}

TEMPLATE void QUAL::Negate()
{
	mA = -mA;
	mB = -mB;
	mC = -mC;
	mD = -mD;
}

TEMPLATE void QUAL::Normalize(_TVarType pLength)
{
	_TVarType lLength = GetMagnitude();

	if (lLength >= MathTraits<_TVarType>::FullEps())
	{
		Mul(pLength/lLength);
	}
	else
	{
		Set(0, 0, 0, 0);
	}
}

TEMPLATE void QUAL::Slerp(const Quaternion<_TVarType>& pFrom,
			  const Quaternion<_TVarType>& pTo,
			  _TVarType pT)
{
	// Calculate cosine (the dot product between two quaternions).
	_TVarType lCosOmega = pFrom.mA * pTo.mA +
				pFrom.mB * pTo.mB +
				pFrom.mC * pTo.mC +
				pFrom.mD * pTo.mD;

	_TVarType lTo[4];

	// Adjust signs if necessary.
	if (lCosOmega < 0)
	{
		lCosOmega = -lCosOmega;
		lTo[0] = -pTo.mA;
		lTo[1] = -pTo.mB;
		lTo[2] = -pTo.mC;
		lTo[3] = -pTo.mD;
	}
	else
	{
		lTo[0] = pTo.mA;
		lTo[1] = pTo.mB;
		lTo[2] = pTo.mC;
		lTo[3] = pTo.mD;
	}

	_TVarType lScale0;
	_TVarType lScale1;

	if (lCosOmega < ((_TVarType)1.0 - MathTraits<_TVarType>::FullEps()))
	{
		// Standard case (slerp).
		_TVarType lOmega = acos(lCosOmega);
		_TVarType lSinOmega = (_TVarType)1.0 / sin(lOmega);
		lScale0 = sin(((_TVarType)1.0 - pT) * lOmega) * lSinOmega;
		lScale1 = sin(pT * lOmega) * lSinOmega;
	}
	else
	{
		// The quaternions are very close to each other, and we
		// can do a linear interpolation instead to avoid division
		// by zero.
		lScale0 = (_TVarType)1.0 - pT;
		lScale1 = pT;
	}

	// And finally, perform the actual interpolation.
	mA = pFrom.mA * lScale0 + lTo[0] * lScale1;
	mB = pFrom.mB * lScale0 + lTo[1] * lScale1;
	mC = pFrom.mC * lScale0 + lTo[2] * lScale1;
	mD = pFrom.mD * lScale0 + lTo[3] * lScale1;
}

TEMPLATE void QUAL::RotateAroundWorldX(_TVarType pAngle)
{
	_TVarType lHalfAngle = pAngle / (_TVarType)2.0;
	Quaternion lQ((_TVarType)cos(lHalfAngle), (_TVarType)sin(lHalfAngle), 0, 0);
	BMulA(lQ);
}

TEMPLATE void QUAL::RotateAroundWorldY(_TVarType pAngle)
{
	_TVarType lHalfAngle = pAngle / (_TVarType)2.0;
	Quaternion lQ((_TVarType)cos(lHalfAngle), 0, (_TVarType)sin(lHalfAngle), 0);
	BMulA(lQ);
}

TEMPLATE void QUAL::RotateAroundWorldZ(_TVarType pAngle)
{
	_TVarType lHalfAngle = pAngle / (_TVarType)2.0;
	Quaternion lQ((_TVarType)cos(lHalfAngle), 0, 0, (_TVarType)sin(lHalfAngle));
	BMulA(lQ);
}

TEMPLATE void QUAL::RotateAroundOwnX(_TVarType pAngle)
{
	_TVarType lHalfAngle = pAngle / (_TVarType)2.0;
	Quaternion lQ((_TVarType)cos(lHalfAngle), (_TVarType)sin(lHalfAngle), 0, 0);
	Mul(lQ);
}

TEMPLATE void QUAL::RotateAroundOwnY(_TVarType pAngle)
{
	_TVarType lHalfAngle = pAngle / (_TVarType)2.0;
	Quaternion lQ((_TVarType)cos(lHalfAngle), 0, (_TVarType)sin(lHalfAngle), 0);
	Mul(lQ);
}

TEMPLATE void QUAL::RotateAroundOwnZ(_TVarType pAngle)
{
	_TVarType lHalfAngle = pAngle / (_TVarType)2.0;
	Quaternion lQ((_TVarType)cos(lHalfAngle), 0, 0, (_TVarType)sin(lHalfAngle));
	Mul(lQ);
}

TEMPLATE void QUAL::RotateAroundVector(const Vector3D<_TVarType>& pVector, _TVarType pAngle)
{
	Quaternion lQ(pAngle, pVector);
	BMulA(lQ);
}

TEMPLATE void QUAL::GetRotationVector(Vector3D<_TVarType>& pVector) const
{
	pVector.Set(mB, mC, mD);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetRotationVector() const
{
	return Vector3D<_TVarType>(mB, mC, mD);
}

TEMPLATE _TVarType QUAL::GetRotationAngle() const
{
	_TVarType lAngle;
	if (mA <= -1)
	{
		lAngle = PI;
	}
	else if (mA >= 1)
	{
		lAngle = 0;
	}
	else
	{
		lAngle = ::acos(mA);
	}
	lAngle *= (_TVarType)2.0;
	return (lAngle);
}

TEMPLATE void QUAL::GetEulerAngles(Vector3D<_TVarType>& pAngles) const
{
	GetEulerAngles(pAngles.x, pAngles.y, pAngles.z);
}

TEMPLATE void QUAL::GetEulerAngles(_TVarType& pYaw, _TVarType& pPitch, _TVarType& pRoll) const
{
	const _TVarType lW2 = GetA()*GetA();
	const _TVarType lX2 = GetB()*GetB();
	const _TVarType lY2 = GetC()*GetC();
	const _TVarType lZ2 = GetD()*GetD();
	const _TVarType lUnitLength = lW2 + lX2 + lY2 + lZ2;	// Normalised == 1, otherwise correction divisor.
	const _TVarType abcd = GetA()*GetB() + GetC()*GetD();
	_TVarType lYaw;
	_TVarType lPitch;
	_TVarType lRoll;
	if (abcd > ((_TVarType)0.5-MathTraits<_TVarType>::FullEps())*lUnitLength)
	{
		lYaw = 2 * atan2(GetC(), GetA());
		lPitch = (_TVarType)PI/2;
		lRoll = 0;
	}
	else if (abcd < (-(_TVarType)0.5+MathTraits<_TVarType>::FullEps())*lUnitLength)
	{
		lYaw = -2 * ::atan2(GetC(), GetA());
		lPitch = -(_TVarType)PI/2;
		lRoll = 0;
	}
	else
	{
		const _TVarType adbc = GetA()*GetD() - GetB()*GetC();
		const _TVarType acbd = GetA()*GetC() - GetB()*GetD();
		lYaw = ::atan2(2*adbc, 1 - 2*(lZ2+lX2));
		lPitch = ::asin(2*abcd/lUnitLength);
		lRoll = ::atan2(2*acbd, 1 - 2*(lY2+lX2));
	}

	pYaw = lYaw;
	pPitch = lPitch;
	pRoll = lRoll;
}

TEMPLATE void QUAL::SetEulerAngles(const Vector3D<_TVarType>& pAngles)
{
	SetEulerAngles(pAngles.x, pAngles.y, pAngles.z);
}

TEMPLATE void QUAL::SetEulerAngles(_TVarType pYaw, _TVarType pPitch, _TVarType pRoll)
{
	SetIdentity();
	RotateAroundOwnZ(pYaw);
	RotateAroundOwnX(pPitch);
	RotateAroundOwnY(pRoll);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetAxisX() const
{
	_TVarType lAC, lAD;
	_TVarType lCC, lDD;
	_TVarType lBC, lBD;
	_TVarType lC2, lD2;

	// Calculate coefficients.
	lC2 = mC + mC;
	lD2 = mD + mD;

	lBC = mB * lC2;
	lBD = mB * lD2;

	lCC = mC * lC2;
	lDD = mD * lD2;

	lAC = mA * lC2;
	lAD = mA * lD2;

/*	Vector3D<_TVarType> lV
	(
		(_TVarType)1.0 - (lCC + lDD),
		lBC - lAD,
		lBD + lAC
	);
*/
	Vector3D<_TVarType> lV
	(
		(_TVarType)1.0 - (lCC + lDD),
		lBC + lAD,
		lBD - lAC
	);

	return lV;
}

TEMPLATE Vector3D<_TVarType> QUAL::GetAxisY() const
{
	_TVarType lAB, lAD;
	_TVarType lBB, lDD;
	_TVarType lBC, lCD;
	_TVarType lB2, lC2, lD2;

	// Calculate coefficients.
	lB2 = mB + mB;
	lC2 = mC + mC;
	lD2 = mD + mD;

	lBB = mB * lB2;
	lDD = mD * lD2;

	lBC = mB * lC2;
	lCD = mC * lD2;

	lAB = mA * lB2;
	lAD = mA * lD2;

/*	Vector3D<_TVarType> lV
	(
		lBC + lAD,
		(_TVarType)1.0 - (lBB + lDD),
		lCD - lAB
	);
*/
	Vector3D<_TVarType> lV
	(
		lBC - lAD,
		(_TVarType)1.0 - (lBB + lDD),
		lCD + lAB
	);

	return lV;
}

TEMPLATE Vector3D<_TVarType> QUAL::GetAxisZ() const
{
	_TVarType lAB, lAC;
	_TVarType lBB, lCC;
	_TVarType lBD, lCD;
	_TVarType lB2, lC2, lD2;

	// Calculate coefficients.
	lB2 = mB + mB;
	lC2 = mC + mC;
	lD2 = mD + mD;

	lBB = mB * lB2;
	lCC = mC * lC2;

	lBD = mB * lD2;
	lCD = mC * lD2;

	lAB = mA * lB2;
	lAC = mA * lC2;

/*	Vector3D<_TVarType> lV
	(
		lBD - lAC,
		lCD + lAB,
		(_TVarType)1.0 - (lBB + lCC)
	);
*/
	Vector3D<_TVarType> lV
	(
		lBD + lAC,
		lCD - lAB,
		(_TVarType)1.0 - (lBB + lCC)
	);

	return lV;
}

TEMPLATE Vector3D<_TVarType> QUAL::GetInverseAxisX() const
{
	return GetInverse().GetAxisX();
}

TEMPLATE Vector3D<_TVarType> QUAL::GetInverseAxisY() const
{
	return GetInverse().GetAxisY();
}

TEMPLATE Vector3D<_TVarType> QUAL::GetInverseAxisZ() const
{
	return GetInverse().GetAxisZ();
}

TEMPLATE Quaternion<_TVarType> QUAL::operator= (const Quaternion<_TVarType>& pQ)
{
	Set(pQ);
	return *this;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator= (const RotationMatrix<_TVarType>& pRotMtx)
{
	Set(pRotMtx);
	return *this;
}

TEMPLATE bool QUAL::operator== (const Quaternion<_TVarType>& pQ) const
{
	return mA == pQ.mA &&
	       mB == pQ.mB &&
	       mC == pQ.mC &&
	       mD == pQ.mD;
}

TEMPLATE bool QUAL::operator!= (const Quaternion<_TVarType>& pQ) const
{
	return mA != pQ.mA ||
	       mB != pQ.mB ||
	       mC != pQ.mC ||
	       mD != pQ.mD;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator+= (const Quaternion<_TVarType>& pQ)
{
	Add(pQ);
	return *this;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator+ (const Quaternion<_TVarType>& pQ) const
{
	Quaternion lTemp(*this);
	lTemp.Add(pQ);
	return lTemp;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator-= (const Quaternion<_TVarType>& pQ)
{
	Sub(pQ);
	return *this;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator- (const Quaternion<_TVarType>& pQ) const
{
	Quaternion lTemp(*this);
	lTemp.Sub(pQ);
	return lTemp;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator- () const
{
	Quaternion lTemp(*this);
	lTemp.Negate();
	return lTemp;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator* (const Quaternion<_TVarType>& pQ) const
{
	Quaternion lTemp(*this);
	lTemp.Mul(pQ);
	return lTemp;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator*= (const Quaternion<_TVarType>& pQ)
{
	Mul(pQ);
	return *this;
}

TEMPLATE Vector3D<_TVarType> QUAL::operator * (const Vector3D<_TVarType>& pV) const
{
	return GetRotatedVector(pV);
}

#if !defined(LEPRA_MSVC)

TEMPLATE Vector3D<_TVarType> operator * (const Vector3D<_TVarType>& pVector, const Quaternion<_TVarType>& pQ)
{
	return pQ.GetInverseRotatedVector(pVector);
}

TEMPLATE Vector3D<_TVarType>& operator *= (Vector3D<_TVarType>& pVector, const Quaternion<_TVarType>& pQ)
{
	pVector = pQ*pVector;
	return (pVector);
}

#else // LEPRA_MSVC

// Visual Studio .NET can't handle function templates... (.NET 2003 can? .NET 2005 can!).
// Need to implement the functions separately like this.

inline Vector3D<float32> operator * (const Vector3D<float32>& pVector, const Quaternion<float32>& pQ)
{
	return pQ.GetInverseRotatedVector(pVector);
}

inline Vector3D<float32>& operator *= (Vector3D<float32>& pVector, const Quaternion<float32>& pQ)
{
	pVector = pQ*pVector;
	return (pVector);
}

inline Vector3D<float64> operator * (const Vector3D<float64>& pVector, const Quaternion<float64>& pQ)
{
	return pQ.GetInverseRotatedVector(pVector);
}

inline Vector3D<float64>& operator *= (Vector3D<float64>& pVector, const Quaternion<float64>& pQ)
{
	pVector = pQ*pVector;
	return (pVector);
}

#endif // !LEPRA_MSVC/LEPRA_MSVC


TEMPLATE Quaternion<_TVarType> QUAL::operator* (_TVarType pScalar) const
{
	Quaternion lTemp(*this);
	lTemp *= pScalar;
	return lTemp;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator*= (_TVarType pScalar)
{
	mA *= pScalar;
	mB *= pScalar;
	mC *= pScalar;
	mD *= pScalar;
	return *this;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator/ (const Quaternion<_TVarType>& pQ) const
{
	Quaternion lTemp(*this);
	lTemp.Div(pQ);
	return lTemp;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator/= (const Quaternion<_TVarType>& pQ)
{
	Div(pQ);
	return *this;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator/ (_TVarType pScalar) const
{
	Quaternion lTemp(*this);
	lTemp /= pScalar;
	return lTemp;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator/= (_TVarType pScalar)
{
	pScalar = 1 / pScalar;
	mA *= pScalar;
	mB *= pScalar;
	mC *= pScalar;
	mD *= pScalar;
	return *this;
}

TEMPLATE Quaternion<_TVarType> QUAL::operator* () const
{
	Quaternion lTemp(*this);
	lTemp.MakeConjugate();
	return lTemp;
}

TEMPLATE Quaternion<float> QUAL::ToFloat() const
{
	return Quaternion<float>((float)mA, (float)mB, (float)mC, (float)mD);
}

TEMPLATE Quaternion<double> QUAL::ToDouble() const
{
	return Quaternion<double>((double)mA, (double)mB, (double)mC, (double)mD);
}

#undef TEMPLATE
#undef QUAL
