
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "Bound.h"



namespace TBC
{
class PhysicsManager;
}
namespace UiTbc
{
class TriangleBasedGeometry;
}



namespace Bound
{



class Level: public UiCure::Machine
{
	typedef UiCure::Machine Parent;
public:
	Level(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Level();

	void GenerateLevel(TBC::PhysicsManager* pPhysicsManager, int pLevel);

private:
	static void FlipTriangles(UiTbc::TriangleBasedGeometry* pMesh);
	static void GenerateVertexColors(UiTbc::TriangleBasedGeometry* pMesh);

	int mLevel;
};



}
