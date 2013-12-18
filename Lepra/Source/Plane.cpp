
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/Plane.h"



namespace Lepra
{



Plane::Plane():
	d(0)
{
}

Plane::Plane(const Vector3DF& pNormal, float D):
	n(pNormal),
	d(D)
{
}

Plane::Plane(const Vector3DF& pPosition, const Vector3DF& pTangent, const Vector3DF& pApproximateNormal)
{
	const Vector3DF lTangent2 = pTangent.Cross(pApproximateNormal);
	n = lTangent2.Cross(pTangent);
	n.Normalize();
	d = n*pPosition;
}

void Plane::operator=(const Plane& pCopy)
{
	n = pCopy.n;
	d = pCopy.d;
}



float Plane::GetAbsDistance(const Vector3DF& pPosition) const
{
	return std::abs(GetDistance(pPosition));
}

float Plane::GetDistance(const Vector3DF& pPosition) const
{
	return n*pPosition-d;
}

QuaternionF Plane::GetOrientation() const
{
	Vector3DF x(1,0,0);
	if (std::abs(n*x) > 0.7f)
	{
		x = Vector3DF(0,1,0);
	}
	Vector3DF y = n.Cross(x);
	x = y.Cross(n);
	return QuaternionF(RotationMatrixF(x,y,n));
}



Plane Plane::operator-() const
{
	return Plane(-n, -d);
}



}
