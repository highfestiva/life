
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
	mGfxMesh(0),
	mGfxMeshId(0),
	mPhysMeshBodyId(0),
	mBodyIndexData(0),
	mLevel(0)
{
}

Level::~Level()
{
}



void Level::GenerateLevel(TBC::PhysicsManager* pPhysicsManager, int pLevel)
{
	(void)pLevel;

	mGfxMesh = CreateTriangleBox(4.5f, 4.5f, 3);
	FlipTriangles(mGfxMesh);
	GenerateVertexColors(mGfxMesh);
	mGfxMesh->SetAlwaysVisible(true);
	mGfxMesh->GetBasicMaterialSettings().mDiffuse	= Vector3DF(0.9f,0.85f,0.3f);
	mGfxMesh->GetBasicMaterialSettings().mSpecular	= Vector3DF(0.5f,0.5f,0.5f);
	mGfxMesh->GetBasicMaterialSettings().mShininess	= false;
	mGfxMesh->GetBasicMaterialSettings().mSmooth	= false;
	mGfxMesh->SetGeometryVolatility(TBC::GeometryBase::GEOM_SEMI_STATIC);
	mGfxMeshId = mUiManager->GetRenderer()->AddGeometry(mGfxMesh, UiTbc::Renderer::MAT_VERTEX_COLOR_SOLID, UiTbc::Renderer::FORCE_NO_SHADOWS);

	CreatePhysicsMesh(pPhysicsManager);
}

const UiTbc::TriangleBasedGeometry* Level::GetMesh() const
{
	return mGfxMesh;
}

void Level::SetTriangles(TBC::PhysicsManager* pPhysicsManager, const std::vector<float>& pVertices, const std::vector<uint8>& pColors)
{
	mUiManager->GetRenderer()->RemoveGeometry(mGfxMeshId);

	std::vector<vtx_idx_t> ni;
	const size_t vc = pVertices.size()/3;
	for (size_t x = 0; x < vc; ++x)
	{
		ni.push_back(x);
	}

	mGfxMesh->Set(&pVertices[0], 0, 0, &pColors[0], TBC::GeometryBase::COLOR_RGBA, &ni[0], vc, vc, TBC::GeometryBase::TRIANGLES, TBC::GeometryBase::GEOM_SEMI_STATIC);
	FlipTriangles(mGfxMesh);
	mGfxMesh->SetAlwaysVisible(true);
	mGfxMeshId = mUiManager->GetRenderer()->AddGeometry(mGfxMesh, UiTbc::Renderer::MAT_VERTEX_COLOR_SOLID, UiTbc::Renderer::FORCE_NO_SHADOWS);
	CreatePhysicsMesh(pPhysicsManager);
}



UiTbc::TriangleBasedGeometry* Level::CreateTriangleBox(float x, float y, float z)
{
	// Create all triangles with unique vertices, we don't want triangles sharing
	// vertices as that would complicate cutting.
	x *= 0.5f;
	y *= 0.5f;
	z *= 0.5f;
	Vector3DF v[] =
	{
		Vector3DF(-x, +y, +z), Vector3DF(+x, +y, +z), Vector3DF(-x, +y, -z),
		Vector3DF(-x, +y, -z), Vector3DF(+x, +y, +z), Vector3DF(+x, +y, -z),
		Vector3DF(+x, -y, +z), Vector3DF(-x, -y, +z), Vector3DF(+x, -y, -z),
		Vector3DF(+x, -y, -z), Vector3DF(-x, -y, +z), Vector3DF(-x, -y, -z),

		Vector3DF(-x, -y, +z), Vector3DF(-x, +y, +z), Vector3DF(-x, -y, -z),
		Vector3DF(-x, -y, -z), Vector3DF(-x, +y, +z), Vector3DF(-x, +y, -z),
		Vector3DF(+x, +y, +z), Vector3DF(+x, -y, +z), Vector3DF(+x, +y, -z),
		Vector3DF(+x, +y, -z), Vector3DF(+x, -y, +z), Vector3DF(+x, -y, -z),

		Vector3DF(-x, -y, +z), Vector3DF(+x, -y, +z), Vector3DF(-x, +y, +z),
		Vector3DF(-x, +y, +z), Vector3DF(+x, -y, +z), Vector3DF(+x, +y, +z),
		Vector3DF(-x, +y, -z), Vector3DF(+x, +y, -z), Vector3DF(-x, -y, -z),
		Vector3DF(-x, -y, -z), Vector3DF(+x, +y, -z), Vector3DF(+x, -y, -z),
	};
	std::vector<vtx_idx_t> ni;
	const size_t vc = LEPRA_ARRAY_COUNT(v);
	for (size_t x = 0; x < vc; ++x)
	{
		ni.push_back(x);
	}
	return new UiTbc::TriangleBasedGeometry(&v[0], 0, 0, 0, TBC::GeometryBase::COLOR_RGBA, &ni[0], vc, vc, TBC::GeometryBase::TRIANGLES, TBC::GeometryBase::GEOM_DYNAMIC);
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
		const int c = x / 6 % lColorCount;
		Color& lColor = lColors[c];
		lColorData[x*4+0] = lColor.mRed;
		lColorData[x*4+1] = lColor.mGreen;
		lColorData[x*4+2] = lColor.mBlue;
		lColorData[x*4+3] = 255;
	}
	pMesh->SetColorData(lColorData, TBC::GeometryBase::COLOR_RGBA);
	delete lColorData;
}

void Level::CreatePhysicsMesh(TBC::PhysicsManager* pPhysicsManager)
{
	if (mPhysMeshBodyId)
	{
		pPhysicsManager->DeleteBody(mPhysMeshBodyId);
	}

	const float lFriction = 0.7f;
	const float lBounce = 1.0f;
	const int ic = mGfxMesh->GetIndexCount();
	delete[] mBodyIndexData;
	mBodyIndexData = new uint32[ic];
	for (int x = 0; x < ic; ++x)
	{
		mBodyIndexData[x] = mGfxMesh->GetIndexData()[x];
	}
	mPhysMeshBodyId = pPhysicsManager->CreateTriMesh(true, mGfxMesh->GetVertexCount(), mGfxMesh->GetVertexData(),
		mGfxMesh->GetTriangleCount(), mBodyIndexData, TransformationF(), lFriction, lBounce, GetInstanceId());
}



}
