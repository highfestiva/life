
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../Lepra/Include/Log.h"
#include "../../TBC/Include/TerrainPatch.h"
#include "../Include/TerrainFunctionManager.h"
#include "../Include/TerrainManager.h"
#include "../Include/TerrainPatchManager.h"



namespace Cure
{



TerrainManager::TerrainManager(ResourceManager* pResourceManager):
	mTerrainLock(new Lock()),
	mResourceManager(pResourceManager),
	mLoadPatchOkCount(0),
	mLoadPatchErrorCount(0),
	mPatchManager(new TerrainPatchManager()),
	mCameraIdManager(1, 100000, 0)
{
	// TODO: we need some logic that handles the quaderatic patch size increase of
	//       the terrain patches relative to the camera.

	//       Note that we don't need any threashold for when to unload, since the
	//       resource system handles that better (resource cache size determined by
	//       priority, timeout and available RAM left).
}

TerrainManager::~TerrainManager()
{
	Clear();
	delete (mPatchManager);
	mPatchManager = 0;
	mResourceManager = 0;
	delete (mTerrainLock);
	mTerrainLock = 0;
}

void TerrainManager::Clear()
{
	mPatchManager->Clear();
	mResourceManager->GetTerrainFunctionManager()->Clear();
}



TBC::TerrainPatch* TerrainManager::CreatePatch(const PatchArea&, float)
{
	// TODO: fill in the construction parameters correct.
	const Vector2D<int> lIntegerArea(3, 4);
	TBC::TerrainPatch* lPatch = new TBC::TerrainPatch(lIntegerArea);
	return (lPatch);
}

void TerrainManager::DeletePatch(TBC::TerrainPatch* pPatch)
{
	delete (pPatch);
}



TerrainManager::CameraId TerrainManager::AddCamera(const Vector3DF& pPosition, float pVisibleRadius)
{
	CameraId lCameraId = mCameraIdManager.GetFreeId();
	mCameraTable.Insert(lCameraId, CameraInfo(pPosition, pVisibleRadius));
	UpdatePatchTree();
	return (lCameraId);
}

void TerrainManager::RemoveCamera(CameraId pCameraId)
{
	mCameraTable.Remove(pCameraId);
	mCameraIdManager.RecycleId(pCameraId);
	UpdatePatchTree();
}

void TerrainManager::MoveCamera(CameraId pCameraId, const Vector3DF& pPosition)
{
	CameraTable::Iterator x = mCameraTable.Find(pCameraId);
	if (x != mCameraTable.End())
	{
		x.GetObject().mPosition.Set(pPosition);
		// TODO:
		// if (this move caused camera to cross terrain patch boundary)
		{
			UpdatePatchTree();
		}
	}
}



void TerrainManager::GetLoadCount(int& pLoadPatchOkCount, int& pLoadPatchErrorCount)
{
	pLoadPatchOkCount = mLoadPatchOkCount;
	pLoadPatchErrorCount = mLoadPatchErrorCount;
}


void TerrainManager::UpdatePatchTree()
{
	// Algorithm for loading/deleting terrain patches and terrain functions.
	// 1. Mark all terrain patches and terrain functions as "not used". TODO: implement!
	// 2.1. Mark those patches and functions near cameras as "in use". TODO: implement!
	// 2.2. Find patches that are not yet loaded, but should be loaded. TODO: implement!
	// 3. Remove the patches and functions that are still marked as "not used".  TODO: implement!

	ScopeLock lLock(mTerrainLock);

	// Assume all patches+functions are "not used".
	mPatchManager->MarkAllDirty();
	mResourceManager->GetTerrainFunctionManager()->MarkAllDirty();

	// Mark patches near any camera as used. Load required patches.
	CameraTable::Iterator x = mCameraTable.First();
	for (; x != mCameraTable.End(); ++x)
	{
		const CameraInfo& lCameraInfo = x.GetObject();
		const Vector3DF& lCameraPosition = lCameraInfo.mPosition;
		const float lLoadRadius = lCameraInfo.mVisibleRadius;

		// Walk through all patches+functions and mark those near this camera as "in use".
		mPatchManager->MarkInUse(lCameraPosition, lLoadRadius);
		mResourceManager->GetTerrainFunctionManager()->MarkInUse(lCameraPosition, lLoadRadius);

		// TODO (2.2.): also find missing patches, and load those.
		// Note that UserPhysicalTerrainResource::Load() will fetch the terrain patch out of the resource
		// cache if it is there to be had.

		// TODO: remove dummy code.
		log_atrace("Loading terrain patch.");
		UserPhysicalTerrainResource* pDummyTerrainResource = new UserPhysicalTerrainResource();
		pDummyTerrainResource->Load(mResourceManager, _T("DummyTerainPatch"),
			UserPhysicalTerrainResource::TypeLoadCallback(this, &TerrainManager::TerrainPatchLoadCallback));
	}

	// Walk through all patches+functions and delete those marked as "not used".
	mPatchManager->DeleteDirty();
	mResourceManager->GetTerrainFunctionManager()->DeleteDirty();
}



void TerrainManager::TerrainPatchLoadCallback(UserPhysicalTerrainResource* pLoadedResource)
{
	if (pLoadedResource->GetConstResource()->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		log_adebug("Terrain patch asynchronously loaded.");
		ScopeLock lLock(mTerrainLock);
		// TODO: insert patch into tree.
		mPatchManager->AddPatch(pLoadedResource);
		++mLoadPatchOkCount;
	}
	else
	{
		mLog.AError("Terrain patch could not be loaded!");
		// TODO: dunno. Terminate? Send error report?
		delete (pLoadedResource);
		++mLoadPatchErrorCount;
	}
}



LOG_CLASS_DEFINE(GAME, TerrainManager);



}
