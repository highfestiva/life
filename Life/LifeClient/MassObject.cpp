
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "MassObject.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Lepra/Include/CyclicArray.h"
#include "../../Lepra/Include/Random.h"
#include "../../UiTbc/Include/UiGeometryBatch.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "RtVar.h"



namespace Life
{



MassObject::MassObject(Cure::ResourceManager* pResourceManager, const str& pClassResourceName,
	UiCure::GameUiManager* pUiManager, Tbc::PhysicsManager::BodyID pTerrainBodyId, size_t pInstanceCount,
	float pSideLength):
	Parent(pResourceManager, pClassResourceName, pUiManager),
	mTerrainBodyId(pTerrainBodyId),
	mSquareInstanceCount(pInstanceCount/SQUARE_COUNT),
	mVisibleAddTerm(0.1f),
	mSquareSideLength((int)(pSideLength/SQUARE_SIDE)),
	mMiddleSquareX(0x80000000),
	mMiddleSquareY(0x80000000),
	mSeed(0)
{
	deb_assert(mSquareInstanceCount > 0);
	::memset(mSquareArray, 0, sizeof(mSquareArray));
	mFullyVisibleDistance = mSquareSideLength * (SQUARE_MID_TO_CORNER - 1.0f);
	mVisibleDistanceFactor = (1+mVisibleAddTerm)/mSquareSideLength;

	SetPhysicsTypeOverride(Cure::PHYSICS_OVERRIDE_BONES);
}

MassObject::~MassObject()
{
	for (int x = 0; x < SQUARE_COUNT; ++x)
	{
		delete mSquareArray[x];
	}
}



void MassObject::SetSeed(unsigned pSeed)
{
	mSeed = pSeed;
}

void MassObject::SetRender(bool pRender)
{
	const vec3& lCenterPosition = GetPosition();
	for (int y = 0; y < SQUARE_SIDE; ++y)
	{
		for (int x = 0; x < SQUARE_SIDE; ++x)
		{
			const size_t lOffset = y*SQUARE_SIDE+x;
			if (mSquareArray[lOffset])
			{
				vec3 lSquarePosition;
				GridToPosition(x, y, lSquarePosition);
				const float lDistance = lSquarePosition.GetDistance(lCenterPosition);
				const float lAlpha = (1.0f + mVisibleAddTerm) - Math::Lerp(0.0f, 1.0f, (lDistance-mFullyVisibleDistance)*mVisibleDistanceFactor);
				mSquareArray[lOffset]->SetRender(pRender, lAlpha);
			}
		}
	}
}

void MassObject::UiMove()
{
	if (!GetUiManager()->CanRender())	// Can only create/remove graphical mass objects if the renderer is online.
	{
		return;
	}

	int x = mMiddleSquareX;
	int y = mMiddleSquareY;

	PositionToGrid(GetPosition(), x, y);
	if (x != mMiddleSquareX || y != mMiddleSquareY)
	{
		MoveToSquare(x, y);
	}
	for (size_t v = 0; v < SQUARE_SIDE; ++v)
	{
		for (size_t u = 0; u < SQUARE_SIDE; ++u)
		{
			const size_t lOffset = v*SQUARE_SIDE+u;
			if (!mSquareArray[lOffset])
			{
				CreateSquare(u, v);
				return;	// Optimization: only create a single square every loop to avoid heavy burdon on single loop.
			}
		}
	}
}



void MassObject::OnLoaded()
{
	Parent::OnLoaded();

	for (MeshArray::const_iterator y = mMeshResourceArray.begin(); y != mMeshResourceArray.end(); ++y)
	{
		Tbc::GeometryReference* lMesh = (Tbc::GeometryReference*)(*y)->GetRamData();
		if (lMesh)
		{
			lMesh->SetAlwaysVisible(false);
		}
	}
}

void MassObject::PositionToGrid(const vec3& pPosition, int& pX, int& pY) const
{
	// 2's complement...
	pX = (pPosition.x < 0)? (int)(pPosition.x/mSquareSideLength)-1 : (int)(pPosition.x/mSquareSideLength);
	pY = (pPosition.y < 0)? (int)(pPosition.y/mSquareSideLength)-1 : (int)(pPosition.y/mSquareSideLength);
}

void MassObject::GridToPosition(int pX, int pY, vec3& pPosition) const
{
	// 2's complement...
	pPosition.x = (float)((pX-SQUARE_MID_TO_CORNER+mMiddleSquareX) * mSquareSideLength);
	pPosition.y = (float)((pY-SQUARE_MID_TO_CORNER+mMiddleSquareY) * mSquareSideLength);
}

void MassObject::MoveToSquare(int pX, int pY)
{
	const int dy = pY - mMiddleSquareY;
	const int dx = pX - mMiddleSquareX;

	// Move all existing once that are in reach.
	Square* lSquareArray[SQUARE_COUNT];
	::memcpy(lSquareArray, mSquareArray, sizeof(lSquareArray));
	for (int v = 0; v < SQUARE_SIDE; ++v)
	{
		const size_t y = v + dy;
		for (int u = 0; u < SQUARE_SIDE; ++u)
		{
			const size_t x = u + dx;
			Square* lSquare = 0;
			if (x < SQUARE_SIDE && y < SQUARE_SIDE)
			{
				const size_t lReadOffset = y*SQUARE_SIDE+x;
				lSquare = lSquareArray[lReadOffset];
				lSquareArray[lReadOffset] = 0;
			}
			const size_t lWriteOffset = v*SQUARE_SIDE+u;
			mSquareArray[lWriteOffset] = lSquare;
		}
	}
	// Destroy all that are no longer in use.
	for (int x = 0; x < SQUARE_COUNT; ++x)
	{
		delete lSquareArray[x];
	}

	mMiddleSquareX = pX;
	mMiddleSquareY = pY;
}

void MassObject::CreateSquare(size_t pX, size_t pY)
{
	deb_assert(pX < SQUARE_SIDE && pY < SQUARE_SIDE);
	deb_assert(!mSquareArray[pY*SQUARE_SIDE+pX]);

	uint32 lSeed = mSeed + (uint32)((pY<<16)+pX);
	std::vector<xform> lDisplacementArray;
	for (size_t x = 0; x < mSquareInstanceCount; ++x)
	{
		quat lRotation;
		lRotation.RotateAroundOwnY(Random::Uniform(lSeed, 0.0f, PIF*2));
		lRotation.RotateAroundOwnX(Random::Uniform(lSeed, 0.0f, PIF/8));
		lRotation.RotateAroundOwnZ(Random::Uniform(lSeed, 0.0f, PIF/8));
		vec3 lPosition;
		GridToPosition(pX, pY, lPosition);
		lPosition.x += Random::Uniform(lSeed, 0.3f, (float)mSquareSideLength);
		lPosition.y += Random::Uniform(lSeed, 0.3f, (float)mSquareSideLength);
		if (GetObjectPlacement(lPosition))
		{
			std::swap(lPosition.y, lPosition.z);	// TRICKY: transform from RG coords to Maya coords.
			lPosition.z = -lPosition.z;	// TRICKY: transform from RG coords to Maya coords.
			lDisplacementArray.push_back(xform(lRotation, lPosition));
		}
	}
	mSquareArray[pY*SQUARE_SIDE+pX] = new Square(lSeed, mMeshResourceArray, lDisplacementArray, GetUiManager()->GetRenderer());
}

bool MassObject::GetObjectPlacement(vec3& pPosition) const
{
	const float lRayLength = 500;
	pPosition.z += lRayLength * 0.5f;
	quat lOrientation;
	lOrientation.RotateAroundOwnX(PIF);
	xform lTransform(lOrientation, pPosition);
	vec3 lCollisionPosition[3];
	const int lCollisions = GetManager()->GetGameManager()->GetPhysicsManager()->QueryRayCollisionAgainst(
		lTransform, lRayLength, mTerrainBodyId, lCollisionPosition, 3);
	if (lCollisions >= 1)
	{
		unsigned lIndex = 0;
		if (lCollisions > 1)
		{
			unsigned lSeed = ((int)pPosition.y << 10) + (int)pPosition.x;
			lIndex = Random::GetRandomNumber(lSeed) % lCollisions;
		}
		pPosition.z = lCollisionPosition[lIndex].z;
		return true;
	}
	return false;
}



MassObject::Square::Square(uint32 pSeed, const MeshArray& pResourceArray,
	const std::vector<xform>& pDisplacementArray, UiTbc::Renderer* pRenderer):
	mRenderer(pRenderer),
	mDisableTransparency(false)
{
	if (pDisplacementArray.empty())
	{
		return;	// No mesh batches if not inside terrain.
	}

	quat lRotation;
	lRotation.RotateAroundWorldX(PIF*0.5f);
	for (MeshArray::const_iterator y = pResourceArray.begin(); y != pResourceArray.end(); ++y)
	{
		Tbc::GeometryReference* lMesh = (Tbc::GeometryReference*)(*y)->GetRamData();

		UiTbc::GeometryBatch* lBatch = new UiTbc::GeometryBatch(lMesh);
		lBatch->SetBasicMaterialSettings(lMesh->GetBasicMaterialSettings());
		lBatch->SetTransformation(xform(lRotation, vec3(0, 0, 0)));
		lBatch->SetInstances(&pDisplacementArray[0], lMesh->GetTransformation().GetPosition(),
			pDisplacementArray.size(), pSeed, 0.8f, 1.3f, 0.6f, 2.0f, 0.7f, 1.2f);
		typedef UiTbc::Renderer R;
		R::GeometryID lGeometryId = mRenderer->AddGeometry(lBatch, R::MAT_SINGLE_COLOR_SOLID_PXS, R::FORCE_NO_SHADOWS);
		mMassMeshArray.push_back(MassMeshPair(lBatch, lGeometryId));
	}
}

MassObject::Square::~Square()
{
	for (MassMeshArray::iterator x = mMassMeshArray.begin(); x != mMassMeshArray.end(); ++x)
	{
		mRenderer->RemoveGeometry(x->second);
		delete x->first;
	}
	//mMassMeshArray.clear();
	//mRenderer = 0;
}

void MassObject::Square::SetRender(bool pRender, float pAlpha)
{
	pAlpha = Math::Clamp(pAlpha, 0.0f, 1.0f);
	bool lMassObjectFading;
	v_get(lMassObjectFading, =, UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMASSOBJECTFADING, true);
	if (!lMassObjectFading)
	{
		pAlpha = 1;
	}
	for (MassMeshArray::iterator x = mMassMeshArray.begin(); x != mMassMeshArray.end(); ++x)
	{
		x->first->SetAlwaysVisible(pRender);
		x->first->GetBasicMaterialSettings().mAlpha = pAlpha;
		if (pAlpha <= 0.001f)
		{
			x->first->SetAlwaysVisible(false);
		}
		else if (pAlpha >= 0.99f)
		{
			mRenderer->ChangeMaterial(x->second, UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID_PXS);
		}
		else
		{
			mRenderer->ChangeMaterial(x->second, UiTbc::Renderer::MAT_SINGLE_COLOR_OUTLINE_BLENDED);
		}
	}
}



loginstance(GAME_CONTEXT_CPP, MassObject);



}
