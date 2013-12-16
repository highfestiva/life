
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
	mLevel(0),
	mOriginalVolume(1),
	mVolume(1)
{
}

Level::~Level()
{
}



void Level::GenerateLevel(TBC::PhysicsManager* pPhysicsManager, int pLevel)
{
	(void)pLevel;

	if (mGfxMeshId)
	{
		mUiManager->GetRenderer()->RemoveGeometry(mGfxMeshId);
	}

	CreateTriangleBox(4.5f, 4.5f, 3);
	mGfxMesh->GetBasicMaterialSettings().mDiffuse	= Vector3DF(0.9f,0.85f,0.3f);
	mGfxMesh->GetBasicMaterialSettings().mSpecular	= Vector3DF(0.5f,0.5f,0.5f);
	mGfxMesh->GetBasicMaterialSettings().mShininess	= false;
	mGfxMesh->GetBasicMaterialSettings().mSmooth	= false;
	mGfxMesh->SetGeometryVolatility(TBC::GeometryBase::GEOM_SEMI_STATIC);
	mGfxMeshId = mUiManager->GetRenderer()->AddGeometry(mGfxMesh, UiTbc::Renderer::MAT_VERTEX_COLOR_SOLID, UiTbc::Renderer::FORCE_NO_SHADOWS);

	CreatePhysicsMesh(pPhysicsManager);

	mOriginalVolume = CalculateVolume();
	mVolume = mOriginalVolume;
}

const UiTbc::TriangleBasedGeometry* Level::GetMesh() const
{
	return mGfxMesh;
}

float Level::GetVolumePercent() const
{
	return mVolume / mOriginalVolume;
}

void Level::SetTriangles(TBC::PhysicsManager* pPhysicsManager, const std::vector<float>& pVertices, const std::vector<uint8>& pColors)
{
	mUiManager->GetRenderer()->RemoveGeometry(mGfxMeshId);

	SetVertices(&pVertices[0], pVertices.size()/3, pColors);
	mGfxMeshId = mUiManager->GetRenderer()->AddGeometry(mGfxMesh, UiTbc::Renderer::MAT_VERTEX_COLOR_SOLID, UiTbc::Renderer::FORCE_NO_SHADOWS);
	CreatePhysicsMesh(pPhysicsManager);

	mVolume = CalculateVolume();
}

void Level::RenderOutline()
{
	const float x = mSize.x / 2;
	const float y = mSize.y / 2;
	const float z = mSize.z / 2;
	static const Vector3DF v[] =
	{
		Vector3DF(-x, +y, +z), Vector3DF(+x, +y, +z),
		Vector3DF(+x, +y, +z), Vector3DF(+x, +y, -z),
		Vector3DF(+x, +y, -z), Vector3DF(-x, +y, -z),
		Vector3DF(-x, +y, -z), Vector3DF(-x, +y, +z),

		Vector3DF(-x, -y, +z), Vector3DF(+x, -y, +z),
		Vector3DF(+x, -y, +z), Vector3DF(+x, -y, -z),
		Vector3DF(+x, -y, -z), Vector3DF(-x, -y, -z),
		Vector3DF(-x, -y, -z), Vector3DF(-x, -y, +z),

		Vector3DF(-x, -y, +z), Vector3DF(-x, +y, +z),
		Vector3DF(+x, -y, +z), Vector3DF(+x, +y, +z),
		Vector3DF(-x, -y, -z), Vector3DF(-x, +y, -z),
		Vector3DF(+x, -y, -z), Vector3DF(+x, +y, -z),
	};
	const int cnt = LEPRA_ARRAY_COUNT(v)/2;
	for (int i = 0; i < cnt; ++i)
	{
		mUiManager->GetRenderer()->DrawLine(v[i*2+0], v[i*2+1]-v[i*2+0], ORANGE);
	}
}



UiTbc::TriangleBasedGeometry* Level::CreateTriangleBox(float x, float y, float z)
{
	mSize = Vector3DF(x, y, z);

	// Create all triangles with unique vertices, we don't want triangles sharing
	// vertices as that would complicate cutting.
	x *= 0.5f;
	y *= 0.5f;
	z *= 0.5f;
	float v[] =
	{
		-x,+y,+z, +x,+y,+z, -x,+y,-z,
		-x,+y,-z, +x,+y,+z, +x,+y,-z,
		+x,-y,+z, -x,-y,+z, +x,-y,-z,
		+x,-y,-z, -x,-y,+z, -x,-y,-z,

		-x,-y,+z, -x,+y,+z, -x,-y,-z,
		-x,-y,-z, -x,+y,+z, -x,+y,-z,
		+x,+y,+z, +x,-y,+z, +x,+y,-z,
		+x,+y,-z, +x,-y,+z, +x,-y,-z,

		-x,-y,+z, +x,-y,+z, -x,+y,+z,
		-x,+y,+z, +x,-y,+z, +x,+y,+z,
		-x,+y,-z, +x,+y,-z, -x,-y,-z,
		-x,-y,-z, +x,+y,-z, +x,-y,-z,
	};
	Color lColors[] = { RED, GREEN, MAGENTA, BLUE, YELLOW, DARK_GRAY, };
	const int lColorCount = LEPRA_ARRAY_COUNT(lColors);
	const size_t vc = LEPRA_ARRAY_COUNT(v)/3;
	std::vector<uint8> lColorData;
	for (int x = 0; x < vc; ++x)
	{
		const int c = x / 6 % lColorCount;
		Color& lColor = lColors[c];
		lColorData.push_back(lColor.mRed);
		lColorData.push_back(lColor.mGreen);
		lColorData.push_back(lColor.mBlue);
		lColorData.push_back(255);
	}

	if (!mGfxMesh)
	{
		mGfxMesh = new UiTbc::TriangleBasedGeometry;
	}
	SetVertices(v, vc, lColorData);
	return mGfxMesh;
}

void Level::SetVertices(const float* v, size_t vc, const std::vector<uint8>& pColorData)
{
	std::vector<uint32> ni;
	for (size_t x = 0; x < vc; ++x)
	{
		ni.push_back(x);
	}

	mGfxMesh->Set(v, 0, 0, &pColorData[0], TBC::GeometryBase::COLOR_RGBA, &ni[0], vc, vc, TBC::GeometryBase::TRIANGLES, TBC::GeometryBase::GEOM_SEMI_STATIC);
	FlipTriangles(mGfxMesh);
	mGfxMesh->SetAlwaysVisible(true);
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

float Level::CalculateVolume() const
{
	float lVolume = 0;
	const int tc = mGfxMesh->GetVertexCount()/3;
	const float* v = mGfxMesh->GetVertexData();
	for (int x = 0; x < tc; ++x)
	{
		float Px = v[x*9+0];
		float Py = v[x*9+1];
		float Pz = v[x*9+2];
		float Qx = v[x*9+3];
		float Qy = v[x*9+4];
		float Qz = v[x*9+5];
		float Rx = v[x*9+6];
		float Ry = v[x*9+7];
		float Rz = v[x*9+8];
		lVolume += Px*Qy*Rz + Py*Qz*Rx + Pz*Qx*Ry - Px*Qz*Ry - Py*Qx*Rz - Pz*Qy*Rx;
		/*Vector3DF p0(v[x*9+0], v[x*9+1], v[x*9+2]);
		Vector3DF p1(v[x*9+3], v[x*9+4], v[x*9+5]);
		Vector3DF p2(v[x*9+6], v[x*9+7], v[x*9+8]);
		p0.x += 100;
		p1.x += 100;
		p2.x += 100;
		lVolume += p0 * p1.Cross(p2);*/
	}
	return std::abs(lVolume) / 6;
}



}
