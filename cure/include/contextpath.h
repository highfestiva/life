
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#pragma once
#include "cppcontextobject.h"
#include "../../lepra/include/cubicdecasteljauspline.h"
#include <vector>



namespace cure {



class ContextPath: public CppContextObject {
public:
	typedef CubicDeCasteljauSpline<vec3, float> SplineShape;
	class SplinePath: public SplineShape {
		typedef SplineShape Parent;
	public:
		SplinePath(vec3* key_frames,	// pCount + 1 elements.
			float* time_tags,		// count + 1 elements.
			int count,
			str type,
			float distance_normal,
			float likeliness);
		SplinePath(const SplinePath& original);
		const str& GetType() const;
		float GetDistanceNormal() const;
		float GetLikeliness() const;
		float GetTimeAsDistance(float time) const;
		float GetDistanceLeft() const;
	private:
		str type_;	// Stuff indicating stuff like "is final path", "towards some type of goal", etc.
		float distance_normal_;	// 1/length. The times span from 0 to 1, divide by DistanceNormal to get total length.
		float likeliness_;
	};

	ContextPath(ResourceManager* resource_manager, const str& class_id);
	virtual ~ContextPath();

	virtual void SetTagIndex(int index);
	int GetPathCount() const;
	SplinePath* GetPath(int index) const;
	SplinePath* GetPath(const str& type) const;

private:
	typedef std::vector<SplinePath*> PathArray;

	PathArray path_array_;
};



}
