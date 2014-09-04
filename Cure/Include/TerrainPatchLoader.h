
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Include/Cure.h"
#include "../Include/TerrainManager.h"



namespace Tbc
{
class TerrainPatch;
}



namespace Cure
{



class TerrainPatchLoader
{
public:
	typedef TerrainManager::PatchArea PatchArea;

	TerrainPatchLoader(ResourceManager* pResourceManager);
	virtual ~TerrainPatchLoader();

	Tbc::TerrainPatch* LoadPatch(const PatchArea& pArea, float pLod);
	static void DeletePatch(Tbc::TerrainPatch* pPatch);

private:
	Tbc::TerrainPatch* LoadPatchMesh(const PatchArea& pArea, float pLod);
	Tbc::TerrainPatch* GeneratePatchMesh(const PatchArea& pArea, float pLod);

	ResourceManager* mResourceManager;
};



}
