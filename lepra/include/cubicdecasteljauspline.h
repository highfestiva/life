/*
	Class:  CubicDeCasteljauSpline
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	The de Casteljau algorithm is a general spline algorithm with which
	you can create a range of popular splines: Bezier, Catmull-Rom and
	B-splines.

	All that is required of the template argument is that it can be
	linearily interpolated using Math::Lerp().

	Due to the nature of the algorithm, no values can be sampled from the
	extreme endpoints of the spline. To avoid this problem the array of
	keyframes will be treated as a cyclic array, and the spline will be
	looped.

	This requires one extra time tag - the time when the spline is back
	to the start position. The spline is constructed from 4 points, 2 before
	and 2 after the current	time.

	TODO:

	Check wether it works with uneven time gaps.
*/



#pragma once

#include "lepra.h"
#include "math.h"



namespace lepra {



template<class T, class TimeType, class TBase = typename T::BaseType>
class CubicDeCasteljauSpline {
public:
	enum SplineType {
		kTypeBezier	= 0,
		kTypeCatmullrom	= 1,
		kTypeBspline	= 2,
	};

	CubicDeCasteljauSpline(T* key_frames,		// pCount + 1 elements.
			      TimeType* time_tags,	// count + 1 elements.
			      int count,
			      SplineType spline_type = kTypeBspline,
			      DataPolicy policy = kFullCopy);
	CubicDeCasteljauSpline(const CubicDeCasteljauSpline& original, DataPolicy policy = kCopyReference);
	~CubicDeCasteljauSpline();
	void Set(T* key_frames,		// pCount + 1 elements.
		TimeType* time_tags,	// count + 1 elements.
		int count,
		SplineType spline_type = kTypeBspline,
		DataPolicy policy = kFullCopy);

	void EnableModulo(bool enable);
	void StartInterpolation(TimeType time);
	void GotoAbsoluteTime(TimeType time);
	void StepInterpolation(TimeType time_step);
	TimeType GetCurrentInterpolationTime() const;
	TimeType FindNearestTime(TimeType step_length, const T& where, TBase& nearest_distance, T& nearest_point, int steps);

	// Choose one...
	T GetValue() const;
	T GetSlope();

private:
	T GetBezierValue() const;
	T GetCatmullRomValue() const;
	T GetBSplineValue() const;

	T GetValue(TimeType t12, TimeType t23, TimeType t34,
		   TimeType t123, TimeType t234,
		   TimeType t) const;

	int FindFrameAtTime(TimeType time, int start_frame) const;

	TimeType GetMinimumTime() const;
	TimeType GetMaximumTime() const;

	TimeType GetCurrentSegmentTime() const;

	T Lerp(const T& a, const T& b, TimeType t) const;

	T* key_frames_;
	T* temp_buffer_;
	TimeType* time_tags_;
	int count_;
	SplineType spline_type_;
	DataPolicy policy_;

	TimeType current_time_;
	int current_frame_[4];

	bool enable_modulo_;
};



#include "cubicdecasteljauspline.inl"



}
