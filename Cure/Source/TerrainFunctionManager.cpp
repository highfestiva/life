
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// TODO: implement a physical terrain.



#include "pch.h"
#include "../../tbc/include/terrainfunction.h"
#include "../include/terrainfunctionmanager.h"
#include "../include/terrainmanager.h"



namespace cure {



TerrainFunctionManager::TerrainFunctionManager():
	function_tree_(new FunctionTree(0, (float)terrain_function_size_, terrain_function_size_/1e4f)),
	dummy_terrain_function_(0) {
}

TerrainFunctionManager::~TerrainFunctionManager() {
	Clear();
	delete (dummy_terrain_function_);
	dummy_terrain_function_ = 0;
	delete (function_tree_);
	function_tree_ = 0;
}

void TerrainFunctionManager::Clear() {
	Delete(false);
}



void TerrainFunctionManager::LoadFunctions(const FunctionArea& area, float) {
	if (dummy_terrain_function_) {
		delete (dummy_terrain_function_);
	}
	dummy_terrain_function_ = new tbc::TerrainConeFunction(area.GetSize().x/6, area.GetPosition(), 0, area.GetSize().x/3);
}

tbc::TerrainPatch* TerrainFunctionManager::GeneratePatchMesh(const FunctionArea& area, float lod) {
	log_trace("Generating terrain patch.");
	tbc::TerrainPatch* patch = TerrainManager::CreatePatch(area, lod);
	dummy_terrain_function_->AddFunction(*patch);
	return (patch);
}



void TerrainFunctionManager::MarkAllDirty() {
#if 0
	FunctionTree::ObjectList object_list;
	function_tree_->GetObjects(object_list, FunctionArea(-terrain_function_size_, -terrain_function_size_, 2*terrain_function_size_, 2*terrain_function_size_));
	for (FunctionTree::ObjectList::Iterator x = object_list.First(); x != object_list.End(); ++x) {
		tbc::TerrainFunction* function = *x;
		// TODO: set some bool that indicates that this instance might have to be deleted in the near future.
		//function->???
	}
#endif
}

void TerrainFunctionManager::MarkInUse(const vec3& /*position*/, float /*radius*/) {
#if 0
	FunctionTree::ObjectList object_list;
	function_tree_->GetObjects(object_list, FunctionArea(-terrain_function_size_, -terrain_function_size_, 2*terrain_function_size_, 2*terrain_function_size_));
	for (FunctionTree::ObjectList::Iterator x = object_list.First(); x != object_list.End(); ++x) {
		tbc::TerrainFunction* function = *x;
		// TODO: reset the dirty bool - to show that indicates instance should not be deleted.
		//function->???
	}
#endif
}

void TerrainFunctionManager::DeleteDirty() {
	Delete(true);
}



void TerrainFunctionManager::Delete(bool /*dirty_only*/) {
#if 0
	FunctionTree::ObjectList object_list;
	function_tree_->GetObjects(object_list, FunctionArea(-terrain_function_size_, -terrain_function_size_, 2*terrain_function_size_, 2*terrain_function_size_));
	for (FunctionTree::ObjectList::Iterator x = object_list.First(); x != object_list.End(); ++x) {
		tbc::TerrainFunction* function = *x;
		if (!dirty_only /* TODO: || isdirty(function)*/) {
			function_tree_->RemoveObject(function);
			delete (function);
		}
	}
#endif
}



loginstance(kGeneral, TerrainFunctionManager);



}
