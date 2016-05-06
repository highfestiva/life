
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepraassert.h"
#include <list>
#include "hirestimer.h"
#include "spinlock.h"



namespace lepra {



// Used for single-point measurements.
class PerformanceData {
public:
	PerformanceData();
	void Clear();
	void Set(double minimum, double value, double maximum);
	void Append(double period_value, double time_of_last_measure);
	void ResetHitCount();
	int GetHitCount() const;
	double GetTimeOfLastMeasure() const;
	double GetMinimum() const;
	double GetLast() const;
	double GetSlidingAverage() const;
	double GetMaximum() const;
	double GetRangeFactor() const;

private:
	bool first_time_;
	int hit_count_;
	double time_of_last_measure_;
	double minimum_;
	double last_;
	double sliding_average_;
	double maximum_;
};



// Used for increasing/decreasing counters, such as for network data monitoring.
template<class _Data = double> class SequencialPerformanceData: public PerformanceData {
	typedef PerformanceData Parent;
public:
	SequencialPerformanceData();
	void Clear();
	void Set(double minimum, double value, double maximum, _Data previous_value);
	void Append(double delta_time, double time_of_last_measure, _Data value);

private:
	_Data previous_value_;
};



// Used for hierarchical measurements, such as a call graph. This class does not need
// mutex locking when adding nodes, since it's using per-thread storage of the currently
// active node. However, it is needed when fiddling with the list of roots for each thread.
class ScopePerformanceData: public PerformanceData {
	typedef PerformanceData Parent;
public:
	typedef std::vector<ScopePerformanceData*> NodeArray;

	static ScopePerformanceData* Insert(const str& _name, size_t hash);

	ScopePerformanceData(ScopePerformanceData* parent, const str& _name, size_t hash);
	~ScopePerformanceData();

	static void EraseAll();
	static void ResetAll();
	void Append(double period_value, double time_of_last_measure);

	static NodeArray GetRoots();
	const str& GetName() const;
	NodeArray GetChildren() const;
	const ScopePerformanceData* GetChild(const str& _name) const;

protected:
	static void ResetAll(NodeArray& nodes);

	ScopePerformanceData* FindChild(/*const str& _name,*/ size_t hash) const;

private:
	static void AddRoot(ScopePerformanceData* node);
	static void SetActive(ScopePerformanceData* node);
	static ScopePerformanceData* GetActive();

	const str name_;
	const size_t hash_;
	ScopePerformanceData* parent_;
	NodeArray child_array_;

	static NodeArray roots_;
	static SpinLock root_lock_;

	ScopePerformanceData(const ScopePerformanceData&);
	void operator=(const ScopePerformanceData&);
};



// Used to mesure the time and store it in a container structure.
template<class _T> class BasicScopeTimer {
public:
	BasicScopeTimer();
	BasicScopeTimer(_T* data);
	~BasicScopeTimer();
	void Attach(_T* data);

protected:
	HiResTimer time_;
	_T* data_;
};

typedef BasicScopeTimer<PerformanceData> ScopeTimer;
typedef BasicScopeTimer<ScopePerformanceData> CallScopeTimer;


// The reason the timer is created separately from the attaching of the performance data container
// is that we want the performance data container CREATION time listed in THIS scope, not in the
// parent scope.
#define LEPRA_DO_MEASURE_SCOPE(name)	\
	CallScopeTimer __lMeasureTimer;	\
	static volatile bool __lMeasureNameInitialized = false;	\
	static HashedString __lMeasureName;	\
	static size_t __lMeasureHash;	\
	if (!__lMeasureNameInitialized)	\
	{	\
		static SpinLock __lInitLock;	\
		ScopeSpinLock __lLock(&__lInitLock);	\
		if (!__lMeasureNameInitialized)	\
		{	\
			__lMeasureName = HashedString(strutil::Format(#name ";" __FILE__ ";%i", __LINE__));	\
			__lMeasureHash = __lMeasureName.hash_;	\
			__lMeasureNameInitialized = true;	\
		}	\
	}	\
	__lMeasureTimer.Attach(ScopePerformanceData::Insert(__lMeasureName, __lMeasureHash))

#define kLepraMeasure	0
#if kLepraMeasure
#define LEPRA_MEASURE_SCOPE(name)	LEPRA_DO_MEASURE_SCOPE(name)
#else // !Measure
#define LEPRA_MEASURE_SCOPE(name)
#endif // Measure / !measure



}



#include "performance.inl"
