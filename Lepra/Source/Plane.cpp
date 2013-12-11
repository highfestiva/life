
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



Plane Plane::operator-() const
{
	return Plane(-n, -d);
}



}
