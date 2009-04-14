
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "../Include/ChunkyBoneGeometry.h"
#include "../Include/ChunkyStructure.h"



namespace TBC
{



ChunkyStructure::ChunkyStructure():
	BoneHierarchy(),
	mGeometryArray(0),
	mPhysicsType(UNINITIALIZED)
{
}

ChunkyStructure::~ChunkyStructure()
{
	ClearBoneGeometries();
}



void ChunkyStructure::SetPhysicsType(PhysicsType pPhysicsType)
{
	// TODO: implement change in physics engine.
	mPhysicsType = pPhysicsType;
}

ChunkyStructure::PhysicsType ChunkyStructure::GetPhysicsType() const
{
	return (mPhysicsType);
}

void ChunkyStructure::SetBoneGeometry(int pBoneIndex, ChunkyBoneGeometry* pGeometry)
{
	if (mGeometryArray[pBoneIndex])
	{
		delete (mGeometryArray[pBoneIndex]);
	}
	mGeometryArray[pBoneIndex] = pGeometry;
}

void ChunkyStructure::ClearBoneGeometries()
{
	if (mGeometryArray)
	{
		for (int x = 0; x < GetBoneCount(); ++x)
		{
			delete (mGeometryArray[x]);
			mGeometryArray[x] = 0;
		}
		delete[] (mGeometryArray);
		mGeometryArray = 0;
	}
}



void ChunkyStructure::ClearAll()
{
	ClearBoneGeometries();
	BoneHierarchy::ClearAll();
}

void ChunkyStructure::SetBoneCount(int pBoneCount)
{
	BoneHierarchy::SetBoneCount(pBoneCount);

	mGeometryArray = new ChunkyBoneGeometry*[GetBoneCount()];
	::memset(mGeometryArray, 0, sizeof(ChunkyBoneGeometry*)*GetBoneCount());
}

void ChunkyStructure::FinalizeInit()
{
	// TODO: add to physics engine depending on mPhysicsType.
	BoneHierarchy::FinalizeInit();
}



}
