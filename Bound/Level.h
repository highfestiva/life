
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "Bound.h"



namespace Lepra
{
class Color;
class Plane;
}
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
	const UiTbc::TriangleBasedGeometry* GetWindowMesh() const;
	float GetVolumePercent() const;
	void SetTriangles(TBC::PhysicsManager* pPhysicsManager, const std::vector<float>& pVertices, const std::vector<uint8>& pColors);
	void SetWindowTriangles(const std::vector<float>& pVertices);
	void AddCutPlane(TBC::PhysicsManager* pPhysicsManager, const Plane& pWindowPlane, const std::vector<float>& pVertices, const Color& pColor);
	void RenderOutline();

private:
	UiTbc::TriangleBasedGeometry* CreateTriangleBox(float x, float y, float z);
	static void SetVertices(UiTbc::TriangleBasedGeometry* pGfxMesh, const float* v, size_t vc, const uint8* pColorData);
	static void FlipTriangles(UiTbc::TriangleBasedGeometry* pMesh);
	static void GenerateVertexColors(UiTbc::TriangleBasedGeometry* pMesh);
	void CreatePhysicsMesh(TBC::PhysicsManager* pPhysicsManager);
	void AddPhysicsWindowBox(TBC::PhysicsManager* pPhysicsManager, const Plane& pPlane);
	void DeleteWindowBoxes(TBC::PhysicsManager* pPhysicsManager);
	float CalculateVolume() const;

	UiTbc::TriangleBasedGeometry* mGfxMesh;
	UiTbc::TriangleBasedGeometry* mGfxWindowMesh;
	UiTbc::Renderer::GeometryID mGfxMeshId;
	UiTbc::Renderer::GeometryID mGfxWindowMeshId;
	TBC::PhysicsManager::BodyID mPhysMeshBodyId;
	std::vector<TBC::PhysicsManager::BodyID> mPhysWindowBoxIds;
	uint32* mBodyIndexData;
	Vector3DF mSize;
	int mLevel;
	float mOriginalVolume;
	float mVolume;
};



}
