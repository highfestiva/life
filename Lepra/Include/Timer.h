
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// A time class that counts milliseconds and seconds.

// Each time you call UpdateTimer(), the time difference 
// since the last clear will be updated. You can clear or 
// reduce this time difference by calling ClearTimeDiff() 
// or ReduceTimeDiff().



#pragma once

#include "Lepra.h"



namespace Lepra
{



class Timer
{
public:
	inline Timer();
	inline Timer(uint64 pMilliSecs);
	inline Timer(const Timer& pTimer);
	inline virtual ~Timer();

	// Updates time, returns current time diff and resets time diff.
	inline double PopTimeDiff();
	// Updates time, returns current time diff (does NOT reset time diff).
	inline double QueryTimeDiff();
	// Returns the time in seconds.
	inline double GetTime();

	// Updates to the current time. 
	// Does not update the previous time.
	inline void UpdateTimer();

	// Sets the previous time to the same value as
	// the current time.
	inline void ClearTimeDiff();
	inline void ReduceTimeDiff(uint64 pMilliSecs);
	inline void ReduceTimeDiff(double pSecs);

	// Returns the time difference between the current time
	// and the previous time.
	inline double GetTimeDiff() const;

	// Operators.
	inline const Timer& operator = (const Timer& pTimer);

	inline bool operator <  (const Timer& pTimer) const;
	inline bool operator >  (const Timer& pTimer) const;
	inline bool operator == (const Timer& pTimer) const;
	inline bool operator != (const Timer& pTimer) const;
	inline bool operator <= (const Timer& pTimer) const;
	inline bool operator >= (const Timer& pTimer) const;

	inline Timer& operator += (const Timer& pTimer);
	inline Timer& operator -= (const Timer& pTimer);

protected:
	inline void SetTime(uint64 pMilliSecs);

private:
	uint64 mMilliSecs;
	uint64 mPrevMilliSecs;
};



Timer::Timer() :
	mMilliSecs(0),
	mPrevMilliSecs(0)
{
	UpdateTimer();
	ClearTimeDiff();
}

Timer::Timer(uint64 pMilliSecs) :
	mMilliSecs(pMilliSecs),
	mPrevMilliSecs(pMilliSecs)
{
}

Timer::Timer(const Timer& pTimer) :
	mMilliSecs(pTimer.mMilliSecs),
	mPrevMilliSecs(pTimer.mPrevMilliSecs)
{
}

Timer::~Timer()
{
}

double Timer::PopTimeDiff()
{
	double lTime = QueryTimeDiff();
	ClearTimeDiff();
	return (lTime);
}

double Timer::QueryTimeDiff()
{
	UpdateTimer();
	return (GetTimeDiff());
}

double Timer::GetTime()
{
	return (double)mMilliSecs * (1.0 / 1000.0);
}

void Timer::UpdateTimer()
{
#ifdef LEPRA_WINDOWS
	SetTime(::GetTickCount());
#elif defined(LEPRA_POSIX)
	// Almost the same implementation as HiResTimer.
	timeval lTimeValue;
	::gettimeofday(&lTimeValue, 0);
	SetTime(((uint64)lTimeValue.tv_sec * 1000 + lTimeValue.tv_usec / 1000));
#else // <Unknown target>
#error Timer::UpdateTimer() not implemented on this platform!
#endif // LEPRA_WINDOWS/LEPRA_POSIX/<Unknown target>
}

double Timer::GetTimeDiff() const
{
	return ((double)(mMilliSecs - mPrevMilliSecs) * (1.0 / 1000.0));
}

void Timer::ClearTimeDiff()
{
	mPrevMilliSecs = mMilliSecs;
}

void Timer::ReduceTimeDiff(uint64 pMilliSecs)
{
	mPrevMilliSecs += pMilliSecs;

	if (mPrevMilliSecs > mMilliSecs)
	{
		mPrevMilliSecs = mMilliSecs;
	}
}

void Timer::ReduceTimeDiff(double pSecs)
{
	mPrevMilliSecs += (uint64)(pSecs * 1000.0);

	if (mPrevMilliSecs > mMilliSecs)
	{
		mPrevMilliSecs = mMilliSecs;
	}
}

const Timer& Timer::operator = (const Timer& pTimer)
{
	mMilliSecs = pTimer.mMilliSecs;
	mPrevMilliSecs = pTimer.mPrevMilliSecs;
	return *this;
}

bool Timer::operator <  (const Timer& pTimer) const
{
	return mMilliSecs < pTimer.mMilliSecs;
}

bool Timer::operator >  (const Timer& pTimer) const
{
	return mMilliSecs > pTimer.mMilliSecs;
}

bool Timer::operator == (const Timer& pTimer) const
{
	return mMilliSecs == pTimer.mMilliSecs;
}

bool Timer::operator != (const Timer& pTimer) const
{
	return mMilliSecs != pTimer.mMilliSecs;
}

bool Timer::operator <= (const Timer& pTimer) const
{
	return mMilliSecs <= pTimer.mMilliSecs;
}

bool Timer::operator >= (const Timer& pTimer) const
{
	return mMilliSecs >= pTimer.mMilliSecs;
}

Timer& Timer::operator += (const Timer& pTimer)
{
	mMilliSecs += pTimer.mMilliSecs;
	return *this;
}

Timer& Timer::operator -= (const Timer& pTimer)
{
	mMilliSecs -= pTimer.mMilliSecs;
	return *this;
}

void Timer::SetTime(uint64 pMilliSecs)
{
	mMilliSecs = pMilliSecs;
}



}
