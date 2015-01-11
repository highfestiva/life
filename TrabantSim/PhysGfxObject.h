
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "TrabantSim.h"



namespace TrabantSim
{


class PlacedObject
{
public:
	quat mOrientation;
	vec3 mPos;
	PlacedObject(const quat& pOrientation, const vec3& pPos);
	virtual ~PlacedObject();
};

class BoxObject: public PlacedObject
{
	typedef PlacedObject Parent;
public:
	vec3 mSize;
	BoxObject(const quat& pQuat, const vec3& pPos, const vec3& pSize);
};

class SphereObject: public PlacedObject
{
	typedef PlacedObject Parent;
public:
	float mRadius;
	SphereObject(const quat& pQuat, const vec3& pPos, float pRadius);
};

class MeshObject: public PlacedObject
{
	typedef PlacedObject Parent;
public:
	std::vector<float> mVertices;
	std::vector<int> mIndices;
	MeshObject();
	MeshObject(const quat& pQuat, const vec3& pPos);
};



typedef std::vector<PlacedObject*> PhysObjectArray;



enum ObjectMaterial
{
	MaterialSmooth = 1,
	MaterialFlat,
	MaterialChecker
};



}
