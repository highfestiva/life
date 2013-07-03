
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Lepra/Include/AABR.h"
#include "../../Lepra/Include/Vector3D.h"
#include "../../Lepra/Include/LooseQuadtree.h"
#include "../Include/Cure.h"



namespace TBC
{
class TerrainFunction;
class TerrainPatch;
}



namespace Cure
{



// This class holds the terrain functions that are visible (within
// any given camera range of any class aggregating us). Caching unused
// terrain functions must be avoided by this class, since the generated
// patches are handled by the resource system, which implicitly makes
// such a cache redundant.
class TerrainFunctionManager
{
public:
	typedef AABR<float> FunctionArea;

	TerrainFunctionManager();
	virtual ~TerrainFunctionManager();
	void Clear();

	void LoadFunctions(const FunctionArea& pArea, float pLod);
	TBC::TerrainPatch* GeneratePatchMesh(const FunctionArea& pArea, float pLod);

	void MarkAllDirty();
	void MarkInUse(const Vector3DF& pPosition, float pRadius);
	void DeleteDirty();

private:
	void Delete(bool pDirtyOnly);

	// TODO: move to some game logic file!
	static const int msTerrainFunctionSize = (int)1e6;

	typedef LooseQuadtree<TBC::TerrainFunction*, TBC::TerrainFunction*, float> FunctionTree;
	FunctionTree* mFunctionTree;

	// TODO: remove the dummy!
	TBC::TerrainFunction* mDummyTerrainFunction;

	LOG_CLASS_DECLARE();
};



}
