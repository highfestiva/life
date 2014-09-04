
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// TODO: implement a physical terrain.



#include "pch.h"
#include "../../Tbc/Include/TerrainPatch.h"
#include "../Include/TerrainPatchManager.h"
#include "../Include/TerrainManager.h"



namespace Cure
{



TerrainPatchManager::TerrainPatchManager():
	mPatchTable(new PatchTable())
{
}

TerrainPatchManager::~TerrainPatchManager()
{
	Clear();
	delete (mPatchTable);
	mPatchTable = 0;
}

void TerrainPatchManager::Clear()
{
	Delete(false);
}



void TerrainPatchManager::AddPatch(UserPhysicalTerrainResource* pResourcePatch)
{
	Tbc::TerrainPatch* lPatch = pResourcePatch->GetData();
	// TODO: calculate some ID from position+size (since both are required for a unique ID)!
	const int x = (int)lPatch->GetSouthWest().x;
	const int y = (int)lPatch->GetSouthWest().y;
	PatchId lPatchId = lPatch->GetPatchSizeMultiplier()*0x100000 + y*0x1000 + x;
	mPatchTable->Insert(lPatchId, PatchDirtyInfo(pResourcePatch, false));
}



void TerrainPatchManager::MarkAllDirty()
{
	for (PatchTable::Iterator x = mPatchTable->First(); x != mPatchTable->End(); ++x)
	{
		PatchDirtyInfo& lResourcePatchInfo = x.GetObject();
		lResourcePatchInfo.mIsDirty = true;
	}
}

void TerrainPatchManager::MarkInUse(const vec3& pPosition, float pRadius)
{
	const float lRadiusSquare = pRadius*pRadius;
	const vec2 lCameraPosition(pPosition.x, pPosition.y);	// Assume on ground for now.
	for (PatchTable::Iterator x = mPatchTable->First(); x != mPatchTable->End(); ++x)
	{
		PatchDirtyInfo& lResourcePatchInfo = x.GetObject();
		Tbc::TerrainPatch* lPatch = lResourcePatchInfo.mResourcePatch->GetData();
		const vec2 lPatchPosition((lPatch->GetSouthWest()+lPatch->GetNorthEast())*0.5f);
		if (lPatchPosition.GetDistanceSquared(lCameraPosition) < lRadiusSquare)
		{
			lResourcePatchInfo.mIsDirty = false;
		}
	}
}

void TerrainPatchManager::DeleteDirty()
{
	Delete(true);
}



void TerrainPatchManager::Delete(bool pDirtyOnly)
{
	for (PatchTable::Iterator x = mPatchTable->First(); x != mPatchTable->End();)
	{
		PatchDirtyInfo& lResourcePatchInfo = x.GetObject();
		if (!pDirtyOnly || lResourcePatchInfo.mIsDirty)
		{
			UserPhysicalTerrainResource* lResourcePatch = lResourcePatchInfo.mResourcePatch;
			mPatchTable->Remove(x++);
			delete (lResourcePatch);
			lResourcePatchInfo.mResourcePatch = 0;
		}
		else
		{
			++x;
		}
	}
}



}
