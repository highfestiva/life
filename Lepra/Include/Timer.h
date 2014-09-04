
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// A time class that counts milliseconds and seconds.

// Each time you call UpdateTimer(), the time difference 
// since the last clear will be updated. You can clear or 
// reduce this time difference by calling ClearTimeDiff() 
// or ReduceTimeDiff().



#pragma once

#include "LepraTypes.h"



namespace Lepra
{



class Timer
{
public:
	Timer();
	Timer(uint64 pMilliSecs);
	Timer(const Timer& pTimer);
	virtual ~Timer();

	// Updates time, returns current time diff and resets time diff.
	double PopTimeDiff();
	// Updates time, returns current time diff (does NOT reset time diff).
	double QueryTimeDiff();
	// Returns the time in seconds.
	double GetTime();

	// Updates to the current time. 
	// Does not update the previous time.
	void UpdateTimer();

	// Sets the previous time to the same value as
	// the current time.
	void ClearTimeDiff();
	void ReduceTimeDiff(uint64 pMilliSecs);
	void ReduceTimeDiff(double pSecs);

	// Returns the time difference between the current time
	// and the previous time.
	double GetTimeDiff() const;

	// Operators.
	const Timer& operator = (const Timer& pTimer);

	bool operator <  (const Timer& pTimer) const;
	bool operator >  (const Timer& pTimer) const;
	bool operator == (const Timer& pTimer) const;
	bool operator != (const Timer& pTimer) const;
	bool operator <= (const Timer& pTimer) const;
	bool operator >= (const Timer& pTimer) const;

	Timer& operator += (const Timer& pTimer);
	Timer& operator -= (const Timer& pTimer);

protected:
	void SetTime(uint64 pMilliSecs);

private:
	uint64 mMilliSecs;
	uint64 mPrevMilliSecs;
};



}
