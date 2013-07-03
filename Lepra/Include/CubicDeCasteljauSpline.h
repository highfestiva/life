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

#include "Lepra.h"
#include "Math.h"



namespace Lepra
{



template<class T, class TimeType, class TBase = typename T::BaseType>
class CubicDeCasteljauSpline
{
public:
	enum SplineType
	{
		TYPE_BEZIER	= 0,
		TYPE_CATMULLROM	= 1,
		TYPE_BSPLINE	= 2,
	};

	CubicDeCasteljauSpline(T* pKeyFrames,		// pCount + 1 elements.
			      TimeType* pTimeTags,	// pCount + 1 elements.
			      int pCount,
			      SplineType pSplineType = TYPE_BSPLINE,
			      DataPolicy pPolicy = FULL_COPY);
	CubicDeCasteljauSpline(const CubicDeCasteljauSpline& pOriginal, DataPolicy pPolicy = COPY_REFERENCE);
	~CubicDeCasteljauSpline();
	void Set(T* pKeyFrames,		// pCount + 1 elements.
		TimeType* pTimeTags,	// pCount + 1 elements.
		int pCount,
		SplineType pSplineType = TYPE_BSPLINE,
		DataPolicy pPolicy = FULL_COPY);

	void EnableModulo(bool pEnable);
	void StartInterpolation(TimeType pTime);
	void GotoAbsoluteTime(TimeType pTime);
	void StepInterpolation(TimeType pTimeStep);
	TimeType GetCurrentInterpolationTime() const;
	TimeType FindNearestTime(TimeType pStepLength, const T& pWhere, TBase& pNearestDistance, T& pNearestPoint, int pSteps);

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

	int FindFrameAtTime(TimeType pTime, int pStartFrame) const;

	TimeType GetMinimumTime() const;
	TimeType GetMaximumTime() const;

	TimeType GetCurrentSegmentTime() const;

	T Lerp(const T& a, const T& b, TimeType t) const;

	T* mKeyFrames;
	T* mTempBuffer;
	TimeType* mTimeTags;
	int mCount;
	SplineType mSplineType;
	DataPolicy mPolicy;

	TimeType mCurrentTime;
	int mCurrentFrame[4];

	bool mEnableModulo;
};



#include "CubicDeCasteljauSpline.inl"



}
