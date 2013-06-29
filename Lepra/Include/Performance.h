
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <assert.h>
#include <list>
#include "HiResTimer.h"
#include "SpinLock.h"



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
	void ResetHitCount();
	int GetHitCount() const;
	double GetTimeOfLastMeasure() const;
	double GetMinimum() const;
	double GetLast() const;
	double GetSlidingAverage() const;
	double GetMaximum() const;
	double GetRangeFactor() const;

private:
	bool mFirstTime;
	int mHitCount;
	double mTimeOfLastMeasure;
	double mMinimum;
	double mLast;
	double mSlidingAverage;
	double mMaximum;
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
};



// Used for hierarchical measurements, such as a call graph. This class does not need
// mutex locking when adding nodes, since it's using per-thread storage of the currently
// active node. However, it is needed when fiddling with the list of roots for each thread.
class ScopePerformanceData: public PerformanceData
{
	typedef PerformanceData Parent;
public:
	typedef std::vector<ScopePerformanceData*> NodeArray;

	static ScopePerformanceData* Insert(const str& pName, size_t pHash);

	ScopePerformanceData(ScopePerformanceData* pParent, const str& pName, size_t pHash);
	~ScopePerformanceData();

	static void ClearAll();
	void Append(double pPeriodValue, double pTimeOfLastMeasure);

	static NodeArray GetRoots();
	const str& GetName() const;
	NodeArray GetChildren() const;
	const ScopePerformanceData* GetChild(const str& pName) const;

protected:
	static void ClearAll(NodeArray& pNodes);

	ScopePerformanceData* FindChild(/*const str& pName,*/ size_t pHash) const;

private:
	static void AddRoot(ScopePerformanceData* pNode);
	static void SetActive(ScopePerformanceData* pNode);
	static ScopePerformanceData* GetActive();

	const str mName;
	const size_t mHash;
	ScopePerformanceData* mParent;
	NodeArray mChildArray;

	static NodeArray mRoots;
	static SpinLock mRootLock;

	ScopePerformanceData(const ScopePerformanceData&);
	void operator=(const ScopePerformanceData&);
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
#define LEPRA_DO_MEASURE_SCOPE(name)	\
	CallScopeTimer __lMeasureTimer;	\
	static volatile bool __lMeasureNameInitialized = false;	\
	static str __lMeasureName;	\
	static size_t __lMeasureHash;	\
	if (!__lMeasureNameInitialized)	\
	{	\
		static SpinLock __lInitLock;	\
		ScopeSpinLock __lLock(&__lInitLock);	\
		if (!__lMeasureNameInitialized)	\
		{	\
			__lMeasureName = strutil::Format(_T(#name) _T(";") _T(__FILE__) _T(";%i"), __LINE__);	\
			__lMeasureHash = HashString(__lMeasureName.c_str());	\
			__lMeasureNameInitialized = true;	\
		}	\
	}	\
	__lMeasureTimer.Attach(ScopePerformanceData::Insert(__lMeasureName, __lMeasureHash))

#define LEPRA_MEASURE	0
#if LEPRA_MEASURE
#define LEPRA_MEASURE_SCOPE(name)	LEPRA_DO_MEASURE_SCOPE(name)
#else // !Measure
#define LEPRA_MEASURE_SCOPE(name)
#endif // Measure / !measure



}



#include "Performance.inl"
