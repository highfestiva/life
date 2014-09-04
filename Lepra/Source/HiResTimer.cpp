
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/HiResTimer.h"
#include "../Include/LepraOS.h"
#ifdef LEPRA_POSIX
#include <sys/time.h>
#endif // Posix



namespace Lepra
{



HiResTimer::HiResTimer(bool pUseShadow):
	mPrevCounter(0),
	mCounter(0),
	mUseShadow(pUseShadow)
{
	UpdateTimer();
	mPrevCounter = mCounter;
}

HiResTimer::HiResTimer(uint64 pCount):
	mPrevCounter(pCount),
	mCounter(pCount),
	mUseShadow(true)
{
}

HiResTimer::HiResTimer(const HiResTimer& pTimer):
	mPrevCounter(pTimer.mPrevCounter),
	mCounter(pTimer.mCounter),
	mUseShadow(pTimer.mUseShadow)
{
}

HiResTimer::~HiResTimer()
{
}

void HiResTimer::EnableShadowCounter(bool pEnable)
{
	mUseShadow = pEnable;
}

void HiResTimer::StepCounterShadow()
{
	mLastSystemCounter = GetSystemCounter();
}

double HiResTimer::PopTimeDiff()
{
	double lTime = QueryTimeDiff();
	ClearTimeDiff();
	return (lTime);
}

double HiResTimer::QueryTimeDiff()
{
	UpdateTimer();
	return (GetTimeDiff());
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
	mCounter = GetSystemCounterShadow();
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



uint64 HiResTimer::GetSystemCounterShadow()
{
	if (!mUseShadow)
	{
		return GetSystemCounter();
	}
	return mLastSystemCounter;
}



void HiResTimer::InitFrequency()
{
#if defined(LEPRA_WINDOWS)
	LARGE_INTEGER lFrequency;
	::QueryPerformanceFrequency(&lFrequency);
	mFrequency = (int64)lFrequency.QuadPart;
#elif defined(LEPRA_MAC)
	mFrequency = 1000000;
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
#elif defined(LEPRA_MAC)
        timeval lTimeValue;
        ::gettimeofday(&lTimeValue, 0);
	return ((uint64)lTimeValue.tv_sec*1000000 + lTimeValue.tv_usec);
#elif defined(LEPRA_POSIX)
	timespec lTimeValue;
	::clock_gettime(CLOCK_REALTIME, &lTimeValue);
	return ((uint64)lTimeValue.tv_sec*1000000000+lTimeValue.tv_nsec);
#else // <Unknown target>
#error HiResTimer::UpdateCounter() not implemented on this platform!
#endif // LEPRA_WINDOWS/LEPRA_POSIX/<Unknown target>
}



StopWatch::StopWatch():
	Parent((uint64)0),
	mIsStarted(false),
	mStartCount(0)
{
}

bool StopWatch::TryStart()
{
	if (mIsStarted)
	{
		return false;
	}
	Start();
	return true;
}

void StopWatch::Start()
{
	PopTimeDiff();
	mIsStarted = true;
	++mStartCount;
}

bool StopWatch::ResumeFromLapTime()
{
	if (mIsStarted)
	{
		return false;
	}
	mIsStarted = true;
	++mStartCount;
	return true;
}

bool StopWatch::ResumeFromStop()
{
	if (mIsStarted)
	{
		return false;
	}
	const uint64 lCounter = mCounter;
	UpdateTimer();
	mPrevCounter += mCounter - lCounter;
	mIsStarted = true;
	++mStartCount;
	return true;
}

void StopWatch::Stop()
{
	mIsStarted = false;
}

bool StopWatch::IsStarted() const
{
	return mIsStarted;
}

double StopWatch::QuerySplitTime()
{
	return mIsStarted? QueryTimeDiff() : GetTimeDiff();
}

int StopWatch::GetStartCount() const
{
	return mStartCount;
}



uint64 HiResTimer::mFrequency = 0;
double HiResTimer::mPeriod = 0;
uint64 HiResTimer::mLastSystemCounter = 0;



}
