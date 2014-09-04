
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#pragma once
#include "CppContextObject.h"
#include "../../Lepra/Include/CubicDeCasteljauSpline.h"
#include <vector>



namespace Cure
{



class ContextPath: public CppContextObject
{
public:
	typedef CubicDeCasteljauSpline<vec3, float> SplineShape;
	class SplinePath: public SplineShape
	{
		typedef SplineShape Parent;
	public:
		SplinePath(vec3* pKeyFrames,	// pCount + 1 elements.
			float* pTimeTags,		// pCount + 1 elements.
			int pCount,
			str pType,
			float pDistanceNormal,
			float pLikeliness);
		SplinePath(const SplinePath& pOriginal);
		const str& GetType() const;
		float GetDistanceNormal() const;
		float GetLikeliness() const;
		float GetTimeAsDistance(float pTime) const;
		float GetDistanceLeft() const;
	private:
		str mType;	// Stuff indicating stuff like "is final path", "towards some type of goal", etc.
		float mDistanceNormal;	// 1/length. The times span from 0 to 1, divide by DistanceNormal to get total length.
		float mLikeliness;
	};

	ContextPath(ResourceManager* pResourceManager, const str& pClassId);
	virtual ~ContextPath();

	virtual void SetTagIndex(int pIndex);
	int GetPathCount() const;
	SplinePath* GetPath(int pIndex) const;
	SplinePath* GetPath(const str& pType) const;

private:
	typedef std::vector<SplinePath*> PathArray;

	PathArray mPathArray;
};



}
