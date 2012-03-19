
// Author: Jonas Bystr�m
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
			str pType,
			float pDistanceNormal,
			float pLikeliness);
		const str& GetType() const;
		float GetDistanceNormal() const;
		float GetLikeliness() const;
	private:
		str mType;	// Stuff indicating stuff like "is final path", "towards some type of goal", etc.
		float mDistanceNormal;
		float mLikeliness;
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
