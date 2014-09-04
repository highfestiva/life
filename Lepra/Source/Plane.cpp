
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/Plane.h"



namespace Lepra
{



Plane::Plane():
	d(0)
{
}

Plane::Plane(const vec3& pNormal, float D):
	n(pNormal),
	d(D)
{
}

Plane::Plane(const vec3& pPosition, const vec3& pTangent, const vec3& pApproximateNormal)
{
	const vec3 lTangent2 = pTangent.Cross(pApproximateNormal);
	n = lTangent2.Cross(pTangent);
	n.Normalize();
	d = n*pPosition;
}

void Plane::operator=(const Plane& pCopy)
{
	n = pCopy.n;
	d = pCopy.d;
}



float Plane::GetAbsDistance(const vec3& pPosition) const
{
	return std::abs(GetDistance(pPosition));
}

float Plane::GetDistance(const vec3& pPosition) const
{
	return n*pPosition-d;
}

quat Plane::GetOrientation() const
{
	vec3 x(1,0,0);
	if (std::abs(n*x) > 0.7f)
	{
		x = vec3(0,1,0);
	}
	vec3 y = n.Cross(x);
	x = y.Cross(n);
	return quat(RotationMatrixF(x,y,n));
}



Plane Plane::operator-() const
{
	return Plane(-n, -d);
}



}
