
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../TBC/Include/TerrainPatch.h"
#include "../Include/TerrainFunctionManager.h"
#include "../Include/TerrainPatchLoader.h"



namespace Cure
{



TerrainPatchLoader::TerrainPatchLoader(ResourceManager* pResourceManager):
	mResourceManager(pResourceManager)
{
}

TerrainPatchLoader::~TerrainPatchLoader()
{
	mResourceManager = 0;
}



TBC::TerrainPatch* TerrainPatchLoader::LoadPatch(const PatchArea& pArea, float pLod)
{
	TBC::TerrainPatch* lPatch = LoadPatchMesh(pArea, pLod);
	if (!lPatch)
	{
		lPatch = GeneratePatchMesh(pArea, pLod);
	}
	return (lPatch);
}

void TerrainPatchLoader::DeletePatch(TBC::TerrainPatch* pPatch)
{
	delete (pPatch);
}



TBC::TerrainPatch* TerrainPatchLoader::LoadPatchMesh(const PatchArea&, float)
{
	// Loading 3D-meshes for terrain not implemented yet. TODO: implement!
	return (0);
}

TBC::TerrainPatch* TerrainPatchLoader::GeneratePatchMesh(const PatchArea& pArea, float pLod)
{
	TerrainFunctionManager* lFunctionManager = mResourceManager->GetTerrainFunctionManager();
	assert(lFunctionManager);
	lFunctionManager->LoadFunctions(pArea, pLod);
	TBC::TerrainPatch* lPatch = lFunctionManager->GeneratePatchMesh(pArea, pLod);
	return (lPatch);
}



/*TerrainPatchLoader::PatchId TerrainPatchLoader::ConvertGridToId(int pX, int pY)
{
	const PatchId lPatchId = (pY<<16)|pX;
	return (lPatchId);
}

TerrainPatchLoader::PatchId TerrainPatchLoader::ConvertGridToPosition(int pX, int pY, float& pPositionX, float& pPositionY)
{
	pPositionX = pX*TBC::TerrainP
	const PatchId lPatchId = (pY<<16)|pX;
	return (lPatchId);
}

void TerrainPatchLoader::ConvertIdToGrid(PatchId pId, int& pX, int& pY)
{
	pX = (pId&0xFFFF);
	pY = (pId>>16);
}*/



}
