
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/Math.h"
#include "../Include/CubicSpline.h"

#include <math.h>



namespace Lepra
{



void CubicSpline::Init(int pNumPoints, int pNumValuesPerPoint)
{
	mNumPoints = pNumPoints;
	mNumValuesPerPoint = pNumValuesPerPoint;

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

	mPoint   = new Point[mNumPoints];
	mSegment = new Segment[mNumPoints];

	mModularValue = new bool[pNumValuesPerPoint];
	mMinModValue  = new float[pNumValuesPerPoint];
	mMaxModValue  = new float[pNumValuesPerPoint];

	int i;
	for (i = 0; i < mNumPoints; i++)
	{
		mPoint[i].Init(mNumValuesPerPoint);
		mSegment[i].Init(mNumValuesPerPoint);
	}

	for (i = 0; i < mNumValuesPerPoint; i++)
	{
		mModularValue[i] = false;
		mMinModValue[i] = 0.0f;
		mMaxModValue[i] = 0.0f;
	}
}

bool CubicSpline::SetPointValues(int pPointIndex, float pTimeTag, const float* pValue)
{
	mPoint[pPointIndex].mT = pTimeTag;
	mPoint[pPointIndex].mValues.Set(pValue);
	mInitialized = false;

	return true;
}

void CubicSpline::SetModValue(int pValueIndex, float pMinModValue, float pMaxModValue)
{
	mModularValue[pValueIndex] = true;
	mMinModValue[pValueIndex] = pMinModValue;
	mMaxModValue[pValueIndex] = pMaxModValue;
	mInitialized = false;
}

void CubicSpline::StartInterpolation(float pTime)
{
	StartInterpolation(pTime, IM_ONCE, 0, 0);
	//CalcABCD();
}

void CubicSpline::StartInterpolation(float pTime, const float* pStartSlope, const float* pEndSlope)
{
	Values lStartSlope(mNumValuesPerPoint);
	Values lEndSlope(mNumValuesPerPoint);

	lStartSlope.Set(pStartSlope);
	lEndSlope.Set(pEndSlope);

	StartInterpolation(pTime, IM_ONCE, &lStartSlope, &lEndSlope);
}

void CubicSpline::StartLoopedInterpolation(float pTime, float pExtraTimeTag)
{
	mExtraTimeTag = pExtraTimeTag;
	StartInterpolation(pTime, IM_LOOP, 0, 0);
	//CalcABCD();
}

void CubicSpline::StartInterpolation(float pTime, InterpolationMode pMode, Values* pStartSlope, Values* pEndSlope)
{
	ReInitialize(pMode, pStartSlope, pEndSlope);

	mCurrentTime = pTime;

	mCurrentPoint = 0;
	StepForwardToCurrentFrame();
}

void CubicSpline::ReInitialize(InterpolationMode pMode, Values* pStartSlope, Values* pEndSlope)
{
	if (pMode != mMode)
	{
		mMode = pMode;
		mInitialized = false;
	}

	if (mInitialized == false)
	{
		if (mMode == IM_ONCE)
		{
			InitNaturalCubic(pStartSlope, pEndSlope, (pStartSlope != 0 && pEndSlope != 0));
		}
		else
		{
			InitNaturalCubicClosed();
		}

		mInitialized = true;
	}
}

void CubicSpline::StepInterpolation(float pDeltaTime)
{
	if (pDeltaTime == 0)
	{
		return;	// TRICKY: optimization.
	}

	mCurrentTime += pDeltaTime;

	if (mMode == IM_ONCE)
	{
		if (mCurrentTime > GetMaxTime())
		{
			mCurrentTime = GetMaxTime();
		}
		else if (mCurrentTime < GetMinTime())
		{
			mCurrentTime = GetMinTime();
		}
	}
	else
	{
		float lPrevTime = mCurrentTime;
		mCurrentTime = Math::Mod(mCurrentTime, GetMinTime(), mExtraTimeTag);
		pDeltaTime = mCurrentTime-lPrevTime;	// Used for stepping backwards.
	}

	if (pDeltaTime < 0.0f)
	{
		while (mCurrentPoint > 0 && mPoint[mCurrentPoint].mT > mCurrentTime)
		{
			mCurrentPoint--;
		}
	}
	StepForwardToCurrentFrame();

	//CalcABCD();
}

void CubicSpline::GotoAbsoluteTime(float pTime)
{
	StepInterpolation(pTime-mCurrentTime);
}

float CubicSpline::FindNearestTime(float pStepLength, const float* pWhere, float& pNearestDistance, float* pNearestPoint)
{
	// Ugly search algorithm. Looks in the step direction until the distance becomes greater than
	// the starting distance. Then turns around and looks in the other direction at half the velocity.
	// And so forth. This could probably be optimized using Newton-Raphson.
	float lNearestDistance[4];
	float lNewNearestDistance[4];
	float lDistance2 = 0;
	int x;
	for (x = 0; x < mNumValuesPerPoint; ++x)
	{
		lNearestDistance[x] = GetValue(x)-pWhere[x];
		lDistance2 += lNearestDistance[x]*lNearestDistance[x];
	}
	for (int y = 0; y < 20; ++y)
	{
		const int zc = 8;
		int z;
		for (z = 0; z < zc; ++z)
		{
			StepInterpolation(pStepLength);
			float lNewDistance2 = 0;
			for (int x = 0; x < mNumValuesPerPoint; ++x)
			{
				lNewNearestDistance[x] = GetValue(x)-pWhere[x];
				lNewDistance2 += lNewNearestDistance[x]*lNewNearestDistance[x];
			}
			if (lNewDistance2 >= lDistance2)
			{
				StepInterpolation(-pStepLength);	// Go back to previous point.
				pStepLength = pStepLength*-0.5f;	// Change search direction; increase granularity.
				if ((mCurrentTime == GetMinTime() && pStepLength < 0) ||
					(mCurrentTime == GetMaxTime() && pStepLength > 0))
				{
					pStepLength = -pStepLength;
				}
			}
			else
			{
				lDistance2 = lNewDistance2;
				for (int x = 0; x < mNumValuesPerPoint; ++x)
				{
					lNearestDistance[x] = lNewNearestDistance[x];
				}
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
	for (x = 0; x < mNumValuesPerPoint; ++x)
	{
		pNearestPoint[x] = lNearestDistance[x]+pWhere[x];
	}
	return (mCurrentTime);
}

void CubicSpline::StepForwardToCurrentFrame()
{
	if (mMode == IM_ONCE)
	{
		while (mCurrentPoint < (mNumPoints - 2) &&
			mPoint[mCurrentPoint + 1].mT < mCurrentTime)
		{
			mCurrentPoint++;
		}
	}
	else
	{
		while (mCurrentPoint < (mNumPoints - 1) &&
			mPoint[mCurrentPoint + 1].mT < mCurrentTime)
		{
			mCurrentPoint++;
		}
	}

	if (mCurrentPoint != mNumPoints - 1)
	{
		mCurrentSegmentTime = (mCurrentTime - mPoint[mCurrentPoint].mT) /
								(mPoint[mCurrentPoint + 1].mT - mPoint[mCurrentPoint].mT);
	}
	else
	{
		mCurrentSegmentTime = (mCurrentTime - mPoint[mCurrentPoint].mT) /
								(mExtraTimeTag - mPoint[mCurrentPoint].mT);
	}
}

void CubicSpline::InitNaturalCubic(Values* pStartSlope, Values* pEndSlope, bool pUseSlopes)
{
	InitModValues();

	int lN = mNumPoints - 1;

	Values* lGamma = new Values[mNumPoints];
	Values* lDelta = new Values[mNumPoints];
	Values* lD     = new Values[mNumPoints];
	int i, j;

	for (i = 0; i < mNumPoints; i++)
	{
		lGamma[i].Init(mNumValuesPerPoint);
		lDelta[i].Init(mNumValuesPerPoint);
		lD[i].Init(mNumValuesPerPoint);
	}

	lGamma[0] = 0.5f;
	for (i = 1; i < lN; i++)
	{
		for (j = 0; j < mNumValuesPerPoint; j++)
		{
			lGamma[i][j] = 1.0f / (4.0f - lGamma[i - 1][j]);
		}
	}
	
	for (j = 0; j < mNumValuesPerPoint; j++)
	{
		lGamma[lN][j] = 1.0f / (2.0f - lGamma[lN - 1][j]);
		lDelta[0][j] = 3.0f * (mPoint[1].mValues[j] - mPoint[0].mValues[j]) * lGamma[0][j];
	}

	float lRef = mPoint[1].mT - mPoint[0].mT;
	float lFactor;

	for (i = 1; i < lN; i++)
	{
		lFactor = (mPoint[i + 1].mT - mPoint[i - 1].mT) / (2.0f * lRef);
		for (j = 0; j < mNumValuesPerPoint; j++)
		{
			lDelta[i][j] = (3.0f * (mPoint[i + 1].mValues[j] - mPoint[i - 1].mValues[j]) * lFactor - lDelta[i - 1][j]) * lGamma[i][j];
		}
	}

	lFactor = (mPoint[lN].mT - mPoint[lN - 1].mT) / lRef;
	for (j = 0; j < mNumValuesPerPoint; j++)
	{
		lDelta[lN][j] = (3.0f * (mPoint[lN].mValues[j] - mPoint[lN - 1].mValues[j]) * lFactor - lDelta[lN - 1][j]) * lGamma[lN][j];
	}

	if (pUseSlopes == false)
	{
		lD[lN] = lDelta[lN];
	}
	else
	{
		lD[lN].Set(pEndSlope->mValue);
	}

	for (i = lN - 1; i >= 0; i--)
	{
		if (i != 0)
		{
			for (j = 0; j < mNumValuesPerPoint; j++)
			{
				lD[i][j] = lDelta[i][j] - lGamma[i][j] * lD[i + 1][j];
			}
		}
		else
		{
			if (pUseSlopes == false)
			{
				for (j = 0; j < mNumValuesPerPoint; j++)
				{
					lD[i][j] = lDelta[i][j] - lGamma[i][j] * lD[i + 1][j];
				}
			}
			else
			{
				lD[i].Set(pStartSlope->mValue);
			}
		}
	}

	float lRefT = GetMeanTDiff();
	for (i = 0; i < lN; i++)
	{
		float lScaleFactor = (mPoint[i + 1].mT - mPoint[i].mT) / lRefT;

		for (j = 0; j < mNumValuesPerPoint; j++)
		{
			mSegment[i].mA.Set(mPoint[i].mValues.mValue);
			mSegment[i].mB[j] = lD[i][j] * lScaleFactor;
			mSegment[i].mC[j] = (3.0f * (mPoint[i + 1].mValues[j] - mPoint[i].mValues[j]) -
								   2.0f * lD[i][j] * lScaleFactor - lD[i + 1][j] * lScaleFactor);
			mSegment[i].mD[j] = (2.0f * (mPoint[i].mValues[j] - mPoint[i + 1].mValues[j]) +
								   lD[i][j] * lScaleFactor + lD[i + 1][j] * lScaleFactor);
		}
	}

	delete[] lGamma;
	delete[] lDelta;
	delete[] lD;
}

void CubicSpline::InitNaturalCubicClosed()
{
	InitModValues();

	int lN = mNumPoints - 1;

	Values* lW = new Values[mNumPoints];
	Values* lV = new Values[mNumPoints];
	Values* lY = new Values[mNumPoints];
	Values* lD = new Values[mNumPoints];
	Values lZ(mNumValuesPerPoint);
	Values lH(mNumValuesPerPoint);
	Values lG(mNumValuesPerPoint);
	Values lF(mNumValuesPerPoint);
	int i, j;

	for (i = 0; i < mNumPoints; i++)
	{
		lW[i].Init(mNumValuesPerPoint);
		lV[i].Init(mNumValuesPerPoint);
		lY[i].Init(mNumValuesPerPoint);
		lD[i].Init(mNumValuesPerPoint);
	}

	lW[1] = lV[1] = lZ = 0.25f;

	for (j = 0; j < mNumValuesPerPoint; j++)
	{
		lY[0][j] = lZ[j] * 3.0f * (mPoint[1].mValues[j] - mPoint[lN].mValues[j]);
		lF[j] = 3.0f * (mPoint[0].mValues[j] - mPoint[lN - 1].mValues[j]);
	}

	lH = 4.0f;
	lG = 1.0f;

	for (i = 1; i < lN; i++)
	{
		for (j = 0; j < mNumValuesPerPoint; j++)
		{
			lV[i + 1][j] = lZ[j] = 1.0f / (4.0f - lV[i][j]);
			lW[i + 1][j] = -lZ[j] * lW[i][j];
			lY[i][j] = lZ[j] * (3.0f * (mPoint[i + 1].mValues[j] - mPoint[i - 1].mValues[j]) - lY[i - 1][j]);
			lH[j] -= lG[j] * lW[i][j];
			lF[j] -= lG[j] * lY[i - 1][j];
			lG[j] *= -lV[i][j];
		}
	}

	for (j = 0; j < mNumValuesPerPoint; j++)
	{
		lH[j] -= (lG[j] + 1) * (lV[lN][j] + lW[lN][j]);
		lY[lN][j] = lF[j] - (lG[j] + 1) * lY[lN - 1][j];

		lD[lN][j]     = lY[lN][j] / lH[j];
		lD[lN - 1][j] = lY[lN - 1][j] - (lV[lN][j] + lW[lN][j]) * lD[lN][j];
	}

	for (i = lN - 2; i >= 0; i--)
	{
		for (j = 0; j < mNumValuesPerPoint; j++)
		{
			lD[i][j] = lY[i][j] - lV[i + 1][j] * lD[i + 1][j] - lW[i + 1][j] * lD[lN][j];
		}
	}

	float lRefT = GetMeanTDiff();
	float lScaleFactor;
	for (i = 0; i < lN; i++)
	{
		lScaleFactor = (mPoint[i + 1].mT - mPoint[i].mT) / lRefT;
		mSegment[i].mA.Set(mPoint[i].mValues.mValue);

		for (j = 0; j < mNumValuesPerPoint; j++)
		{
			mSegment[i].mB[j] = lD[i][j] * lScaleFactor;
			mSegment[i].mC[j] = (3.0f * (mPoint[i + 1].mValues[j] - mPoint[i].mValues[j]) -
									2.0f * lD[i][j] * lScaleFactor - lD[i + 1][j] * lScaleFactor);
			mSegment[i].mD[j] = (2.0f * (mPoint[i].mValues[j] - mPoint[i + 1].mValues[j]) +
									lD[i][j] * lScaleFactor + lD[i + 1][j] * lScaleFactor);
		}
	}

	lScaleFactor = (mExtraTimeTag - mPoint[lN].mT) / lRefT;

	for (i = 0; i < mNumValuesPerPoint; i++)
	{
		mSegment[lN].mA[i] = mPoint[lN].mValues[i];
		mSegment[lN].mB[i] = lD[lN][i] * lScaleFactor;

		if (mModularValue[i] == false)
		{
			mSegment[lN].mC[i] = (3.0f * (mPoint[0].mValues[i] - mPoint[lN].mValues[i]) -
									   2.0f * lD[lN][i] * lScaleFactor - lD[0][i] * lScaleFactor);
			mSegment[lN].mD[i] = (2.0f * (mPoint[lN].mValues[i] - mPoint[0].mValues[i]) +
									   lD[lN][i] * lScaleFactor + lD[0][i] * lScaleFactor);
		}
		else
		{
			float lValue = GetClosestEquivalentModValue(mPoint[lN].mValues[i],
														  mPoint[0].mValues[i],
														  mMinModValue[i],
														  mMaxModValue[i]);
			mSegment[lN].mC[i] = (3.0f * (lValue - mPoint[lN].mValues[i]) -
									   2.0f * lD[lN][i] * lScaleFactor - lD[0][i] * lScaleFactor);
			mSegment[lN].mD[i] = (2.0f * (mPoint[lN].mValues[i] - lValue) +
									   lD[lN][i] * lScaleFactor + lD[0][i] * lScaleFactor);
/*
			float lNewValue;
			if (OptimizeLastSegment(i, lNewValue) == true)
			{
				mSegment[lN].mC[i] = (3.0f * (lNewValue - mPoint[lN].mValues[i]) -
										   2.0f * lD[lN][i] * lScaleFactor - lD[0][i] * lScaleFactor);
				mSegment[lN].mD[i] = (2.0f * (mPoint[lN].mValues[i] - lNewValue) +
										   lD[lN][i] * lScaleFactor + lD[0][i] * lScaleFactor);
			}
*/
		}
	}

	delete[] lW;
	delete[] lV;
	delete[] lY;
	delete[] lD;
}

float CubicSpline::GetValue(int pValueIndex)
{
	// mCurrentPoint is also the current segment.
	float lY = mSegment[mCurrentPoint].GetValue(pValueIndex, mCurrentSegmentTime);

	if (mModularValue[pValueIndex] == true)
	{
		return Math::Mod(lY, mMinModValue[pValueIndex], mMaxModValue[pValueIndex]);
	}
	else
	{
		return lY;
	}
}

float CubicSpline::GetSlope(int pValueIndex)
{
	return mSegment[mCurrentPoint].GetSlope(pValueIndex, mCurrentSegmentTime);
}

float CubicSpline::GetSlopeChange(int pValueIndex)
{
	return mSegment[mCurrentPoint].GetSlopeChange(pValueIndex, mCurrentSegmentTime);
}

float CubicSpline::GetClosestEquivalentModValue(float pPreviousValue,
											  float pCurrentValue,
											  float pMinMod,
											  float pMaxMod)
{
	float lPrev  = Math::Mod(pPreviousValue, pMinMod, pMaxMod);
	float lValue = Math::Mod(pCurrentValue, pMinMod, pMaxMod);

	float lDown = lValue - lPrev;
	float lUp   = (lPrev - pMinMod) + (pMaxMod - lValue);

	if (lValue < lPrev)
		lUp = -((lValue - pMinMod) + (pMaxMod - lPrev));

	if (fabs(lDown) < fabs(lUp))
		return (pPreviousValue + lDown);
	else
		return (pPreviousValue - lUp);
}
/*
void CubicSpline::CalcABCD()
{
	float lDX;
	float lDT;

	if (mCurrentPoint == mNumPoints - 1)
	{
		lDX = mExtraTimeTag - mPoint[mCurrentPoint].mT;
		lDT = mExtraTimeTag - mCurrentTime;
	}
	else
	{
		lDX = mPoint[mCurrentPoint + 1].mT - mPoint[mCurrentPoint].mT;
		lDT = mPoint[mCurrentPoint + 1].mT - mCurrentTime;
	}

	float lDX2 = lDX * lDX;

	mA = lDT / lDX;
	mB = 1.0f - mA;
	mC = (mA * mA * mA - mA) * lDX2 * (1.0f / 6.0f);
	mD = (mB * mB * mB - mB) * lDX2 * (1.0f / 6.0f);
}
*/

float CubicSpline::GetMeanTDiff()
{
	int lN = mNumPoints - 1;

	float lMeanTDiff = 0.0f;
	for (int i = 0; i < lN; i++)
	{
		lMeanTDiff += mPoint[i + 1].mT - mPoint[i].mT;
	}

	lMeanTDiff /= (float)lN;

	return lMeanTDiff;
}

void CubicSpline::InitModValues()
{
	for (int j = 0; j < mNumValuesPerPoint; j++)
	{
		if (mModularValue[j] == true)
		{
			float lPrevValue = 0.0f;

			for (int i = 0; i < mNumPoints; i++)
			{
				mPoint[i].mValues[j] = GetClosestEquivalentModValue(lPrevValue,
																	   mPoint[i].mValues[j],
																	   mMinModValue[j], 
																	   mMaxModValue[j]);
				lPrevValue = mPoint[i].mValues[j];
			}
		}
	}
}

bool CubicSpline::OptimizeLastSegment(int pValueIndex, float& pValue)
{
	// This function is only used in combination with modular values and
	// closed cubic splines. It checks wether the last segment (the one
	// that connects the last point with the first one) is a detour and can
	// take a shorter path. This test is done by finding the value where
	// the segment's derivative is zero, and by checking if this value is
	// closer to the destination than the segment's starting value.

	Segment* lSeg = &mSegment[mNumPoints - 1];

	int i = pValueIndex;

	if (mModularValue[i] == true)
	{
		float lP = (lSeg->mC[i] / lSeg->mD[i]) * (2.0f / 3.0f);
		float lQ = (lSeg->mB[i] / lSeg->mD[i]) * (1.0f / 3.0f);
		float lS = (lP * lP * 0.25f) - lQ;

		if (lS > 0.0f)
		{
			float lX1 = -lP * 0.5f + sqrt(lS);
			float lX2 = -lP * 0.5f - sqrt(lS);
			float lX = lX1;	// TODO: Assign correct data. Assignment missing prior to 2006-10-19.
			bool lValidX = false;

			if (lX1 >= 0.0f && lX1 <= 1.0f)
			{
				lX = lX1;
				lValidX = true;

				if (lX2 >= 0.0f && lX2 <= 1.0f && lX2 < lX1)
				{
					lX = lX2;
				}
			}
			else if(lX2 >= 0.0f && lX2 <= 1.0f)
			{
				lX = lX2;
				lValidX = true;
			}

			if (lValidX == true)
			{
				float lValue = lSeg->GetValue(i, lX);
				pValue = GetClosestEquivalentModValue(lValue,
														mPoint[0].mValues[i],
														mMinModValue[i], 
														mMaxModValue[i]);
				return true;
			}
		}
	}

	return false;
}



}
