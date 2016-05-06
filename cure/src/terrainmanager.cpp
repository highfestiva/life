
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../lepra/include/log.h"
#include "../../tbc/include/terrainpatch.h"
#include "../include/terrainfunctionmanager.h"
#include "../include/terrainmanager.h"
#include "../include/terrainpatchmanager.h"



namespace cure {



TerrainManager::TerrainManager(ResourceManager* resource_manager):
	resource_manager_(resource_manager),
	load_patch_ok_count_(0),
	load_patch_error_count_(0),
	patch_manager_(new TerrainPatchManager()),
	camera_id_manager_(1, 100000, 0) {
	// TODO: we need some logic that handles the quaderatic patch size increase of
	//       the terrain patches relative to the camera.

	//       Note that we don't need any threashold for when to unload, since the
	//       resource system handles that better (resource cache size determined by
	//       priority, timeout and available RAM left).
}

TerrainManager::~TerrainManager() {
	Clear();
	delete (patch_manager_);
	patch_manager_ = 0;
	resource_manager_ = 0;
}

void TerrainManager::Clear() {
	patch_manager_->Clear();
	resource_manager_->GetTerrainFunctionManager()->Clear();
}



tbc::TerrainPatch* TerrainManager::CreatePatch(const PatchArea&, float) {
	// TODO: fill in the construction parameters correct.
	const Vector2D<int> integer_area(3, 4);
	tbc::TerrainPatch* _patch = new tbc::TerrainPatch(integer_area);
	return (_patch);
}

void TerrainManager::DeletePatch(tbc::TerrainPatch* patch) {
	delete (patch);
}



TerrainManager::CameraId TerrainManager::AddCamera(const vec3& position, float visible_radius) {
	CameraId _camera_id = camera_id_manager_.GetFreeId();
	camera_table_.Insert(_camera_id, CameraInfo(position, visible_radius));
	UpdatePatchTree();
	return (_camera_id);
}

void TerrainManager::RemoveCamera(CameraId camera_id) {
	camera_table_.Remove(camera_id);
	camera_id_manager_.RecycleId(camera_id);
	UpdatePatchTree();
}

void TerrainManager::MoveCamera(CameraId camera_id, const vec3& position) {
	CameraTable::Iterator x = camera_table_.Find(camera_id);
	if (x != camera_table_.End()) {
		x.GetObject().position_.Set(position);
		// TODO:
		// if (this move caused camera to cross terrain patch boundary)
		{
			UpdatePatchTree();
		}
	}
}



void TerrainManager::GetLoadCount(int& load_patch_ok_count, int& load_patch_error_count) {
	load_patch_ok_count = load_patch_ok_count_;
	load_patch_error_count = load_patch_error_count_;
}


void TerrainManager::UpdatePatchTree() {
	// Algorithm for loading/deleting terrain patches and terrain functions.
	// 1. Mark all terrain patches and terrain functions as "not used". TODO: implement!
	// 2.1. Mark those patches and functions near cameras as "in use". TODO: implement!
	// 2.2. Find patches that are not yet loaded, but should be loaded. TODO: implement!
	// 3. Remove the patches and functions that are still marked as "not used".  TODO: implement!

	ScopeLock lock(&terrain_lock_);

	// Assume all patches+functions are "not used".
	patch_manager_->MarkAllDirty();
	resource_manager_->GetTerrainFunctionManager()->MarkAllDirty();

	// Mark patches near any camera as used. Load required patches.
	CameraTable::Iterator x = camera_table_.First();
	for (; x != camera_table_.End(); ++x) {
		const CameraInfo& camera_info = x.GetObject();
		const vec3& camera_position = camera_info.position_;
		const float load_radius = camera_info.visible_radius_;

		// Walk through all patches+functions and mark those near this camera as "in use".
		patch_manager_->MarkInUse(camera_position, load_radius);
		resource_manager_->GetTerrainFunctionManager()->MarkInUse(camera_position, load_radius);

		// TODO (2.2.): also find missing patches, and load those.
		// Note that UserPhysicalTerrainResource::Load() will fetch the terrain patch out of the resource
		// cache if it is there to be had.

		// TODO: remove dummy code.
		log_trace("Loading terrain patch.");
		UserPhysicalTerrainResource* dummy_terrain_resource = new UserPhysicalTerrainResource();
		dummy_terrain_resource->Load(resource_manager_, "DummyTerainPatch",
			UserPhysicalTerrainResource::TypeLoadCallback(this, &TerrainManager::TerrainPatchLoadCallback));
	}

	// Walk through all patches+functions and delete those marked as "not used".
	patch_manager_->DeleteDirty();
	resource_manager_->GetTerrainFunctionManager()->DeleteDirty();
}



void TerrainManager::TerrainPatchLoadCallback(UserPhysicalTerrainResource* loaded_resource) {
	if (loaded_resource->GetConstResource()->GetLoadState() == cure::kResourceLoadComplete) {
		log_debug("Terrain patch asynchronously loaded.");
		ScopeLock lock(&terrain_lock_);
		// TODO: insert patch into tree.
		patch_manager_->AddPatch(loaded_resource);
		++load_patch_ok_count_;
	} else {
		log_.Error("Terrain patch could not be loaded!");
		// TODO: dunno. Terminate? Send error report?
		delete (loaded_resource);
		++load_patch_error_count_;
	}
}



loginstance(kGame, TerrainManager);



}
