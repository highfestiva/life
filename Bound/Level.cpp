
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "Level.h"
#include "../Lepra/Include/CyclicArray.h"
#include "../Lepra/Include/Plane.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiTBC/Include/UiBasicMeshCreator.h"
#include "../UiTBC/Include/UiTriangleBasedGeometry.h"
#include "RtVar.h"



namespace Bound
{



Level::Level(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager),
	mGfxMesh(0),
	mGfxWindowMesh(0),
	mGfxMeshId(0),
	mGfxWindowMeshId(0),
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



void Level::GenerateLevel(TBC::PhysicsManager* pPhysicsManager, bool pVaryShapes, int pLevel)
{
	(void)pLevel;

	if (mGfxMeshId)
	{
		mUiManager->GetRenderer()->RemoveGeometry(mGfxMeshId);
		mGfxMeshId = 0;
	}
	if (mGfxWindowMeshId)
	{
		mUiManager->GetRenderer()->RemoveGeometry(mGfxWindowMeshId);
		mGfxWindowMeshId = 0;
	}

	CreateMesh(pVaryShapes, pLevel, 4.5f, 4.5f, 3);
	mGfxMesh->GetBasicMaterialSettings().mDiffuse	= Vector3DF(0.5f,0.5f,0.5f);
	mGfxMesh->GetBasicMaterialSettings().mSpecular	= Vector3DF(0,0,0);
	mGfxMesh->GetBasicMaterialSettings().mShininess	= false;
	mGfxMesh->GetBasicMaterialSettings().mSmooth	= false;
	mGfxMesh->SetGeometryVolatility(TBC::GeometryBase::GEOM_SEMI_STATIC);
	mGfxMeshId = mUiManager->GetRenderer()->AddGeometry(mGfxMesh, UiTbc::Renderer::MAT_VERTEX_COLOR_SOLID, UiTbc::Renderer::FORCE_NO_SHADOWS);

	delete mGfxWindowMesh;
	mGfxWindowMesh = 0;

	CreatePhysicsMesh(pPhysicsManager);
	DeleteWindowBoxes(pPhysicsManager);

	mOriginalVolume = CalculateVolume();
	mVolume = mOriginalVolume;
}

const UiTbc::TriangleBasedGeometry* Level::GetMesh() const
{
	return mGfxMesh;
}

const UiTbc::TriangleBasedGeometry* Level::GetWindowMesh() const
{
	return mGfxWindowMesh;
}

float Level::GetVolumePart() const
{
	return mVolume / mOriginalVolume;
}

void Level::SetTriangles(TBC::PhysicsManager* pPhysicsManager, const std::vector<float>& pVertices, const std::vector<uint8>& pColors)
{
	mUiManager->GetRenderer()->RemoveGeometry(mGfxMeshId);

	SetVertices(mGfxMesh, &pVertices[0], pVertices.size()/3, &pColors[0]);
	mGfxMeshId = mUiManager->GetRenderer()->AddGeometry(mGfxMesh, UiTbc::Renderer::MAT_VERTEX_COLOR_SOLID, UiTbc::Renderer::FORCE_NO_SHADOWS);
	CreatePhysicsMesh(pPhysicsManager);

	mVolume = CalculateVolume();
}

void Level::SetWindowTriangles(const std::vector<float>& pVertices)
{
	if (mGfxWindowMeshId)
	{
		mUiManager->GetRenderer()->RemoveGeometry(mGfxWindowMeshId);
	}

	SetVertices(mGfxWindowMesh, pVertices.empty()? 0 : &pVertices[0], pVertices.size()/3, 0);
	mGfxWindowMeshId = mUiManager->GetRenderer()->AddGeometry(mGfxWindowMesh, UiTbc::Renderer::MAT_SINGLE_COLOR_BLENDED, UiTbc::Renderer::FORCE_NO_SHADOWS);
}

void Level::AddCutPlane(TBC::PhysicsManager* pPhysicsManager, const Plane& pWindowPlane, const std::vector<float>& pVertices, const Color& pColor)
{
	if (mGfxWindowMeshId)
	{
		mUiManager->GetRenderer()->RemoveGeometry(mGfxWindowMeshId);
	}

	if (!mGfxWindowMesh)
	{
		mGfxWindowMesh = new UiTbc::TriangleBasedGeometry;
	}
	mGfxWindowMesh->GetBasicMaterialSettings().mDiffuse	= Vector3DF(pColor.mRed/255.0f, pColor.mGreen/255.0f, pColor.mBlue/255.0f);
	mGfxWindowMesh->GetBasicMaterialSettings().mSpecular	= Vector3DF(1,1,1);
	mGfxWindowMesh->GetBasicMaterialSettings().mAlpha	= pColor.mAlpha/255.0f;
	mGfxWindowMesh->GetBasicMaterialSettings().mShininess	= true;
	mGfxWindowMesh->GetBasicMaterialSettings().mSmooth	= false;

	std::vector<float> lVertices(pVertices);
	const float* v = mGfxWindowMesh->GetVertexData();
	int vc = mGfxWindowMesh->GetVertexCount()*3;
	for (int x = 0; x < vc; ++x)
	{
		lVertices.push_back(v[x]);
	}
	SetVertices(mGfxWindowMesh, &lVertices[0], lVertices.size()/3, 0);
	mGfxWindowMeshId = mUiManager->GetRenderer()->AddGeometry(mGfxWindowMesh, UiTbc::Renderer::MAT_SINGLE_COLOR_BLENDED, UiTbc::Renderer::FORCE_NO_SHADOWS);

	AddPhysicsWindowBox(pPhysicsManager, pWindowPlane);
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



UiTbc::TriangleBasedGeometry* Level::CreateMesh(bool pVaryShapes, int pLevel, float x, float y, float z)
{
	if (mGfxMesh)
	{
		delete mGfxMesh;
	}
	float lMoveZ = 0;
	const int lShape = pVaryShapes? pLevel%4 : 0;
	switch (lShape)
	{
		case 0:
			mGfxMesh = UiTbc::BasicMeshCreator::CreateFlatBox(x, y, z, 1, 1, 1);
		break;
		case 1:
			mGfxMesh = UiTbc::BasicMeshCreator::CreateEllipsoid(x*0.75f, y*0.75f, z*0.7f, 14, 14);
		break;
		case 2:
			mGfxMesh = UiTbc::BasicMeshCreator::CreateCone((x+y)/2*0.6f, z*1.4f, 16);
			lMoveZ = -z*1.1f/2;
		break;
		case 3:
			mGfxMesh = UiTbc::BasicMeshCreator::CreateCylinder(z*0.6f, z*0.6f, (x+y)/2*1.2f, 14);
		break;
	}
	// Create unique vertices for each triangle to simplify cutting.
	std::vector<float> nv;
	const float* v = mGfxMesh->GetVertexData();
	const int tc = mGfxMesh->GetTriangleCount();
	const vtx_idx_t* lIndices = mGfxMesh->GetIndexData();
	for (int t = 0; t < tc*3; ++t)
	{
		nv.push_back(v[lIndices[t]*3+0]);
		nv.push_back(v[lIndices[t]*3+1]);
		nv.push_back(v[lIndices[t]*3+2] + lMoveZ);
	}

	Color lColors[] = { RED, GREEN, MAGENTA, BLUE, YELLOW, DARK_GRAY, };
	const int lColorCount = LEPRA_ARRAY_COUNT(lColors);
	const int vc = (int)nv.size()/3;
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

	SetVertices(mGfxMesh, &nv[0], vc, &lColorData[0]);
	return mGfxMesh;
}

void Level::SetVertices(UiTbc::TriangleBasedGeometry* pGfxMesh, const float* v, size_t vc, const uint8* pColorData)
{
	std::vector<uint32> ni;
	for (size_t x = 0; x < vc; ++x)
	{
		ni.push_back(x);
	}

	pGfxMesh->Set(v, 0, 0, pColorData, TBC::GeometryBase::COLOR_RGBA, ni.empty()? 0 : &ni[0], vc, vc, TBC::GeometryBase::TRIANGLES, TBC::GeometryBase::GEOM_SEMI_STATIC);
	FlipTriangles(pGfxMesh);
	pGfxMesh->SetAlwaysVisible(true);
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

void Level::AddPhysicsWindowBox(TBC::PhysicsManager* pPhysicsManager, const Plane& pPlane)
{
	TransformationF lTransform(pPlane.GetOrientation(), pPlane.n*pPlane.d);
	const float lFriction = 0.7f;
	const float lBounce = 1.0f;
	TBC::PhysicsManager::BodyID lPhysWindowId = pPhysicsManager->CreateBox(true, lTransform, 0, Vector3DF(20,20,0.1f),
		TBC::PhysicsManager::STATIC, lFriction, lBounce, GetInstanceId());
	mPhysWindowBoxIds.push_back(lPhysWindowId);
}

void Level::DeleteWindowBoxes(TBC::PhysicsManager* pPhysicsManager)
{
	std::vector<TBC::PhysicsManager::BodyID>::iterator x = mPhysWindowBoxIds.begin();
	for (; x != mPhysWindowBoxIds.end(); ++x)
	{
		pPhysicsManager->DeleteBody(*x);
	}
	mPhysWindowBoxIds.clear();
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
