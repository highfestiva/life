
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "quaternion.h"
#include "vector3d.h"



namespace lepra {



class Plane {
public:
	Plane();
	Plane(const vec3& normal, float D);
	Plane(const vec3& position, const vec3& tangent, const vec3& approximate_normal);
	void operator=(const Plane& copy);

	float GetAbsDistance(const vec3& position) const;
	float GetDistance(const vec3& position) const;
	quat GetOrientation() const;

	Plane operator-() const;

	vec3 n;
	float d;
};




}