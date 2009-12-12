
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <assert.h>
#include <list>
#include "HiResTimer.h"
#include "Thread.h"



namespace Lepra
{



// Used for single-point measurements.
class PerformanceData
{
public:
	PerformanceData();
	void Clear();
	void Set(double pMinimum, double pThis, double pMaximum);
	void Append(double pPeriodValue, double pTimeOfLastMeasure);
	double GetTimeOfLastMeasure() const;
	double GetMinimum() const;
	double GetLast() const;
	double GetSlidingAverage() const;
	double GetMaximum() const;

private:
	bool mFirstTime;
	double mTimeOfLastMeasure;
	double mMinimum;
	double mLast;
	double mSlidingAverage;
	double mMaximum;

	PerformanceData(const PerformanceData&);
	void operator=(const PerformanceData&);
};



// Used for increasing/decreasing counters, such as for network data monitoring.
template<class _Data = double> class SequencialPerformanceData: public PerformanceData
{
	typedef PerformanceData Parent;
public:
	SequencialPerformanceData();
	void Clear();
	void Set(double pMinimum, double pThis, double pMaximum, _Data pPreviousValue);
	void Append(double pDeltaTime, double pTimeOfLastMeasure, _Data pValue);

private:
	_Data mPreviousValue;

	SequencialPerformanceData(const SequencialPerformanceData&);
	void operator=(const SequencialPerformanceData&);
};



// Used for hierarchical measurements, such as a call graph. This class does not need
// mutex locking when adding nodes, since it's using per-thread storage of the currently
// active node. However, it is needed when fiddling with the list of roots for each thread.
class ScopePerformanceData: public PerformanceData
{
	typedef PerformanceData Parent;
public:
	typedef std::vector<ScopePerformanceData*> NodeArray;

	static ScopePerformanceData* Insert(const Lepra::String& pName, size_t pHash);

	ScopePerformanceData(ScopePerformanceData* pParent, const Lepra::String& pName, size_t pHash);

	static void ClearAll(const NodeArray& pNodes);
	void Append(double pPeriodValue, double pTimeOfLastMeasure);

	static NodeArray GetRoots();
	const Lepra::String& GetName() const;
	const NodeArray& GetChildren() const;

protected:
	ScopePerformanceData* FindChild(/*const Lepra::String& pName,*/ size_t pHash) const;

private:
	static void AddRoot(ScopePerformanceData* pNode);
	static void SetActive(ScopePerformanceData* pNode);
	static ScopePerformanceData* GetActive();

	const Lepra::String mName;
	const size_t mHash;
	ScopePerformanceData* mParent;
	NodeArray mChildArray;

	static NodeArray mRoots;
	static Lock mRootLock;
};



// Used to mesure the time and store it in a container structure.
template<class _T> class BasicScopeTimer
{
public:
	BasicScopeTimer();
	BasicScopeTimer(_T* pData);
	~BasicScopeTimer();
	void Attach(_T* pData);

protected:
	HiResTimer mTime;
	_T* mData;
};

typedef BasicScopeTimer<PerformanceData> ScopeTimer;
typedef BasicScopeTimer<ScopePerformanceData> CallScopeTimer;


// The reason the timer is created separately from the attaching of the performance data container
// is that we want the performance data container CREATION time listed in THIS scope, not in the
// parent scope.
#define LEPRA_MEASURE_SCOPE(name)	\
static const Lepra::String __lMeasureName(Lepra::StringUtility::Format(_T(#name) _T(";") _T(__FILE__) _T(";%i"), __LINE__));	\
static const std::LEPRA_STD_HASHER<Lepra::String> __lMeasureHasher;	\
static const size_t __lMeasureHash = (__lMeasureHasher)(__lMeasureName);	\
Lepra::CallScopeTimer __lMeasureTimer;	\
__lMeasureTimer.Attach(Lepra::ScopePerformanceData::Insert(__lMeasureName, __lMeasureHash))



}



#include "Performance.inl"
