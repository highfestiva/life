
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Lepra/Include/AABR.h"
#include "../../Lepra/Include/Vector3D.h"
#include "../../Lepra/Include/LooseQuadtree.h"
#include "../Include/Cure.h"
#include "../Include/ResourceManager.h"



namespace Tbc
{
class TerrainFunction;
class TerrainPatch;
}



namespace Cure
{



// Data holder for the actual terrain patches. Cache handling must be avoided
// by this class, since that would be redundant (already better handled by the
// resource system).
class TerrainPatchManager
{
public:
	TerrainPatchManager();
	virtual ~TerrainPatchManager();
	void Clear();

	void AddPatch(UserPhysicalTerrainResource* pPatchResource);

	void MarkAllDirty();
	void MarkInUse(const vec3& pPosition, float pRadius);
	void DeleteDirty();

private:
	typedef int PatchId;
	struct PatchDirtyInfo
	{
		inline PatchDirtyInfo(UserPhysicalTerrainResource* pResourcePatch, bool pIsDirty):
			mResourcePatch(pResourcePatch),
			mIsDirty(pIsDirty)
		{
		}
		UserPhysicalTerrainResource* mResourcePatch;
		bool mIsDirty;
	};

	void Delete(bool pDirtyOnly);

	typedef HashTable<PatchId, PatchDirtyInfo> PatchTable;
	PatchTable* mPatchTable;
};



}
