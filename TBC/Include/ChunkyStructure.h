
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#ifndef CHUNKYSTRUCTURE_H
#define CHUNKYSTRUCTURE_H



#include "TBC.h"
#include "../Include/Bones.h"



namespace TBC
{



class ChunkyBoneGeometry;



class ChunkyStructure: public BoneHierarchy
{
public:
	enum PhysicsType
	{
		UNINITIALIZED = 1,
		STATIC,			// Static world object.
		DYNAMIC,		// Dynamic object within the world.
		COLLISION_DETECT_ONLY,	// Only collision detection. (Used when ray-testing NPCs/avatars.) Change type to dynamic for ragdoll (when falling or dead).
	};

	ChunkyStructure();
	virtual ~ChunkyStructure();

	void SetPhysicsType(PhysicsType pPhysicsType);	// You may use this to change into a ragdoll (COLLISION_DETECT_ONLY -> DYNAMIC), or back.
	PhysicsType GetPhysicsType() const;
	// Takes ownership of the given geometry.
	void SetBoneGeometry(int pBoneIndex, ChunkyBoneGeometry* pGeometry);
	void ClearBoneGeometries();

	// Overrides.
	void ClearAll();
	void SetBoneCount(int pBoneCount);
	void FinalizeInit();

protected:
private:
	ChunkyBoneGeometry** mGeometryArray;
	PhysicsType mPhysicsType;
};



}



#endif // CHUNKYSTRUCTURE_H
