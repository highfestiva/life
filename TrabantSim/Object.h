
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "TrabantSim.h"



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



namespace TrabantSim
{



class Object: public UiCure::Machine
{
	typedef UiCure::Machine Parent;
public:
	Object(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Object();

	virtual const Tbc::ChunkyClass* GetClass() const;
	void AddGfxMesh(const std::vector<float>& pVertices, const std::vector<int>& pIndices, const vec3& pColor, bool pIsSmooth, int pCastsShadows);

	quat mInitialOrientation;
	quat mInitialInverseOrientation;

private:
	Tbc::ChunkyClass* mClass;
	UiTbc::TriangleBasedGeometry* mGfxMesh;
	UiTbc::Renderer::GeometryID mGfxMeshId;
};



}
