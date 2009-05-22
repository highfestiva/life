
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#pragma once

#include <assert.h>
#include "HiResTimer.h"



namespace Lepra
{



class PerformanceData
{
public:
	PerformanceData();
	void Clear();
	void Set(double pMinimum, double pThis, double pMaximum);
	void Update(double pPeriodValue);
	double GetMinimum() const;
	double GetLast() const;
	double GetSlidingAverage() const;
	double GetMaximum() const;

private:
	bool mFirstTime;
	double mMinimum;
	double mLast;
	double mSlidingAverage;
	double mMaximum;

	PerformanceData(const PerformanceData&);
	void operator=(const PerformanceData&);
};



template<class _Data = double> class SequencialPerformanceData: public PerformanceData
{
	typedef PerformanceData Parent;
public:
	SequencialPerformanceData();
	void Clear();
	void Set(double pMinimum, double pThis, double pMaximum, _Data pPreviousValue);
	void Update(double pTime, _Data pValue);

private:
	_Data mPreviousValue;

	SequencialPerformanceData(const SequencialPerformanceData&);
	void operator=(const SequencialPerformanceData&);
};



class ScopeTimer
{
public:
	ScopeTimer(PerformanceData* pData);
	~ScopeTimer();
	void Update();

protected:
	HiResTimer mTime;
	PerformanceData* mData;
};



}



#include "Performance.inl"
