/*
	Class:  Quaternion
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#define TEMPLATE template<class _TVarType>
#define QUAL Quaternion<_TVarType>


TEMPLATE QUAL::Quaternion() :
	a(1),
	b(0),
	c(0),
	d(0)
{
}

TEMPLATE QUAL::Quaternion(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	mData[0] = pA;
	mData[1] = pB;
	mData[2] = pC;
	mData[3] = pD;
}

TEMPLATE QUAL::Quaternion(const _TVarType pData[4])
{
	mData[0] = pData[0];
	mData[1] = pData[1];
	mData[2] = pData[2];
	mData[3] = pData[3];
}

TEMPLATE QUAL::Quaternion(const Quaternion& pQuaternion)
{
	mData[0] = pQuaternion.mData[0];
	mData[1] = pQuaternion.mData[1];
	mData[2] = pQuaternion.mData[2];
	mData[3] = pQuaternion.mData[3];
}

TEMPLATE QUAL::Quaternion(const RotationMatrix<_TVarType>& pRotMtx)
{
	Set(pRotMtx);
}

TEMPLATE QUAL::Quaternion(_TVarType pAngle, const Vector3D<_TVarType>& pVector)
{
	Set(pAngle, pVector);
}

TEMPLATE void QUAL::Set(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	mData[0] = pA;
	mData[1] = pB;
	mData[2] = pC;
	mData[3] = pD;
}

TEMPLATE void QUAL::Set(const _TVarType pData[4])
{
	mData[0] = pData[0];
	mData[1] = pData[1];
	mData[2] = pData[2];
	mData[3] = pData[3];
}

TEMPLATE void QUAL::Set(const Quaternion& pQuaternion)
{
	mData[0] = pQuaternion.mData[0];
	mData[1] = pQuaternion.mData[1];
	mData[2] = pQuaternion.mData[2];
	mData[3] = pQuaternion.mData[3];
}

TEMPLATE void QUAL::SetConjugate(const Quaternion& pQuaternion)
{
	a =  pQuaternion.a;
	b = -pQuaternion.b;
	c = -pQuaternion.c;
	d = -pQuaternion.d;
}

TEMPLATE void QUAL::SetIdentity()
{
	a = 1;
	b = 0;
	c = 0;
	d = 0;
}

TEMPLATE void QUAL::Set(_TVarType pAngle, const Vector3D<_TVarType>& pVector)
{
	Vector3D<_TVarType> lV(pVector);
	lV.Normalize();

	_TVarType lSinA = (_TVarType)sin(pAngle / (_TVarType)2.0);

	a = (_TVarType)cos(pAngle / (_TVarType)2.0);
	b = lSinA * lV.x;
	c = lSinA * lV.y;
	d = lSinA * lV.z;

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

		a = s / (_TVarType)2;
		s = (_TVarType)0.5 / s;

		b = (lMatrix[5] - lMatrix[7]) * s;
		c = (lMatrix[6] - lMatrix[2]) * s;
		d = (lMatrix[1] - lMatrix[3]) * s;
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

		a = lQ[0];
		b = lQ[1];
		c = lQ[2];
		d = lQ[3];
	}
*/
}

TEMPLATE RotationMatrix<_TVarType> QUAL::GetAsRotationMatrix() const
{
	_TVarType lAB, lAC, lAD;
	_TVarType lBB, lCC, lDD;
	_TVarType lBC, lBD, lCD;
	_TVarType lB2, lC2, lD2;

	// Calculate coefficients.
	lB2 = b + b;
	lC2 = c + c;
	lD2 = d + d;

	lBB = b * lB2;
	lBC = b * lC2;
	lBD = b * lD2;

	lCC = c * lC2;
	lCD = c * lD2;
	lDD = d * lD2;

	lAB = a * lB2;
	lAC = a * lC2;
	lAD = a * lD2;

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
	lB2 = b + b;
	lC2 = c + c;
	lD2 = d + d;

	lBB = b * lB2;
	lBC = b * lC2;
	lBD = b * lD2;

	lCC = c * lC2;
	lCD = c * lD2;
	lDD = d * lD2;

	lAB = a * lB2;
	lAC = a * lC2;
	lAD = a * lD2;

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
	a += pA;
	b += pB;
	c += pC;
	d += pD;
}

TEMPLATE void QUAL::Add(const Quaternion& pQuaternion)
{
	a += pQuaternion.a;
	b += pQuaternion.b;
	c += pQuaternion.c;
	d += pQuaternion.d;
}

TEMPLATE void QUAL::Sub(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	a -= pA;
	b -= pB;
	c -= pC;
	d -= pD;
}

TEMPLATE void QUAL::Sub(const Quaternion& pQuaternion)
{
	a -= pQuaternion.a;
	b -= pQuaternion.b;
	c -= pQuaternion.c;
	d -= pQuaternion.d;
}

TEMPLATE void QUAL::Mul(_TVarType pX)
{
	a *= pX;
	b *= pX;
	c *= pX;
	d *= pX;
}

TEMPLATE void QUAL::Div(_TVarType pX)
{
	a /= pX;
	b /= pX;
	c /= pX;
	d /= pX;
}

TEMPLATE void QUAL::Mul(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	_TVarType lA = a * pA - b * pB - c * pC - d * pD;
	_TVarType lB = a * pB + b * pA + c * pD - d * pC;
	_TVarType lC = a * pC + c * pA + d * pB - b * pD;
	_TVarType lD = a * pD + d * pA + b * pC - c * pB;

	a = lA;
	b = lB;
	c = lC;
	d = lD;
}

TEMPLATE void QUAL::Mul(const _TVarType pData[4])
{
	_TVarType lA = mData[0] * pData[0] - mData[1] * pData[1] - mData[2] * pData[2] - mData[3] * pData[3];
	_TVarType lB = mData[0] * pData[1] + mData[1] * pData[0] + mData[2] * pData[3] - mData[3] * pData[2];
	_TVarType lC = mData[0] * pData[2] + mData[2] * pData[0] + mData[3] * pData[1] - mData[1] * pData[3];
	_TVarType lD = mData[0] * pData[3] + mData[3] * pData[0] + mData[1] * pData[2] - mData[2] * pData[1];

	mData[0] = lA;
	mData[1] = lB;
	mData[2] = lC;
	mData[3] = lD;
}

TEMPLATE void QUAL::Mul(const Quaternion& pQuaternion)
{
	Mul(pQuaternion.mData);
}

TEMPLATE void QUAL::AMulInvB(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	_TVarType lA =  a * pA + b * pB + c * pC + d * pD;
	_TVarType lB = -a * pB + b * pA - c * pD + d * pC;
	_TVarType lC = -a * pC + c * pA - d * pB + b * pD;
	_TVarType lD = -a * pD + d * pA - b * pC + c * pB;

	_TVarType lLRecip = (_TVarType)1.0 / (pA * pA + pB * pB + pC * pC + pD * pD);

	a = lA * lLRecip;
	b = lB * lLRecip;
	c = lC * lLRecip;
	d = lD * lLRecip;
}

TEMPLATE void QUAL::AMulInvB(const _TVarType pData[4])
{
	_TVarType lA =  mData[0] * pData[0] + mData[1] * pData[1] + mData[2] * pData[2] + mData[3] * pData[3];
	_TVarType lB = -mData[0] * pData[1] + mData[1] * pData[0] - mData[2] * pData[3] + mData[3] * pData[2];
	_TVarType lC = -mData[0] * pData[2] + mData[2] * pData[0] - mData[3] * pData[1] + mData[1] * pData[3];
	_TVarType lD = -mData[0] * pData[3] + mData[3] * pData[0] - mData[1] * pData[2] + mData[2] * pData[1];

	_TVarType lLRecip = (_TVarType)1.0 / (pData[0] * pData[0] + pData[1] * pData[1] + pData[2] * pData[2] + pData[3] * pData[3]);

	mData[0] = lA * lLRecip;
	mData[1] = lB * lLRecip;
	mData[2] = lC * lLRecip;
	mData[3] = lD * lLRecip;
}

TEMPLATE void QUAL::AMulInvB(const Quaternion& pQuaternion)
{
	AMulInvB(pQuaternion.mData);
}

TEMPLATE void QUAL::InvAMulB(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	_TVarType lA = a * pA + b * pB + c * pC + d * pD;
	_TVarType lB = a * pB - b * pA - c * pD + d * pC;
	_TVarType lC = a * pC - c * pA - d * pB + b * pD;
	_TVarType lD = a * pD - d * pA - b * pC + c * pB;

	_TVarType lLRecip = (_TVarType)1.0 / GetNorm();

	a = lA * lLRecip;
	b = lB * lLRecip;
	c = lC * lLRecip;
	d = lD * lLRecip;
}

TEMPLATE void QUAL::FastInvAMulB(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	_TVarType lA = a * pA + b * pB + c * pC + d * pD;
	_TVarType lB = a * pB - b * pA - c * pD + d * pC;
	_TVarType lC = a * pC - c * pA - d * pB + b * pD;
	_TVarType lD = a * pD - d * pA - b * pC + c * pB;

	a = lA;
	b = lB;
	c = lC;
	d = lD;
}

TEMPLATE void QUAL::InvAMulB(const _TVarType pData[4])
{
	_TVarType lA = mData[0] * pData[0] + mData[1] * pData[1] + mData[2] * pData[2] + mData[3] * pData[3];
	_TVarType lB = mData[0] * pData[1] - mData[1] * pData[0] - mData[2] * pData[3] + mData[3] * pData[2];
	_TVarType lC = mData[0] * pData[2] - mData[2] * pData[0] - mData[3] * pData[1] + mData[1] * pData[3];
	_TVarType lD = mData[0] * pData[3] - mData[3] * pData[0] - mData[1] * pData[2] + mData[2] * pData[1];

	_TVarType lLRecip = (_TVarType)1.0 / GetNorm();

	mData[0] = lA * lLRecip;
	mData[1] = lB * lLRecip;
	mData[2] = lC * lLRecip;
	mData[3] = lD * lLRecip;
}

TEMPLATE void QUAL::InvAMulB(const Quaternion& pQuaternion)
{
	InvAMulB(pQuaternion.mData);
}

TEMPLATE void QUAL::InvAMulInvB(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	_TVarType lA =  a * pA - b * pB - c * pC - d * pD;
	_TVarType lB = -a * pB - b * pA + c * pD - d * pC;
	_TVarType lC = -a * pC - c * pA + d * pB - b * pD;
	_TVarType lD = -a * pD - d * pA + b * pC - c * pB;

	_TVarType lLRecip = (_TVarType)1.0 / 
		(
			(a * a + b * b + c * c + d * d) *
			(pA * pA + pB * pB + pC * pC + pD * pD)
		);

	a = lA * lLRecip;
	b = lB * lLRecip;
	c = lC * lLRecip;
	d = lD * lLRecip;
}

TEMPLATE void QUAL::InvAMulInvB(const _TVarType pData[4])
{
	_TVarType lA =  mData[0] * pData[0] - mData[1] * pData[1] - mData[2] * pData[2] - mData[3] * pData[3];
	_TVarType lB = -mData[0] * pData[1] - mData[1] * pData[0] + mData[2] * pData[3] - mData[3] * pData[2];
	_TVarType lC = -mData[0] * pData[2] - mData[2] * pData[0] + mData[3] * pData[1] - mData[1] * pData[3];
	_TVarType lD = -mData[0] * pData[3] - mData[3] * pData[0] + mData[1] * pData[2] - mData[2] * pData[1];

	_TVarType lLRecip = (_TVarType)1.0 / 
		(
			(mData[0] * mData[0] + mData[1] * mData[1] + mData[2] * mData[2] + mData[3] * mData[3]) *
			(pData[0] * pData[0] + pData[1] * pData[1] + pData[2] * pData[2] + pData[3] * pData[3])
		);

	mData[0] = lA * lLRecip;
	mData[1] = lB * lLRecip;
	mData[2] = lC * lLRecip;
	mData[3] = lD * lLRecip;
}

TEMPLATE void QUAL::InvAMulInvB(const Quaternion& pQuaternion)
{
	InvAMulInvB(pQuaternion.mData);
}


TEMPLATE void QUAL::BMulA(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	_TVarType lA = pA * a - pB * b - pC * c - pD * d;
	_TVarType lB = pA * b + pB * a + pC * d - pD * c;
	_TVarType lC = pA * c + pC * a + pD * b - pB * d;
	_TVarType lD = pA * d + pD * a + pB * c - pC * b;

	a = lA;
	b = lB;
	c = lC;
	d = lD;
}

TEMPLATE void QUAL::BMulA(const _TVarType pData[4])
{
	_TVarType lA = pData[0] * mData[0] - pData[1] * mData[1] - pData[2] * mData[2] - pData[3] * mData[3];
	_TVarType lB = pData[0] * mData[1] + pData[1] * mData[0] + pData[2] * mData[3] - pData[3] * mData[2];
	_TVarType lC = pData[0] * mData[2] + pData[2] * mData[0] + pData[3] * mData[1] - pData[1] * mData[3];
	_TVarType lD = pData[0] * mData[3] + pData[3] * mData[0] + pData[1] * mData[2] - pData[2] * mData[1];

	mData[0] = lA;
	mData[1] = lB;
	mData[2] = lC;
	mData[3] = lD;
}

TEMPLATE void QUAL::BMulA(const Quaternion& pQuaternion)
{
	BMulA(pQuaternion.mData);
}


TEMPLATE void QUAL::InvBMulA(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	_TVarType lA = pA * a + pB * b + pC * c + pD * d;
	_TVarType lB = pA * b - pB * a - pC * d + pD * c;
	_TVarType lC = pA * c - pC * a - pD * b + pB * d;
	_TVarType lD = pA * d - pD * a - pB * c + pC * b;

	_TVarType lLRecip = (_TVarType)1.0 / (pA * pA + pB * pB + pC * pC + pD * pD);

	a = lA * lLRecip;
	b = lB * lLRecip;
	c = lC * lLRecip;
	d = lD * lLRecip;
}

TEMPLATE void QUAL::InvBMulA(const _TVarType pData[4])
{
	_TVarType lA = pData[0] * mData[0] + pData[1] * mData[1] + pData[2] * mData[2] + pData[3] * mData[3];
	_TVarType lB = pData[0] * mData[1] - pData[1] * mData[0] - pData[2] * mData[3] + pData[3] * mData[2];
	_TVarType lC = pData[0] * mData[2] - pData[2] * mData[0] - pData[3] * mData[1] + pData[1] * mData[3];
	_TVarType lD = pData[0] * mData[3] - pData[3] * mData[0] - pData[1] * mData[2] + pData[2] * mData[1];

	_TVarType lLRecip = (_TVarType)1.0 / (pData[0] * pData[0] + pData[1] * pData[1] + pData[2] * pData[2] + pData[3] * pData[3]);

	mData[0] = lA * lLRecip;
	mData[1] = lB * lLRecip;
	mData[2] = lC * lLRecip;
	mData[3] = lD * lLRecip;
}

TEMPLATE void QUAL::InvBMulA(const Quaternion& pQuaternion)
{
	InvBMulA(pQuaternion.mData);
}

TEMPLATE void QUAL::BMulInvA(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	_TVarType lA =  pA * a + pB * b + pC * c + pD * d;
	_TVarType lB = -pA * b + pB * a - pC * d + pD * c;
	_TVarType lC = -pA * c + pC * a - pD * b + pB * d;
	_TVarType lD = -pA * d + pD * a - pB * c + pC * b;

	_TVarType lLRecip = (_TVarType)1.0 / GetNorm();

	a = lA * lLRecip;
	b = lB * lLRecip;
	c = lC * lLRecip;
	d = lD * lLRecip;
}

TEMPLATE void QUAL::BMulInvA(const _TVarType pData[4])
{
	_TVarType lA =  pData[0] * mData[0] + pData[1] * mData[1] + pData[2] * mData[2] + pData[3] * mData[3];
	_TVarType lB = -pData[0] * mData[1] + pData[1] * mData[0] - pData[2] * mData[3] + pData[3] * mData[2];
	_TVarType lC = -pData[0] * mData[2] + pData[2] * mData[0] - pData[3] * mData[1] + pData[1] * mData[3];
	_TVarType lD = -pData[0] * mData[3] + pData[3] * mData[0] - pData[1] * mData[2] + pData[2] * mData[1];

	_TVarType lLRecip = (_TVarType)1.0 / GetNorm();

	mData[0] = lA * lLRecip;
	mData[1] = lB * lLRecip;
	mData[2] = lC * lLRecip;
	mData[3] = lD * lLRecip;
}

TEMPLATE void QUAL::BMulInvA(const Quaternion& pQuaternion)
{
	BMulInvA(pQuaternion.mData);
}

TEMPLATE void QUAL::InvBMulInvA(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	_TVarType lA =  pA * a - pB * b - pC * c - pD * d;
	_TVarType lB = -pA * b - pB * a + pC * d - pD * c;
	_TVarType lC = -pA * c - pC * a + pD * b - pB * d;
	_TVarType lD = -pA * d - pD * a + pB * c - pC * b;

	_TVarType lLRecip = (_TVarType)1.0 / 
		(
			(a * a + b * b + c * c + d * d) *
			(pA * pA + pB * pB + pC * pC + pD * pD)
		);

	a = lA * lLRecip;
	b = lB * lLRecip;
	c = lC * lLRecip;
	d = lD * lLRecip;
}

TEMPLATE void QUAL::InvBMulInvA(const _TVarType pData[4])
{
	_TVarType lA =  pData[0] * mData[0] - pData[1] * mData[1] - pData[2] * mData[2] - pData[3] * mData[3];
	_TVarType lB = -pData[0] * mData[1] - pData[1] * mData[0] + pData[2] * mData[3] - pData[3] * mData[2];
	_TVarType lC = -pData[0] * mData[2] - pData[2] * mData[0] + pData[3] * mData[1] - pData[1] * mData[3];
	_TVarType lD = -pData[0] * mData[3] - pData[3] * mData[0] + pData[1] * mData[2] - pData[2] * mData[1];

	_TVarType lLRecip = (_TVarType)1.0 / 
		(
			(mData[0] * mData[0] + mData[1] * mData[1] + mData[2] * mData[2] + mData[3] * mData[3]) *
			(pData[0] * pData[0] + pData[1] * pData[1] + pData[2] * pData[2] + pData[3] * pData[3])
		);

	mData[0] = lA * lLRecip;
	mData[1] = lB * lLRecip;
	mData[2] = lC * lLRecip;
	mData[3] = lD * lLRecip;
}

TEMPLATE void QUAL::InvBMulInvA(const Quaternion& pQuaternion)
{
	InvBMulInvA(pQuaternion.mData);
}



TEMPLATE void QUAL::Div(_TVarType pA, _TVarType pB, _TVarType pC, _TVarType pD)
{
	AMulInvB(pA, pB, pC, pD);
}

TEMPLATE void QUAL::Div(const _TVarType pData[4])
{
	// Division is defined as qp', where q = this, and p' is the inverse
	// of the quaternion pData.
	// Note that we can redefine division as p'q as well, but with other
	// results.
	AMulInvB(pData);

	// Division is defined as q'p, where q' = inverse of this, and p is the
	// quaternion (pA, pB, pC, pD).
	//InvAMulB(pA, pB, pC, pD);
}

TEMPLATE void QUAL::Div(const Quaternion& pQuaternion)
{
	Div(pQuaternion.mData);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetRotatedVector(const Vector3D<_TVarType>& pVector) const
{
	Quaternion lQ(0, pVector.x, pVector.y, pVector.z);
	lQ = (*this) * lQ * GetInverse();	// TODO: assume unit, and use conjugate instead of inverse.

	Vector3D<_TVarType> lV(lQ.b, lQ.c, lQ.d);
	return lV;
}

TEMPLATE void QUAL::FastRotatedVector(const Quaternion& pInverse, Vector3D<_TVarType>& pTarget, const Vector3D<_TVarType>& pSource) const
{
	Quaternion<_TVarType> lQ(*this);
	lQ.Mul(0, pSource.x, pSource.y, pSource.z);
	lQ.Mul(pInverse);
	pTarget.x = lQ.b;
	pTarget.y = lQ.c;
	pTarget.z = lQ.d;
}

TEMPLATE Vector3D<_TVarType> QUAL::GetInverseRotatedVector(const Vector3D<_TVarType>& pVector) const
{
	Quaternion lQ(0, pVector.x, pVector.y, pVector.z);
	lQ = GetInverse() * lQ * (*this);	// TODO: assume unit, and use conjugate instead of inverse.
	return Vector3D<_TVarType>(lQ.b, lQ.c, lQ.d);
}

TEMPLATE void QUAL::FastInverseRotatedVector(const Quaternion& pInverse, Vector3D<_TVarType>& pTarget, const Vector3D<_TVarType>& pSource) const
{
	Quaternion<_TVarType> lQ(pInverse);
	lQ.Mul(0, pSource.x, pSource.y, pSource.z);
	lQ.Mul(a, b, c, d);
	pTarget.x = lQ.b;
	pTarget.y = lQ.c;
	pTarget.z = lQ.d;
}

TEMPLATE void QUAL::MakeConjugate()
{
	b = -b;
	c = -c;
	d = -d;
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
	_TVarType l2xA = a * 2.0f;
	a  = a * a - (b * b + c * c + d * d);
	b *= l2xA;
	c *= l2xA;
	d *= l2xA;
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
	return mData[0] * mData[0] + mData[1] * mData[1] + mData[2] * mData[2] + mData[3] * mData[3];
}

TEMPLATE _TVarType QUAL::GetMagnitude() const
{
	return sqrt(GetNorm());
}

TEMPLATE void QUAL::Negate()
{
	a = -a;
	b = -b;
	c = -c;
	d = -d;
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
	_TVarType lCosOmega = pFrom.a * pTo.a +
				pFrom.b * pTo.b +
				pFrom.c * pTo.c +
				pFrom.d * pTo.d;

	_TVarType lTo[4];

	// Adjust signs if necessary.
	if (lCosOmega < 0)
	{
		lCosOmega = -lCosOmega;
		lTo[0] = -pTo.a;
		lTo[1] = -pTo.b;
		lTo[2] = -pTo.c;
		lTo[3] = -pTo.d;
	}
	else
	{
		lTo[0] = pTo.a;
		lTo[1] = pTo.b;
		lTo[2] = pTo.c;
		lTo[3] = pTo.d;
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
	a = pFrom.a * lScale0 + lTo[0] * lScale1;
	b = pFrom.b * lScale0 + lTo[1] * lScale1;
	c = pFrom.c * lScale0 + lTo[2] * lScale1;
	d = pFrom.d * lScale0 + lTo[3] * lScale1;
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
	pVector.Set(b, c, d);
}

TEMPLATE Vector3D<_TVarType> QUAL::GetRotationVector() const
{
	return Vector3D<_TVarType>(b, c, d);
}

TEMPLATE _TVarType QUAL::GetRotationAngle() const
{
	_TVarType lAngle;
	if (a <= -1)
	{
		lAngle = PI;
	}
	else if (a >= 1)
	{
		lAngle = 0;
	}
	else
	{
		lAngle = ::acos(a);
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
	const _TVarType lW2 = a*a;
	const _TVarType lX2 = b*b;
	const _TVarType lY2 = c*c;
	const _TVarType lZ2 = d*d;
	const _TVarType lUnitLength = lW2 + lX2 + lY2 + lZ2;	// Normalised == 1, otherwise correction divisor.
	const _TVarType abcd = a*b + c*d;
	_TVarType lYaw;
	_TVarType lPitch;
	_TVarType lRoll;
	if (abcd > ((_TVarType)0.5-MathTraits<_TVarType>::FullEps())*lUnitLength)
	{
		lYaw = 2 * atan2(c, a);
		lPitch = (_TVarType)PI/2;
		lRoll = 0;
	}
	else if (abcd < (-(_TVarType)0.5+MathTraits<_TVarType>::FullEps())*lUnitLength)
	{
		lYaw = -2 * ::atan2(c, a);
		lPitch = -(_TVarType)PI/2;
		lRoll = 0;
	}
	else
	{
		const _TVarType adbc = a*d - b*c;
		const _TVarType acbd = a*c - b*d;
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
	lC2 = c + c;
	lD2 = d + d;

	lBC = b * lC2;
	lBD = b * lD2;

	lCC = c * lC2;
	lDD = d * lD2;

	lAC = a * lC2;
	lAD = a * lD2;

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
	lB2 = b + b;
	lC2 = c + c;
	lD2 = d + d;

	lBB = b * lB2;
	lDD = d * lD2;

	lBC = b * lC2;
	lCD = c * lD2;

	lAB = a * lB2;
	lAD = a * lD2;

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
	lB2 = b + b;
	lC2 = c + c;
	lD2 = d + d;

	lBB = b * lB2;
	lCC = c * lC2;

	lBD = b * lD2;
	lCD = c * lD2;

	lAB = a * lB2;
	lAC = a * lC2;

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
	return a == pQ.a &&
	       b == pQ.b &&
	       c == pQ.c &&
	       d == pQ.d;
}

TEMPLATE bool QUAL::operator!= (const Quaternion<_TVarType>& pQ) const
{
	return a != pQ.a ||
	       b != pQ.b ||
	       c != pQ.c ||
	       d != pQ.d;
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
	a *= pScalar;
	b *= pScalar;
	c *= pScalar;
	d *= pScalar;
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
	a *= pScalar;
	b *= pScalar;
	c *= pScalar;
	d *= pScalar;
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
	return Quaternion<float>((float)a, (float)b, (float)c, (float)d);
}

TEMPLATE Quaternion<double> QUAL::ToDouble() const
{
	return Quaternion<double>((double)a, (double)b, (double)c, (double)d);
}

#undef TEMPLATE
#undef QUAL
