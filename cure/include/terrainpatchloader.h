
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../include/cure.h"
#include "../include/terrainmanager.h"



namespace tbc {
class TerrainPatch;
}



namespace cure {



class TerrainPatchLoader {
public:
	typedef TerrainManager::PatchArea PatchArea;

	TerrainPatchLoader(ResourceManager* resource_manager);
	virtual ~TerrainPatchLoader();

	tbc::TerrainPatch* LoadPatch(const PatchArea& area, float lod);
	static void DeletePatch(tbc::TerrainPatch* patch);

private:
	tbc::TerrainPatch* LoadPatchMesh(const PatchArea& area, float lod);
	tbc::TerrainPatch* GeneratePatchMesh(const PatchArea& area, float lod);

	ResourceManager* resource_manager_;
};



}
