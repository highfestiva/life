
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "Tv3d.h"



namespace Lepra
{
class Color;
class Plane;
}
namespace Tbc
{
class PhysicsManager;
}
namespace UiTbc
{
class TriangleBasedGeometry;
}



namespace Tv3d
{



class Object: public UiCure::Machine
{
	typedef UiCure::Machine Parent;
public:
	Object(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Object();

	void AddGfxMesh(const std::vector<float>& pVertices, const std::vector<int>& pIndices, const vec3& pColor);

private:
	UiTbc::TriangleBasedGeometry* mGfxMesh;
	UiTbc::Renderer::GeometryID mGfxMeshId;
};



}
