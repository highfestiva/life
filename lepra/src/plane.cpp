
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/plane.h"



namespace lepra {



Plane::Plane():
	d(0) {
}

Plane::Plane(const vec3& normal, float D):
	n(normal),
	d(D) {
}

Plane::Plane(const vec3& position, const vec3& tangent, const vec3& approximate_normal) {
	const vec3 tangent2 = tangent.Cross(approximate_normal);
	n = tangent2.Cross(tangent);
	n.Normalize();
	d = n*position;
}

void Plane::operator=(const Plane& copy) {
	n = copy.n;
	d = copy.d;
}



float Plane::GetAbsDistance(const vec3& position) const {
	return std::abs(GetDistance(position));
}

float Plane::GetDistance(const vec3& position) const {
	return n*position-d;
}

quat Plane::GetOrientation() const {
	vec3 x(1,0,0);
	if (std::abs(n*x) > 0.7f) {
		x = vec3(0,1,0);
	}
	vec3 y = n.Cross(x);
	x = y.Cross(n);
	return quat(RotationMatrixF(x,y,n));
}



Plane Plane::operator-() const {
	return Plane(-n, -d);
}



}
