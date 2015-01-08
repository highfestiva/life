
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "PhysGfxObject.h"



namespace Tv3d
{



PlacedObject::PlacedObject(const quat& pOrientation, const vec3& pPos):
	mOrientation(pOrientation),
	mPos(pPos)
{
}

PlacedObject::~PlacedObject()
{
}



BoxObject::BoxObject(const quat& pQuat, const vec3& pPos, const vec3& pSize):
	Parent(pQuat,pPos),
	mSize(pSize)
{
}



SphereObject::SphereObject(const quat& pQuat, const vec3& pPos, float pRadius):
	Parent(pQuat,pPos),
	mRadius(pRadius)
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
