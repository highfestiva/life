
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "Level.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiTBC/Include/UiBasicMeshCreator.h"
#include "../UiTBC/Include/UiTriangleBasedGeometry.h"



namespace Bound
{



Level::Level(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager),
	mLevel(0)
{
}

Level::~Level()
{
}



void Level::GenerateLevel(TBC::PhysicsManager* pPhysicsManager, int pLevel)
{
	(void)pLevel;

	UiTbc::TriangleBasedGeometry* lLevelMesh = UiTbc::BasicMeshCreator::CreateFlatBox(4, 4, 3, 1, 1, 1);
	FlipTriangles(lLevelMesh);
	lLevelMesh->SetAlwaysVisible(true);
	mUiManager->GetRenderer()->AddGeometry(lLevelMesh, UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID_PXS, UiTbc::Renderer::CAST_SHADOWS);

	const float lFriction = 0.7f;
	const float lBounce = 1.0f;
	pPhysicsManager->CreateTriMesh(true, lLevelMesh->GetVertexCount(), lLevelMesh->GetVertexData(),
		lLevelMesh->GetTriangleCount(), lLevelMesh->GetIndexData(),
		TransformationF(), lFriction, lBounce, GetInstanceId());
}



void Level::FlipTriangles(UiTbc::TriangleBasedGeometry* pMesh)
{
	vtx_idx_t* lTriangles = pMesh->GetIndexData();
	const int tc = pMesh->GetTriangleCount();
	for (int x = 0; x < tc; ++x)
	{
		std::swap(lTriangles[x*3+1], lTriangles[x*3+2]);
	}
}



}
