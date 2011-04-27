
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../Include/Cure.h"
#include "../Include/TerrainManager.h"



namespace TBC
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

	TBC::TerrainPatch* LoadPatch(const PatchArea& pArea, float pLod);
	static void DeletePatch(TBC::TerrainPatch* pPatch);

private:
	TBC::TerrainPatch* LoadPatchMesh(const PatchArea& pArea, float pLod);
	TBC::TerrainPatch* GeneratePatchMesh(const PatchArea& pArea, float pLod);

	ResourceManager* mResourceManager;
};



}
