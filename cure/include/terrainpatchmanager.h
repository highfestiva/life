
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/aabr.h"
#include "../../lepra/include/vector3d.h"
#include "../../lepra/include/loosequadtree.h"
#include "../include/cure.h"
#include "../include/resourcemanager.h"



namespace tbc {
class TerrainFunction;
class TerrainPatch;
}



namespace cure {



// Data holder for the actual terrain patches. Cache handling must be avoided
// by this class, since that would be redundant (already better handled by the
// resource system).
class TerrainPatchManager {
public:
	TerrainPatchManager();
	virtual ~TerrainPatchManager();
	void Clear();

	void AddPatch(UserPhysicalTerrainResource* patch_resource);

	void MarkAllDirty();
	void MarkInUse(const vec3& position, float radius);
	void DeleteDirty();

private:
	typedef int PatchId;
	struct PatchDirtyInfo {
		inline PatchDirtyInfo(UserPhysicalTerrainResource* resource_patch, bool is_dirty):
			resource_patch_(resource_patch),
			is_dirty_(is_dirty) {
		}
		UserPhysicalTerrainResource* resource_patch_;
		bool is_dirty_;
	};

	void Delete(bool dirty_only);

	typedef HashTable<PatchId, PatchDirtyInfo> PatchTable;
	PatchTable* patch_table_;
};



}
