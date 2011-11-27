
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/ContextPath.h"
#include "../../TBC/Include/ChunkyBoneGeometry.h"



namespace Cure
{



ContextPath::ContextPath(ResourceManager* pResourceManager, const str& pClassId):
	CppContextObject(pResourceManager, pClassId)
{
}

ContextPath::~ContextPath()
{
	PathArray::iterator x = mPathArray.begin();
	for (; x != mPathArray.end(); ++x)
	{
		delete *x;
	}
	mPathArray.clear();
}



void ContextPath::SetTagIndex(int pIndex)
{
	const TBC::ChunkyClass::Tag& lTag = ((CppContextObject*)mParent)->GetClass()->GetTag(pIndex);
	const size_t lBodyCount = lTag.mBodyIndexList.size();
	assert(lBodyCount >= 2);
	if (lBodyCount < 2)
	{
		return;
	}
	// 1. Store positions.
	// 2. Calculate total distance (shortest distance through all vertices).
	// 3. Scale time on each key so 1.0 corresponds total length.
	//
	// TRICKY: this algorithm adds one extra head vertex and two extra tail vertices to the
	// path, to avoid spline looping.
	float lTotalDistance = 1;	// Start at an offset (head).
	TBC::ChunkyPhysics* lPhysics = mParent->GetPhysics();
	Vector3DF* lPathPositions = new Vector3DF[1+lBodyCount+2];
	float* lTimes = new float[1+lBodyCount+2];
	size_t x;
	for (x = 0; x < lBodyCount; ++x)
	{
		const int lBoneIndex = lTag.mBodyIndexList[x];
#ifdef LEPRA_DEBUG
		TBC::ChunkyBoneGeometry* lBone = lPhysics->GetBoneGeometry(lBoneIndex);
		assert(lBone->GetBoneType() == TBC::ChunkyBoneGeometry::BONE_POSITION);
#endif // Debug
		lPathPositions[x+1] = lPhysics->GetBoneTransformation(lBoneIndex).GetPosition();
		if (x > 0)
		{
			lTotalDistance += lPathPositions[x].GetDistance(lPathPositions[x-1]);
			lTimes[x+1] = lTotalDistance;
		}
	}
	lTimes[0] = -1;
	lPathPositions[0] = lPathPositions[1];
	lTimes[1] = 0;
	lTimes[1+lBodyCount] = lTotalDistance+1.0f;
	lTimes[1+lBodyCount+1] = lTotalDistance+2.0f;
	const float lScale = 1 / lTotalDistance;
	for (size_t x = 0; x <= 1+lBodyCount+1; ++x)
	{
		lTimes[x] *= lScale;
	}
	lPathPositions[1+lBodyCount] = lPathPositions[1+lBodyCount-1];
	lPathPositions[1+lBodyCount+1] = lPathPositions[1+lBodyCount-1];
	Spline* lSpline = new Spline(lPathPositions, lTimes, lBodyCount+2, Spline::TYPE_CATMULLROM, TAKE_OWNERSHIP);
	lSpline->StartInterpolation(0);
	mPathArray.push_back(lSpline);
}



}
