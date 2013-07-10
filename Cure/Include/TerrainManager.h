
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Lepra/Include/AABR.h"
#include "../../Lepra/Include/IdManager.h"
#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/Vector3D.h"
#include "Cure.h"
#include "ResourceManager.h"



namespace TBC
{
class TerrainPatch;
}



namespace Cure
{



class TerrainFunctionManager;
class TerrainPatchManager;



// Supervisor terrain manager. It, and its sidekicks patch and function manager,
// are built around some general principles:
// 1. Terrain manager handles the interface (including thread safety).
// 2. The patch manager handles the actual patches. The patch instances passed
//    around are resources from the resource system. (In this way we get a better
//    global resource cache than if we'd keep one locally in the patch manager.)
// 3. The function manager owns its resources, rather than using those from the
//    resource manager. The reason for this is that functions are loaded directly
//    when required by a patch, since it in turn is loaded by the resource system.
//    Therefore, latency when loading a function is not a problem. We do not need
//    a resource cache of terrain functions either, because the patches in the
//    managers cache provide already generated terrain data.
class TerrainManager
{
public:
	typedef int CameraId;
	typedef AABR<float> PatchArea;

	TerrainManager(ResourceManager* pResourceManager);
	virtual ~TerrainManager();
	void Clear();

	static TBC::TerrainPatch* CreatePatch(const PatchArea& pArea, float pLod);
	static void DeletePatch(TBC::TerrainPatch* pPatch);

	CameraId AddCamera(const Vector3DF& pPosition, float pVisibleRadius);
	void RemoveCamera(CameraId pCameraId);
	void MoveCamera(CameraId pCameraId, const Vector3DF& pPosition);

	void GetLoadCount(int& pLoadPatchOkCount, int& pLoadPatchErrorCount);

private:
	void UpdatePatchTree();
	void TerrainPatchLoadCallback(UserPhysicalTerrainResource* pLoadedResource);

	Lock* mTerrainLock;

	ResourceManager* mResourceManager;

	int mLoadPatchOkCount;
	int mLoadPatchErrorCount;

	TerrainPatchManager* mPatchManager;

	typedef IdManager<CameraId> CameraIdManager;
	CameraIdManager mCameraIdManager;

	struct CameraInfo
	{
		inline CameraInfo(const Vector3DF& pPosition, float pVisibleRadius):
			mPosition(pPosition),
			mVisibleRadius(pVisibleRadius)
		{
		}
		Vector3DF mPosition;
		float mVisibleRadius;
	};
	typedef HashTable<CameraId, CameraInfo> CameraTable;
	CameraTable mCameraTable;

	LOG_CLASS_DECLARE();
};



}
