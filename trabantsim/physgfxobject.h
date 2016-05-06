
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uicure/include/uimachine.h"
#include "trabantsim.h"



namespace TrabantSim {


class PlacedObject {
public:
	quat orientation_;
	vec3 pos_;
	PlacedObject(const quat& orientation, const vec3& pos);
	virtual ~PlacedObject();
};

class BoxObject: public PlacedObject {
	typedef PlacedObject Parent;
public:
	vec3 size_;
	BoxObject(const quat& _quat, const vec3& pos, const vec3& size);
};

class SphereObject: public PlacedObject {
	typedef PlacedObject Parent;
public:
	float radius_;
	SphereObject(const quat& _quat, const vec3& pos, float radius);
};

class CapsuleObject: public PlacedObject {
	typedef PlacedObject Parent;
public:
	float radius_;
	float length_;
	CapsuleObject(const quat& _quat, const vec3& pos, float radius, float length);
};

class MeshObject: public PlacedObject {
	typedef PlacedObject Parent;
public:
	std::vector<float> vertices_;
	std::vector<int> indices_;
	MeshObject();
	MeshObject(const quat& _quat, const vec3& pos);
};



typedef std::vector<PlacedObject*> PhysObjectArray;



enum ObjectMaterial {
	MaterialSmooth = 1,
	MaterialFlat,
	MaterialChecker,
	MaterialNoise
};



}
