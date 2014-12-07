
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "Tv3d.h"



namespace Tv3d
{


class PlacedObject
{
public:
	quat q;
	vec3 pos;
	PlacedObject(const quat& pQuat, const vec3& pPos);
};

class BoxObject: public PlacedObject
{
	typedef PlacedObject Parent;
public:
	vec3 size;
	BoxObject(const quat& pQuat, const vec3& pPos, const vec3& pSize);
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



}
