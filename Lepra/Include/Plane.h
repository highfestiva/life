
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
	Plane(const vec3& pNormal, float D);
	Plane(const vec3& pPosition, const vec3& pTangent, const vec3& pApproximateNormal);
	void operator=(const Plane& pCopy);

	float GetAbsDistance(const vec3& pPosition) const;
	float GetDistance(const vec3& pPosition) const;
	quat GetOrientation() const;

	Plane operator-() const;

	vec3 n;
	float d;
};




}