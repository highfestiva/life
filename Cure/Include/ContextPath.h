
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

#pragma once
#include "CppContextObject.h"
#include "../../Lepra/Include/CubicDeCasteljauSpline.h"
#include <vector>



namespace Cure
{



class ContextPath: public CppContextObject
{
public:
	typedef CubicDeCasteljauSpline<Vector3DF, float> SplineShape;
	class SplinePath: public SplineShape
	{
		typedef SplineShape Parent;
	public:
		SplinePath(Vector3DF* pKeyFrames,	// pCount + 1 elements.
			float* pTimeTags,		// pCount + 1 elements.
			int pCount,
			float pDistanceNormal);
		float GetDistanceNormal() const;
	private:
		float mDistanceNormal;
	};

	ContextPath(ResourceManager* pResourceManager, const str& pClassId);
	virtual ~ContextPath();

	virtual void SetTagIndex(int pIndex);
	int GetPathCount() const;
	SplinePath* GetPath(int pIndex) const;

private:
	typedef std::vector<SplinePath*> PathArray;

	PathArray mPathArray;
};



}
