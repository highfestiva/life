
// Author: Alexander Hugestrand
// Copyright (c) 2002-2008, Righteous Games



template<class T, class TimeType, class TBase>
CubicDeCasteljauSpline<T, TimeType, TBase>::CubicDeCasteljauSpline(T* pKeyFrames,
	TimeType* pTimeTags, int pCount, SplineType pSplineType, DataPolicy pPolicy):
	mKeyFrames(pKeyFrames),
	mTempBuffer(0),
	mTimeTags(pTimeTags),
	mCount(pCount),
	mSplineType(pSplineType),
	mPolicy(pPolicy),
	mCurrentTime(0)
{
	if (mPolicy == FULL_COPY)
	{
		mKeyFrames = new T[mCount];
		mTimeTags = new TimeType[mCount + 1];

		::memcpy(mKeyFrames, pKeyFrames, mCount * sizeof(T));
		::memcpy(mTimeTags, pTimeTags, (mCount + 1) * sizeof(TimeType));
	}

	if (mSplineType == TYPE_BEZIER)
	{
		mTempBuffer = new T[mCount];
	}

	// Check if valid.
	/*for (int i = 1; i <= mCount; i++)
	{
		assert(mTimeTags[i - 1] < mTimeTags[i]);
	}*/
}

template<class T, class TimeType, class TBase>
CubicDeCasteljauSpline<T, TimeType, TBase>::~CubicDeCasteljauSpline()
{
	if (mPolicy != COPY_REFERENCE)
	{
		delete[] mKeyFrames;
		delete[] mTimeTags;
	}

	if (mTempBuffer != 0)
	{
		delete[] mTempBuffer;
	}
}

template<class T, class TimeType, class TBase>
void CubicDeCasteljauSpline<T, TimeType, TBase>::StartInterpolation(TimeType pTime)
{
	pTime = Math::Mod(pTime, mTimeTags[0], mTimeTags[mCount]);

	int lCount = mCount + 1;
	mCurrentFrame[1] = FindFrameAtTime(pTime, 0);
	mCurrentFrame[0] = Math::Mod(mCurrentFrame[1] - 1, lCount);
	mCurrentFrame[2] = Math::Mod(mCurrentFrame[1] + 1, lCount);
	mCurrentFrame[3] = Math::Mod(mCurrentFrame[2] + 1, lCount);

	mCurrentTime = pTime;
}

template<class T, class TimeType, class TBase>
void CubicDeCasteljauSpline<T, TimeType, TBase>::GotoAbsoluteTime(TimeType pTime)
{
	TimeType pRelativeTime = pTime-GetCurrentInterpolationTime();
	StepInterpolation(pRelativeTime);
}

template<class T, class TimeType, class TBase>
void CubicDeCasteljauSpline<T, TimeType, TBase>::StepInterpolation(TimeType pTimeStep)
{
	TimeType lPrevTime = mCurrentTime;
	mCurrentTime += pTimeStep;
	/*if (mCurrentTime < GetMinimumTime())
	{
		mCurrentTime = GetMinimumTime();
	}
	else if (mCurrentTime > GetMaximumTime())
	{
		mCurrentTime = GetMaximumTime();
	}*/
	const TimeType lModulo = Math::Mod(mCurrentTime, mTimeTags[0], mTimeTags[mCount]);
	mCurrentTime = lModulo;

	if (mCurrentTime < lPrevTime && pTimeStep > 0)	// Note: don't change. Checks for "looped" condition (as opposed to "backed").
	{
		// We have moved looped. Search from the start.
		mCurrentFrame[1] = FindFrameAtTime(mCurrentTime, 0);
	}
	else
	{
		mCurrentFrame[1] = FindFrameAtTime(mCurrentTime, mCurrentFrame[1]);
	}

	mCurrentFrame[0] = Math::Mod(mCurrentFrame[1] - 1, mCount);
	mCurrentFrame[2] = Math::Mod(mCurrentFrame[1] + 1, mCount);
	mCurrentFrame[3] = Math::Mod(mCurrentFrame[2] + 1, mCount);
}

template<class T, class TimeType, class TBase>
TimeType CubicDeCasteljauSpline<T, TimeType, TBase>::GetCurrentInterpolationTime() const
{
	return mCurrentTime;
}

template<class T, class TimeType, class TBase>
TimeType CubicDeCasteljauSpline<T, TimeType, TBase>::FindNearestTime(TimeType pStepLength, const T& pWhere, TBase& pNearestDistance, T& pNearestPoint)
{
	// Ugly search algorithm. Looks in the step direction until the distance becomes greater than
	// the starting distance. Then turns around and looks in the other direction at half the velocity.
	// And so forth. This could probably be optimized using Newton-Raphson, or (worst-case) Runge-Cutta.
	TimeType lNearestTime = mCurrentTime;
	T lNearestDistance = GetValue()-pWhere;
	typename T::BaseType lDistance2 = lNearestDistance.GetLengthSquared();
	T lNewNearestDistance;
	for (int y = 0; y < 20; ++y)
	{
		const int zc = 8;
		int z;
		for (z = 0; z < zc; ++z)
		{
			StepInterpolation(pStepLength);
			lNewNearestDistance = GetValue()-pWhere;
			typename T::BaseType lNewDistance2 = lNewNearestDistance.GetLengthSquared();
			if (lNewDistance2 >= lDistance2)
			{
				StepInterpolation(-pStepLength);	// Go back to previous point.
				pStepLength = pStepLength*-0.5f;	// Change search direction; increase granularity.
				if ((mCurrentTime == GetMinimumTime() && pStepLength < 0) ||
					(mCurrentTime == GetMaximumTime() && pStepLength > 0))
				{
					pStepLength = -pStepLength;
				}
			}
			else
			{
				lDistance2 = lNewDistance2;
				lNearestDistance = lNewNearestDistance;
				lNearestTime = mCurrentTime;
				break;
			}
		}
		if (z == zc)	// Nothing new found in a few loops - might as well give up.
		{
			break;
		}
		pStepLength *= 0.7f;	// Increase granularity when we've found a closer spot.
	}

	// Fill in our output values.
	pNearestDistance = ::sqrt(lDistance2);
	pNearestPoint = lNearestDistance+pWhere;
	return (lNearestTime);
}

template<class T, class TimeType, class TBase>
T CubicDeCasteljauSpline<T, TimeType, TBase>::GetValue() const
{
	switch (mSplineType)
	{
		case TYPE_BEZIER:	return (GetBezierValue());	// TRICKY: returns as an optimization.
		case TYPE_CATMULLROM:	return (GetCatmullRomValue());	// TRICKY: returns as an optimization.
		default:		return (GetBSplineValue());	// TRICKY: returns as an optimization.
	}
}

template<class T, class TimeType, class TBase>
T CubicDeCasteljauSpline<T, TimeType, TBase>::GetBezierValue() const
{
	TimeType t = (mCurrentTime - mTimeTags[0]) / (mTimeTags[mCount] - mTimeTags[0]);

	int i;
	int j;

	for (i = 0; i < mCount; i++)
	{
		mTempBuffer[i] = mKeyFrames[i];
	}

	for (j = mCount - 1; j > 0; j--)
	{
		for (i = 0; i < j; i++)
		{
			mTempBuffer[i] = Lerp(mTempBuffer[i], mTempBuffer[i + 1], t);
		}
	}

	return mTempBuffer[0];
}

template<class T, class TimeType, class TBase>
T CubicDeCasteljauSpline<T, TimeType, TBase>::GetCatmullRomValue() const
{
	TimeType t = GetCurrentSegmentTime();
	return GetValue(t + (TimeType)1.0, t, t - (TimeType)1.0, (t + (TimeType)1.0) * (TimeType)0.5, t * (TimeType)0.5, t);
/*	T& k1 = mKeyFrames[mCurrentFrame[0]];
	T& k2 = mKeyFrames[mCurrentFrame[1]];
	T& k3 = mKeyFrames[mCurrentFrame[2]];
	T& k4 = mKeyFrames[mCurrentFrame[3]];

	// Use locally defined Lerp to avoid problems with VS-compilers that doesn't support
	// template functions.
	T lL12 = Lerp(k1, k2, t + (TimeType)1.0);
	T lL23 = Lerp(k2, k3, t);
	T lL34 = Lerp(k3, k4, t - (TimeType)1.0);
	return Lerp(Lerp(lL12, lL23, (t + (TimeType)1.0) * (TimeType)0.5), Lerp(lL23, lL34, t * (TimeType)0.5), t);
*/
}

template<class T, class TimeType, class TBase>
T CubicDeCasteljauSpline<T, TimeType, TBase>::GetBSplineValue() const
{
	TimeType t = GetCurrentSegmentTime();
	return GetValue((t + (TimeType)2.0) / (TimeType)3.0, (t + (TimeType)1.0) / (TimeType)3.0, t / (TimeType)3.0, (t + (TimeType)1.0) * (TimeType)0.5, t * (TimeType)0.5, t);
}

template<class T, class TimeType, class TBase>
T CubicDeCasteljauSpline<T, TimeType, TBase>::GetSlope()
{
	// TODO: fixme! This is just a slow and simple approximation.
	const TimeType lDelta = (GetMaximumTime()-GetMinimumTime())*(TimeType)0.001;
	StepInterpolation(-lDelta);
	const T lValue1 = GetValue();
	StepInterpolation(+lDelta*2);
	const T lValue2 = GetValue();
	const T lSlope = lValue2-lValue1;
	return (lSlope);
}

template<class T, class TimeType, class TBase>
T CubicDeCasteljauSpline<T, TimeType, TBase>::GetValue(TimeType t12, TimeType t23, TimeType t34,
				     TimeType t123, TimeType t234,
				     TimeType t) const
{
	T& k1 = mKeyFrames[mCurrentFrame[0]];
	T& k2 = mKeyFrames[mCurrentFrame[1]];
	T& k3 = mKeyFrames[mCurrentFrame[2]];
	T& k4 = mKeyFrames[mCurrentFrame[3]];

	// Use locally defined Lerp to avoid problems with VS-compilers that doesn't support
	// template functions.
	T lL12 = Lerp(k1, k2, t12);
	T lL23 = Lerp(k2, k3, t23);
	T lL34 = Lerp(k3, k4, t34);
	return Lerp(Lerp(lL12, lL23, t123), Lerp(lL23, lL34, t234), t);
}

template<class T, class TimeType, class TBase>
int CubicDeCasteljauSpline<T, TimeType, TBase>::FindFrameAtTime(TimeType pTime, int pStartFrame) const
{
	assert(pTime >= mTimeTags[0] && pTime <= mTimeTags[mCount]);

	int lCurrent = Math::Mod(pStartFrame, mCount + 1);
	int lNext = Math::Mod(lCurrent + 1, mCount + 1);

	// Loop forward.
	while(mTimeTags[lCurrent] < pTime && mTimeTags[lNext] < pTime)
	{
		lCurrent = lNext;
		lNext = Math::Mod(lCurrent + 1, mCount + 1);
	}

	// Loop backward.
	while(mTimeTags[lCurrent] > pTime && mTimeTags[lNext] > pTime)
	{
		lNext = lCurrent;
		lCurrent = Math::Mod(lNext - 1, mCount + 1);
	}

	return lCurrent;
}

template<class T, class TimeType, class TBase>
T CubicDeCasteljauSpline<T, TimeType, TBase>::Lerp(const T& a, const T& b, TimeType t) const
{
	return (a + (b-a)*t);
}

template<class T, class TimeType, class TBase>
TimeType CubicDeCasteljauSpline<T, TimeType, TBase>::GetMinimumTime() const
{
	return (mTimeTags[0]);
}

template<class T, class TimeType, class TBase>
TimeType CubicDeCasteljauSpline<T, TimeType, TBase>::GetMaximumTime() const
{
	return (mTimeTags[mCount]);
}

template<class T, class TimeType, class TBase>
TimeType CubicDeCasteljauSpline<T, TimeType, TBase>::GetCurrentSegmentTime() const
{
	return (mCurrentTime - mTimeTags[mCurrentFrame[1]]) / (mTimeTags[mCurrentFrame[2]] - mTimeTags[mCurrentFrame[1]]);
}
