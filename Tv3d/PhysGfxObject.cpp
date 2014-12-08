
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "PhysGfxObject.h"



namespace Tv3d
{



PlacedObject::PlacedObject(const quat& pQuat, const vec3& pPos):
	q(pQuat),
	pos(pPos)
{
}

PlacedObject::~PlacedObject()
{
}



BoxObject::BoxObject(const quat& pQuat, const vec3& pPos, const vec3& pSize):
	Parent(pQuat,pPos),
	size(pSize)
{
}



MeshObject::MeshObject():
	Parent(quat(), vec3())
{
}

MeshObject::MeshObject(const quat& pQuat, const vec3& pPos):
	Parent(pQuat,pPos)
{
}



}
