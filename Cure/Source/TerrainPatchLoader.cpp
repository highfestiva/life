
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../tbc/include/terrainpatch.h"
#include "../include/terrainfunctionmanager.h"
#include "../include/terrainpatchloader.h"



namespace cure {



TerrainPatchLoader::TerrainPatchLoader(ResourceManager* resource_manager):
	resource_manager_(resource_manager) {
}

TerrainPatchLoader::~TerrainPatchLoader() {
	resource_manager_ = 0;
}



tbc::TerrainPatch* TerrainPatchLoader::LoadPatch(const PatchArea& area, float lod) {
	tbc::TerrainPatch* _patch = LoadPatchMesh(area, lod);
	if (!_patch) {
		_patch = GeneratePatchMesh(area, lod);
	}
	return (_patch);
}

void TerrainPatchLoader::DeletePatch(tbc::TerrainPatch* patch) {
	delete (patch);
}



tbc::TerrainPatch* TerrainPatchLoader::LoadPatchMesh(const PatchArea&, float) {
	// Loading 3D-meshes for terrain not implemented yet. TODO: implement!
	return (0);
}

tbc::TerrainPatch* TerrainPatchLoader::GeneratePatchMesh(const PatchArea& area, float lod) {
	TerrainFunctionManager* function_manager = resource_manager_->GetTerrainFunctionManager();
	deb_assert(function_manager);
	function_manager->LoadFunctions(area, lod);
	tbc::TerrainPatch* _patch = function_manager->GeneratePatchMesh(area, lod);
	return (_patch);
}



/*TerrainPatchLoader::PatchId TerrainPatchLoader::ConvertGridToId(int x, int y) {
	const PatchId patch_id = (y<<16)|x;
	return (patch_id);
}

TerrainPatchLoader::PatchId TerrainPatchLoader::ConvertGridToPosition(int x, int y, float& pPositionX, float& pPositionY) {
	pPositionX = x*tbc::TerrainP
	const PatchId patch_id = (y<<16)|x;
	return (patch_id);
}

void TerrainPatchLoader::ConvertIdToGrid(PatchId id, int& x, int& y) {
	x = (id&0xFFFF);
	y = (id>>16);
}*/



}
