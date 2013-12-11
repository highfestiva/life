
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
	const UiTbc::TriangleBasedGeometry* GetMesh() const;
	void SetTriangles(TBC::PhysicsManager* pPhysicsManager, const std::vector<float>& pVertices, const std::vector<uint8>& pColors);

private:
	static UiTbc::TriangleBasedGeometry* CreateTriangleBox(float x, float y, float z);
	static void FlipTriangles(UiTbc::TriangleBasedGeometry* pMesh);
	static void GenerateVertexColors(UiTbc::TriangleBasedGeometry* pMesh);
	void CreatePhysicsMesh(TBC::PhysicsManager* pPhysicsManager);

	UiTbc::TriangleBasedGeometry* mGfxMesh;
	UiTbc::Renderer::GeometryID mGfxMeshId;
	TBC::PhysicsManager::BodyID mPhysMeshBodyId;
	uint32* mBodyIndexData;
	int mLevel;
};



}
