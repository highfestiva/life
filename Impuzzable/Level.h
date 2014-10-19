
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "Impuzzable.h"



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



namespace Impuzzable
{



class Level: public UiCure::Machine
{
	typedef UiCure::Machine Parent;
public:
	Level(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Level();

	void GenerateLevel(Tbc::PhysicsManager* pPhysicsManager, bool pVaryShapes, int pLevel);
	const UiTbc::TriangleBasedGeometry* GetMesh() const;
	const UiTbc::TriangleBasedGeometry* GetWindowMesh() const;
	float GetVolumePart() const;
	void SetTriangles(Tbc::PhysicsManager* pPhysicsManager, const std::vector<float>& pVertices, const std::vector<uint8>& pColors);
	void SetWindowTriangles(const std::vector<float>& pVertices);
	void AddCutPlane(Tbc::PhysicsManager* pPhysicsManager, const Plane& pWindowPlane, const std::vector<float>& pVertices, const Color& pColor);
	void RenderOutline();

private:
	UiTbc::TriangleBasedGeometry* CreateMesh(bool pVaryShapes, int pLevel, float x, float y, float z);
	static void SetVertices(UiTbc::TriangleBasedGeometry* pGfxMesh, const float* v, size_t vc, const uint8* pColorData);
	static void FlipTriangles(UiTbc::TriangleBasedGeometry* pMesh);
	static void GenerateVertexColors(UiTbc::TriangleBasedGeometry* pMesh);
	void CreatePhysicsMesh(Tbc::PhysicsManager* pPhysicsManager);
	void AddPhysicsWindowBox(Tbc::PhysicsManager* pPhysicsManager, const Plane& pPlane);
	void DeleteWindowBoxes(Tbc::PhysicsManager* pPhysicsManager);
	float CalculateVolume() const;

	UiTbc::TriangleBasedGeometry* mGfxMesh;
	UiTbc::TriangleBasedGeometry* mGfxWindowMesh;
	UiTbc::Renderer::GeometryID mGfxMeshId;
	UiTbc::Renderer::GeometryID mGfxWindowMeshId;
	Tbc::PhysicsManager::BodyID mPhysMeshBodyId;
	std::vector<Tbc::PhysicsManager::BodyID> mPhysWindowBoxIds;
	uint32* mBodyIndexData;
	vec3 mSize;
	int mLevel;
	float mOriginalVolume;
	float mVolume;
};



}
