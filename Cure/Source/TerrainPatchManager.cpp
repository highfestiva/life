
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// TODO: implement a physical terrain.



#include "pch.h"
#include "../../tbc/include/terrainpatch.h"
#include "../include/terrainpatchmanager.h"
#include "../include/terrainmanager.h"



namespace cure {



TerrainPatchManager::TerrainPatchManager():
	patch_table_(new PatchTable()) {
}

TerrainPatchManager::~TerrainPatchManager() {
	Clear();
	delete (patch_table_);
	patch_table_ = 0;
}

void TerrainPatchManager::Clear() {
	Delete(false);
}



void TerrainPatchManager::AddPatch(UserPhysicalTerrainResource* resource_patch) {
	tbc::TerrainPatch* patch = resource_patch->GetData();
	// TODO: calculate some ID from position+size (since both are required for a unique ID)!
	const int x = (int)patch->GetSouthWest().x;
	const int y = (int)patch->GetSouthWest().y;
	PatchId patch_id = patch->GetPatchSizeMultiplier()*0x100000 + y*0x1000 + x;
	patch_table_->Insert(patch_id, PatchDirtyInfo(resource_patch, false));
}



void TerrainPatchManager::MarkAllDirty() {
	for (PatchTable::Iterator x = patch_table_->First(); x != patch_table_->End(); ++x) {
		PatchDirtyInfo& resource_patch_info = x.GetObject();
		resource_patch_info.is_dirty_ = true;
	}
}

void TerrainPatchManager::MarkInUse(const vec3& position, float radius) {
	const float radius_square = radius*radius;
	const vec2 camera_position(position.x, position.y);	// Assume on ground for now.
	for (PatchTable::Iterator x = patch_table_->First(); x != patch_table_->End(); ++x) {
		PatchDirtyInfo& resource_patch_info = x.GetObject();
		tbc::TerrainPatch* patch = resource_patch_info.resource_patch_->GetData();
		const vec2 patch_position((patch->GetSouthWest()+patch->GetNorthEast())*0.5f);
		if (patch_position.GetDistanceSquared(camera_position) < radius_square) {
			resource_patch_info.is_dirty_ = false;
		}
	}
}

void TerrainPatchManager::DeleteDirty() {
	Delete(true);
}



void TerrainPatchManager::Delete(bool dirty_only) {
	for (PatchTable::Iterator x = patch_table_->First(); x != patch_table_->End();) {
		PatchDirtyInfo& resource_patch_info = x.GetObject();
		if (!dirty_only || resource_patch_info.is_dirty_) {
			UserPhysicalTerrainResource* _resource_patch = resource_patch_info.resource_patch_;
			patch_table_->Remove(x++);
			delete (_resource_patch);
			resource_patch_info.resource_patch_ = 0;
		} else {
			++x;
		}
	}
}



}
