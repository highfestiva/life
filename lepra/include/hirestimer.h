
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepratypes.h"



namespace lepra {



class HiResTimer {
public:
	HiResTimer(bool use_shadow = true);
	HiResTimer(uint64 count);
	HiResTimer(const HiResTimer& timer);
	~HiResTimer();

	void EnableShadowCounter(bool enable);
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
	void ReduceTimeDiff(double secsonds);

	// Returns the time difference between the current time
	// and the previous time.
	double GetTimeDiff() const;

	int64 GetCounterDiff() const;

	// Operators.
	const HiResTimer& operator = (const HiResTimer& timer);

	bool operator <  (const HiResTimer& timer) const;
	bool operator >  (const HiResTimer& timer) const;
	bool operator == (const HiResTimer& timer) const;
	bool operator != (const HiResTimer& timer) const;
	bool operator <= (const HiResTimer& timer) const;
	bool operator >= (const HiResTimer& timer) const;

	HiResTimer& operator += (const HiResTimer& timer);
	HiResTimer& operator -= (const HiResTimer& timer);

	uint64 GetSystemCounterShadow();

	static void InitFrequency();
	static int64 GetFrequency();
	static double GetPeriod();
	static uint64 GetSystemCounter();

protected:
	uint64 prev_counter_;
	uint64 counter_;
	bool use_shadow_;

	static uint64 frequency_;
	static double period_;
	static uint64 last_system_counter_;
};



class StopWatch: public HiResTimer {
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
	bool is_started_;
	int start_count_;
};



}
