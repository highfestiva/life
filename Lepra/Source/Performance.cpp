
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include "../Include/Math.h"
#include "../Include/Performance.h"



namespace Lepra
{



PerformanceData::PerformanceData()
{
	Clear();
}

void PerformanceData::Clear()
{
	mFirstTime = true;
	Set(0, 0, 0);
}

void PerformanceData::Set(double pMinimum, double pThis, double pMaximum)
{
	mMinimum = pMinimum;
	mLast = pThis;
	mSlidingAverage = pThis;
	mMaximum = pMaximum;
}

void PerformanceData::Update(double pPeriodValue)
{
	if (mFirstTime)
	{
		mFirstTime = false;
		Set(pPeriodValue, pPeriodValue, pPeriodValue);
	}
	else
	{
		mMinimum = (pPeriodValue < mMinimum)? pPeriodValue : mMinimum;
		mLast = pPeriodValue;
		mSlidingAverage = Lepra::Math::Lerp(mSlidingAverage, mLast, 0.1);
		mMaximum = (pPeriodValue > mMaximum)? pPeriodValue : mMaximum;
	}
}

double PerformanceData::GetMinimum() const
{
	return (mMinimum);
}

double PerformanceData::GetLast() const
{
	return (mLast);
}

double PerformanceData::GetSlidingAverage() const
{
	return (mSlidingAverage);
}

double PerformanceData::GetMaximum() const
{
	return (mMaximum);
}



ScopeTimer::ScopeTimer(PerformanceData* pData):
	mData(pData)
{
}

ScopeTimer::~ScopeTimer()
{
	Update();
	mData = 0;
}

void ScopeTimer::Update()
{
	mData->Update(mTime.PopTimeDiff());
}



}
