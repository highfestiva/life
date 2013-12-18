
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "Quaternion.h"
#include "Vector3D.h"



namespace Lepra
{



class Plane
{
public:
	Plane();
	Plane(const Vector3DF& pNormal, float D);
	Plane(const Vector3DF& pPosition, const Vector3DF& pTangent, const Vector3DF& pApproximateNormal);
	void operator=(const Plane& pCopy);

	float GetAbsDistance(const Vector3DF& pPosition) const;
	float GetDistance(const Vector3DF& pPosition) const;
	QuaternionF GetOrientation() const;

	Plane operator-() const;

	Vector3DF n;
	float d;
};




}