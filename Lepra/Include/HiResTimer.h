
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "LepraTypes.h"



namespace Lepra
{



class HiResTimer
{
public:
	HiResTimer(bool pUseShadow = true);
	HiResTimer(uint64 pCount);
	HiResTimer(const HiResTimer& pTimer);
	~HiResTimer();

	void EnableShadowCounter(bool pEnable);
	static void StepCounterShadow();

	// Updates time, returns current time diff and resets time diff.
	double PopTimeDiff();
	// Updates time, returns current time diff (does NOT reset time diff).
	double QueryTimeDiff();

	// Returns the time in seconds.
	double GetTime() const;

	// Returns the actual counter value.
	int64 GetCounter() const;

	// Updates to the current time. 
	// Does not update the previous time.
	void UpdateTimer();

	// Sets the previous time to the same value as
	// the current time.
	void ClearTimeDiff();
	void ReduceTimeDiff(double pSecsonds);

	// Returns the time difference between the current time
	// and the previous time.
	double GetTimeDiff() const;

	int64 GetCounterDiff() const;

	// Operators.
	const HiResTimer& operator = (const HiResTimer& pTimer);

	bool operator <  (const HiResTimer& pTimer) const;
	bool operator >  (const HiResTimer& pTimer) const;
	bool operator == (const HiResTimer& pTimer) const;
	bool operator != (const HiResTimer& pTimer) const;
	bool operator <= (const HiResTimer& pTimer) const;
	bool operator >= (const HiResTimer& pTimer) const;

	HiResTimer& operator += (const HiResTimer& pTimer);
	HiResTimer& operator -= (const HiResTimer& pTimer);

	uint64 GetSystemCounterShadow();

	static void InitFrequency();
	static int64 GetFrequency();
	static double GetPeriod();
	static uint64 GetSystemCounter();

protected:
	uint64 mPrevCounter;
	uint64 mCounter;
	bool mUseShadow;

	static uint64 mFrequency;
	static double mPeriod;
	static uint64 mLastSystemCounter;
};



class StopWatch: public HiResTimer
{
	typedef HiResTimer Parent;
public:
	StopWatch();
	bool TryStart();
	void Start();
	bool ResumeFromLapTime();
	bool ResumeFromStop();
	void Stop();
	bool IsStarted() const;
	double QuerySplitTime();
	int GetStartCount() const;

protected:
	bool mIsStarted;
	int mStartCount;
};



}
