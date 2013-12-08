
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "Level.h"
#include "../Lepra/Include/CyclicArray.h"
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

	UiTbc::TriangleBasedGeometry* lLevelMesh = UiTbc::BasicMeshCreator::CreateFlatBox(4.5f, 4.5f, 3, 1, 1, 1);
	FlipTriangles(lLevelMesh);
	GenerateVertexColors(lLevelMesh);
	lLevelMesh->SetAlwaysVisible(true);
	lLevelMesh->GetBasicMaterialSettings().mDiffuse		= Vector3DF(0.9f,0.85f,0.3f);
	lLevelMesh->GetBasicMaterialSettings().mSpecular	= Vector3DF(0.5f,0.5f,0.5f);
	lLevelMesh->GetBasicMaterialSettings().mShininess	= false;
	lLevelMesh->GetBasicMaterialSettings().mSmooth		= false;
	mUiManager->GetRenderer()->AddGeometry(lLevelMesh, UiTbc::Renderer::MAT_VERTEX_COLOR_SOLID, UiTbc::Renderer::FORCE_NO_SHADOWS);

	const float lFriction = 0.7f;
	const float lBounce = 1.0f;
	const int ic = lLevelMesh->GetIndexCount();
	uint32* lIndexData = new uint32[ic];
	for (int x = 0; x < ic; ++x)
	{
		lIndexData[x] = lLevelMesh->GetIndexData()[x];
	}
	pPhysicsManager->CreateTriMesh(true, lLevelMesh->GetVertexCount(), lLevelMesh->GetVertexData(),
		lLevelMesh->GetTriangleCount(), lIndexData,
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

void Level::GenerateVertexColors(UiTbc::TriangleBasedGeometry* pMesh)
{
	Color lColors[] = { RED, GREEN, MAGENTA, BLUE, YELLOW, DARK_GRAY, };
	const int lColorCount = LEPRA_ARRAY_COUNT(lColors);
	const int vc = pMesh->GetVertexCount();
	uint8* lColorData = new uint8[vc*4];
	for (int x = 0; x < vc; ++x)
	{
		const int c = x / 4 % lColorCount;
		Color& lColor = lColors[c];
		lColorData[x*4+0] = lColor.mRed;
		lColorData[x*4+1] = lColor.mGreen;
		lColorData[x*4+2] = lColor.mBlue;
		lColorData[x*4+3] = 255;
	}
	pMesh->SetColorData(lColorData, TBC::GeometryBase::COLOR_RGBA);
	delete lColorData;
}



}
