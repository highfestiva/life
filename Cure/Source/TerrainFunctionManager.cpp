
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

// TODO: implement a physical terrain.



#include "../../TBC/Include/TerrainFunction.h"
#include "../Include/TerrainFunctionManager.h"
#include "../Include/TerrainManager.h"



namespace Cure
{



TerrainFunctionManager::TerrainFunctionManager():
	mFunctionTree(new FunctionTree(0, (float)msTerrainFunctionSize, msTerrainFunctionSize/1e4f)),
	mDummyTerrainFunction(0)
{
}

TerrainFunctionManager::~TerrainFunctionManager()
{
	Clear();
	delete (mDummyTerrainFunction);
	mDummyTerrainFunction = 0;
	delete (mFunctionTree);
	mFunctionTree = 0;
}

void TerrainFunctionManager::Clear()
{
	Delete(false);
}



void TerrainFunctionManager::LoadFunctions(const FunctionArea& pArea, float)
{
	if (mDummyTerrainFunction)
	{
		delete (mDummyTerrainFunction);
	}
	mDummyTerrainFunction = new TBC::TerrainConeFunction(pArea.GetSize().x/6, pArea.GetPosition(), 0, pArea.GetSize().x/3);
}

TBC::TerrainPatch* TerrainFunctionManager::GeneratePatchMesh(const FunctionArea& pArea, float pLod)
{
	log_atrace("Generating terrain patch.");
	TBC::TerrainPatch* lPatch = TerrainManager::CreatePatch(pArea, pLod);
	mDummyTerrainFunction->AddFunction(*lPatch);
	return (lPatch);
}



void TerrainFunctionManager::MarkAllDirty()
{
#if 0
	FunctionTree::ObjectList lObjectList;
	mFunctionTree->GetObjects(lObjectList, FunctionArea(-msTerrainFunctionSize, -msTerrainFunctionSize, 2*msTerrainFunctionSize, 2*msTerrainFunctionSize));
	for (FunctionTree::ObjectList::Iterator x = lObjectList.First(); x != lObjectList.End(); ++x)
	{
		TBC::TerrainFunction* lFunction = *x;
		// TODO: set some bool that indicates that this instance might have to be deleted in the near future.
		//lFunction->???
	}
#endif
}

void TerrainFunctionManager::MarkInUse(const Lepra::Vector3DF& /*pPosition*/, float /*pRadius*/)
{
#if 0
	FunctionTree::ObjectList lObjectList;
	mFunctionTree->GetObjects(lObjectList, FunctionArea(-msTerrainFunctionSize, -msTerrainFunctionSize, 2*msTerrainFunctionSize, 2*msTerrainFunctionSize));
	for (FunctionTree::ObjectList::Iterator x = lObjectList.First(); x != lObjectList.End(); ++x)
	{
		TBC::TerrainFunction* lFunction = *x;
		// TODO: reset the dirty bool - to show that indicates instance should not be deleted.
		//lFunction->???
	}
#endif
}

void TerrainFunctionManager::DeleteDirty()
{
	Delete(true);
}



void TerrainFunctionManager::Delete(bool /*pDirtyOnly*/)
{
#if 0
	FunctionTree::ObjectList lObjectList;
	mFunctionTree->GetObjects(lObjectList, FunctionArea(-msTerrainFunctionSize, -msTerrainFunctionSize, 2*msTerrainFunctionSize, 2*msTerrainFunctionSize));
	for (FunctionTree::ObjectList::Iterator x = lObjectList.First(); x != lObjectList.End(); ++x)
	{
		TBC::TerrainFunction* lFunction = *x;
		if (!pDirtyOnly /* TODO: || isdirty(lFunction)*/)
		{
			mFunctionTree->RemoveObject(lFunction);
			delete (lFunction);
		}
	}
#endif
}



LOG_CLASS_DEFINE(GENERAL, TerrainFunctionManager);



}
