
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/aabr.h"
#include "../../lepra/include/vector3d.h"
#include "../../lepra/include/loosequadtree.h"
#include "../include/cure.h"



namespace tbc {
class TerrainFunction;
class TerrainPatch;
}



namespace cure {



// This class holds the terrain functions that are visible (within
// any given camera range of any class aggregating us). Caching unused
// terrain functions must be avoided by this class, since the generated
// patches are handled by the resource system, which implicitly makes
// such a cache redundant.
class TerrainFunctionManager {
public:
	typedef AABR<float> FunctionArea;

	TerrainFunctionManager();
	virtual ~TerrainFunctionManager();
	void Clear();

	void LoadFunctions(const FunctionArea& area, float lod);
	tbc::TerrainPatch* GeneratePatchMesh(const FunctionArea& area, float lod);

	void MarkAllDirty();
	void MarkInUse(const vec3& position, float radius);
	void DeleteDirty();

private:
	void Delete(bool dirty_only);

	// TODO: move to some game logic file!
	static const int terrain_function_size_ = (int)1e6;

	typedef LooseQuadtree<tbc::TerrainFunction*, tbc::TerrainFunction*, float> FunctionTree;
	FunctionTree* function_tree_;

	// TODO: remove the dummy!
	tbc::TerrainFunction* dummy_terrain_function_;

	logclass();
};



}
