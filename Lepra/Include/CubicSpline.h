/*
	Class:  CubicSpline
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	CubicSpline is using the natural cubic spline algorithm. It was hard to
	find any good information about it on the internet, and I actually don't
	fully understand the math and the algorithms used. The original algorithm
	only supported equally spaced points, and I managed to change it to support
	unequal spaces by scaling the derivatives in InitNaturalCubic() and in
	InitNaturalCubicClosed().

	HOW TO USE THIS CLASS:

	Start by calling Init() to allocate all needed memory and such.
	Then setup each point in ascending time order by calling SetPointValues().
	Each point can take several values to interpolate.

	After setting all points, you can start interpolating the spline
	by calling StartInterpolation(), and step by calling StepInterpolation().
	When the interpolation has started, you can retrieve all interpolated
	values by calling GetValue() passing the appropriate value index.
*/

#ifndef CUBICSPLINE_H
#define CUBICSPLINE_H

#include "LepraTypes.h"

namespace Lepra
{

class CubicSpline
{
public:

	inline CubicSpline();
	inline ~CubicSpline();
	
	// pNumPoints = number of control points.
	// pNumValuesPerPoint = number of values to interpolate per point.
	void Init(int pNumPoints, int pNumValuesPerPoint);

	// The points in the spline must be sorted in ascending time order.
	// If the time tag given is less than or equal to the previous point's
	// time tag, the point will not be updated, and the function will return
	// false.
	bool SetPointValues(int pPointIndex, float pTimeTag, const float* pValue);

	// Defines a "modular" value (a value which will be wrapped around whithin
	// the range defined by pMinModValue and pMaxModValue).
	// This works perfectly with open splines, but not as well with closed (looped)
	// splines (but it works ok anyway).
	void SetModValue(int pValueIndex, float pMinModValue, float pMaxModValue);

	inline float GetMinTime() const;
	inline float GetMaxTime() const;

	// Returns time t such that 0 <= t <= 1, where t = 0 has PointIndex = x, and t = 1 
	// PointIndex = x + 1.
	inline float GetCurrentSegmentTime();

	// Initializes spline interpolation at time pTime.
	void StartInterpolation(float pTime);
	void StartInterpolation(float pTime, const float* pStartSlope, const float* pEndSlope);
	void StartLoopedInterpolation(float pTime, float pExtraTimeTag);
	void StepInterpolation(float pDeltaTime);
	void GotoAbsoluteTime(float pTime);
	float FindNearestTime(float pStepLength, const float* pWhere, float& pNearestDistance, float* pNearestPoint);

	float GetValue(int pValueIndex);	// y
	float GetSlope(int pValueIndex);	// y'
	float GetSlopeChange(int pValueIndex);	// y''

	inline int GetNumPoints();
	inline int GetNumValuesPerPoint();

protected:
private:
	enum InterpolationMode
	{
		IM_ONCE = 0,
		IM_LOOP
	};

	// This class provides the interface to treat an array of float values
	// as if they were only one value.
	class Values
	{
	public:

		inline Values() :
			mValue(0)
		{
		}

		inline Values(int pNumValues) :
			mValue(0)
		{
			Init(pNumValues);
		}

		inline Values(const Values& pValues) :
			mValue(0)
		{
			Init(pValues.mNumValues);
			Set(pValues.mValue);
		}

		inline ~Values()
		{
			if (mValue != 0)
				delete[] mValue;
		}

		inline void Init(int pNumValues)
		{
			if (mValue != 0)
				delete[] mValue;

			mValue    = new float[pNumValues];

			for (int i = 0; i < pNumValues; i++)
			{
				mValue[i] = 0;
			}

			mNumValues = pNumValues;
		}

		inline void Set(const float* pValue)
		{
			for (int i = 0; i < mNumValues; i++)
			{
				mValue[i] = pValue[i];
			}
		}

		inline void Set(float pValue)
		{
			for (int i = 0; i < mNumValues; i++)
			{
				mValue[i] = pValue;
			}
		}

		inline float& operator[] (int pIndex)
		{
			return mValue[pIndex];
		}

		inline Values operator=  (const Values& pValues) { Set(pValues.mValue); return *this; }
		inline Values operator=  (float pValue) { Set(pValue); return *this; }

		inline Values operator+= (const Values& pValues) {	for (int i = 0; i < mNumValues; i++) mValue[i] += pValues.mValue[i]; return *this; }
		inline Values operator-= (const Values& pValues) {	for (int i = 0; i < mNumValues; i++) mValue[i] -= pValues.mValue[i]; return *this; }
		inline Values operator*= (const Values& pValues) {	for (int i = 0; i < mNumValues; i++) mValue[i] *= pValues.mValue[i]; return *this; }
		inline Values operator/= (const Values& pValues) {	for (int i = 0; i < mNumValues; i++) mValue[i] /= pValues.mValue[i]; return *this; }

		inline Values operator-  () const
		{
			Values lTemp(*this); 
			lTemp *= -1.0f;
			return lTemp;
		}

		inline Values operator+  (const Values& pValues) const
		{
			Values lTemp(*this); 
			return (lTemp += pValues);
		}
		inline Values operator-  (const Values& pValues) const
		{
			Values lTemp(*this); 
			return (lTemp -= pValues);
		}
		inline Values operator*  (const Values& pValues) const
		{
			Values lTemp(*this); 
			return (lTemp *= pValues);
		}
		inline Values operator/  (const Values& pValues) const
		{
			Values lTemp(*this); 
			return (lTemp /= pValues);
		}

		inline Values operator+= (float pValue) { for(int i = 0; i < mNumValues; i++) mValue[i] += pValue; return *this; }
		inline Values operator-= (float pValue) { for(int i = 0; i < mNumValues; i++) mValue[i] -= pValue; return *this; }
		inline Values operator*= (float pValue) { for(int i = 0; i < mNumValues; i++) mValue[i] *= pValue; return *this; }
		inline Values operator/= (float pValue) { for(int i = 0; i < mNumValues; i++) mValue[i] /= pValue; return *this; }

		inline Values operator+  (float pValue) const
		{
			Values lTemp(*this); 
			return (lTemp += pValue);
		}
		inline Values operator-  (float pValue) const
		{
			Values lTemp(*this); 
			return (lTemp -= pValue);
		}
		inline Values operator*  (float pValue) const
		{
			Values lTemp(*this); 
			return (lTemp *= pValue);
		}
		inline Values operator/  (float pValue) const
		{
			Values lTemp(*this); 
			return (lTemp /= pValue);
		}

		inline friend Values operator+ (float pValue, const Values& pValues)
		{
			return (pValues + pValue);
		}

		inline friend Values operator- (float pValue, const Values& pValues)
		{
			Values lTemp(pValues.mNumValues);
			for (int i = 0; i < pValues.mNumValues; i++)
			{
				lTemp.mValue[i] = pValue - pValues.mValue[i];
			}
			return lTemp;
		}

		inline friend Values operator* (float pValue, const Values& pValues)
		{
			return (pValues * pValue);
		}

		inline friend Values operator/ (float pValue, const Values& pValues)
		{
			Values lTemp(pValues.mNumValues);
			for (int i = 0; i < pValues.mNumValues; i++)
			{
				lTemp.mValue[i] = pValue / pValues.mValue[i];
			}
			return lTemp;
		}

		int mNumValues;
		float* mValue;
	};




	class Point
	{
	public:

		inline Point() :
			mT(0)
		{
		}

		inline void Init(int pNumValues)
		{
			mValues.Init(pNumValues);
		}

		float mT;			// Time tag, or x-coordinate.
		Values mValues;	// Y-coordinates.
	};




	// Cubic spline segment.
	class Segment
	{
	public:

		inline void Init(int pNumValues)
		{
			mA.Init(pNumValues);
			mB.Init(pNumValues);
			mC.Init(pNumValues);
			mD.Init(pNumValues);
		}

		// 0 <= pT <= 1 within the segment.
		inline float GetValue(int pValueIndex, float pT)
		{
			int i = pValueIndex;
			return ((((mD[i] * pT) + mC[i]) * pT + mB[i]) * pT) + mA[i];
		}

		inline float GetSlope(int pValueIndex, float pT)
		{
			int i = pValueIndex;
			return 3.0f * mD[i] * pT * pT + 2.0f * mC[i] * pT + mB[i];
		}

		inline float GetSlopeChange(int pValueIndex, float pT)
		{
			int i = pValueIndex;
			return 6.0f * mD[i] * pT + 2.0f * mC[i];
		}

		Values mA;
		Values mB;
		Values mC;
		Values mD;
	};

	void StartInterpolation(float pTime, InterpolationMode pMode, Values* pStartSlope, Values* pEndSlope);
	void ReInitialize(InterpolationMode pMode, Values* pStartSlope, Values* pEndSlope);
	void StepForwardToCurrentFrame();

	void InitNaturalCubic(Values* pStartSlope, Values* pEndSlope, bool pUseSlopes);
	void InitNaturalCubicClosed();

	void InitModValues();
	bool OptimizeLastSegment(int pValueIndex, float& pValue);

//	void CalcABCD();

	float GetClosestEquivalentModValue(float pPreviousValue, 
									   float pCurrentValue,
									   float pMinMod,
									   float pMaxMod);

	float GetMeanTDiff();

	// Spline data.
	int mNumPoints;
	int mNumValuesPerPoint;
	Point* mPoint;
	Segment* mSegment;

	bool mInitialized;
	InterpolationMode mMode;

	// Modular values.
	bool* mModularValue;
	float* mMinModValue;
	float* mMaxModValue;

	// Interpolation data.
	int mCurrentPoint;
	float mCurrentTime;
	float mCurrentSegmentTime;
	float mExtraTimeTag;
};

CubicSpline::CubicSpline() :
	mNumPoints(0),
	mNumValuesPerPoint(0),
	mPoint(0),
	mSegment(0),
	mInitialized(false),
	mMode(IM_ONCE),
	mModularValue(0),
	mMinModValue(0),
	mMaxModValue(0),
	mCurrentPoint(0),
	mCurrentTime(0),
	mCurrentSegmentTime(0),
	mExtraTimeTag(0)
{
}

CubicSpline::~CubicSpline()
{
	if (mPoint != 0)
		delete[] mPoint;

	if (mSegment != 0)
		delete[] mSegment;

	if (mModularValue != 0)
		delete[] mModularValue;

	if (mMinModValue != 0)
		delete[] mMinModValue;

	if (mMaxModValue != 0)
		delete[] mMaxModValue;
}

float CubicSpline::GetMinTime() const
{
	return mPoint[0].mT;
}

float CubicSpline::GetMaxTime() const
{
	return mPoint[mNumPoints - 1].mT;
}

float CubicSpline::GetCurrentSegmentTime()
{
	return mCurrentSegmentTime;
}

int CubicSpline::GetNumPoints()
{
	return mNumPoints;
}

int CubicSpline::GetNumValuesPerPoint()
{
	return mNumValuesPerPoint;
}

} // End namespace.

#endif
