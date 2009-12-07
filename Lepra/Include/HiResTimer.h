
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "Lepra.h"
#ifdef LEPRA_POSIX
#include <sys/time.h>
#endif // Posix



namespace Lepra
{



class HiResTimer
{
public:
	inline HiResTimer();
	inline HiResTimer(uint64 pCount);
	inline HiResTimer(const HiResTimer& pTimer);
	inline ~HiResTimer();

	// Updates time, returns current time diff and resets time diff.
	inline double PopTimeDiff();

	// Returns the time in seconds.
	inline double GetTime() const;

	// Returns the actual counter value.
	inline int64 GetCounter() const;

	// Updates to the current time. 
	// Does not update the previous time.
	inline void UpdateTimer();

	// Sets the previous time to the same value as
	// the current time.
	inline void ClearTimeDiff();
	inline void ReduceTimeDiff(double pSecsonds);

	// Returns the time difference between the current time
	// and the previous time.
	inline double GetTimeDiff() const;

	inline int64 GetCounterDiff() const;

	// Operators.
	inline const HiResTimer& operator = (const HiResTimer& pTimer);

	inline bool operator <  (const HiResTimer& pTimer) const;
	inline bool operator >  (const HiResTimer& pTimer) const;
	inline bool operator == (const HiResTimer& pTimer) const;
	inline bool operator != (const HiResTimer& pTimer) const;
	inline bool operator <= (const HiResTimer& pTimer) const;
	inline bool operator >= (const HiResTimer& pTimer) const;

	inline HiResTimer& operator += (const HiResTimer& pTimer);
	inline HiResTimer& operator -= (const HiResTimer& pTimer);

	static inline void InitFrequency();
	static inline int64 GetFrequency();
	static inline double GetPeriod();
	static inline uint64 GetSystemCounter();

private:
	uint64 mPrevCounter;
	uint64 mCounter;

	static uint64 mFrequency;
	static double mPeriod;
};

HiResTimer::HiResTimer() :
	mPrevCounter(0),
	mCounter(0)
{
	UpdateTimer();
	mPrevCounter = mCounter;
}

HiResTimer::HiResTimer(uint64 pCount) :
	mPrevCounter(pCount),
	mCounter(pCount)
{
}

HiResTimer::HiResTimer(const HiResTimer& pTimer) :
	mPrevCounter(pTimer.mPrevCounter),
	mCounter(pTimer.mCounter)
{
}

HiResTimer::~HiResTimer()
{
}

double HiResTimer::PopTimeDiff()
{
	UpdateTimer();
	double lTime = GetTimeDiff();
	ClearTimeDiff();
	return (lTime);
}

double HiResTimer::GetTime() const
{
	return (mCounter*mPeriod);
}

int64 HiResTimer::GetCounter() const
{
	return (mCounter);
}

void HiResTimer::UpdateTimer()
{
	mCounter = GetSystemCounter();
}

void HiResTimer::ClearTimeDiff()
{
	mPrevCounter = mCounter;
}

void HiResTimer::ReduceTimeDiff(double pSeconds)
{
	mPrevCounter += (int64)(pSeconds * (double)mFrequency);
}

double HiResTimer::GetTimeDiff() const
{
	return (((int64)(mCounter - mPrevCounter)) * mPeriod);
}

int64 HiResTimer::GetCounterDiff() const
{
	return (mCounter - mPrevCounter);
}

const HiResTimer& HiResTimer::operator= (const HiResTimer& pTimer)
{
	mPrevCounter = pTimer.mPrevCounter;
	mCounter = pTimer.mCounter;
	mFrequency = pTimer.mFrequency;
	mPeriod = pTimer.mPeriod;
	return (pTimer);
}

bool HiResTimer::operator <  (const HiResTimer& pTimer) const
{
	return (mCounter < pTimer.mCounter);
}

bool HiResTimer::operator >  (const HiResTimer& pTimer) const
{
	return (mCounter > pTimer.mCounter);
}

bool HiResTimer::operator == (const HiResTimer& pTimer) const
{
	return (mCounter == pTimer.mCounter);
}

bool HiResTimer::operator != (const HiResTimer& pTimer) const
{
	return (mCounter != pTimer.mCounter);
}

bool HiResTimer::operator <= (const HiResTimer& pTimer) const
{
	return (mCounter <= pTimer.mCounter);
}

bool HiResTimer::operator >= (const HiResTimer& pTimer) const
{
	return (mCounter >= pTimer.mCounter);
}

HiResTimer& HiResTimer::operator += (const HiResTimer& pTimer)
{
	mCounter += pTimer.mCounter;
	return (*this);
}

HiResTimer& HiResTimer::operator -= (const HiResTimer& pTimer)
{
	mCounter -= pTimer.mCounter;
	return (*this);
}



void HiResTimer::InitFrequency()
{
#if defined(LEPRA_WINDOWS)
	LARGE_INTEGER lFrequency;
	::QueryPerformanceFrequency(&lFrequency);
	mFrequency = (int64)lFrequency.QuadPart;
#elif defined(LEPRA_POSIX)
	mFrequency = 1000000000;
#else // <Unknown target>
#error HiResTimer::InitFrequency() not implemented on this platform!
#endif // LEPRA_WINDOWS/LEPRA_POSIX/<Unknown target>
	mPeriod = 1.0/mFrequency;
}

int64 HiResTimer::GetFrequency()
{
	return (mFrequency);
}

double HiResTimer::GetPeriod()
{
	return (mPeriod);
}

uint64 HiResTimer::GetSystemCounter()
{
#if defined(LEPRA_WINDOWS)
	LARGE_INTEGER lTimeCounter;
	::QueryPerformanceCounter(&lTimeCounter);
	return ((uint64)lTimeCounter.QuadPart);
#elif defined(LEPRA_POSIX)
	timespec lTimeValue;
	::clock_gettime(CLOCK_REALTIME, &lTimeValue);
	return ((uint64)lTimeValue.tv_sec*1000000000+lTimeValue.tv_nsec);
#else // <Unknown target>
#error HiResTimer::UpdateCounter() not implemented on this platform!
#endif // LEPRA_WINDOWS/LEPRA_POSIX/<Unknown target>
}



}
