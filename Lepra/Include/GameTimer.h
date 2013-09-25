
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "HiResTimer.h"



namespace Lepra
{



class GameTimer
{
public:
	inline GameTimer();

	inline double PopTimeDiff();
	// May only be used once per frame, or couting will be off!
	inline double QueryTimeDiff();

	inline bool TryStart();
	inline void Start(double pOffset = 0);
	inline bool ResumeFromLapTime();
	inline bool ResumeFromStop();
	inline void Stop();
	inline bool IsStarted() const;
	// May only be used once per frame, or couting will be off!
	inline double QuerySplitTime();
	inline int GetStartCount() const;

	inline static void SetRealTimeRatio(double pRealTimeRatio);

protected:
	StopWatch mTimer;
	double mGameTime;
	static double mRealTimeRatio;
};

GameTimer::GameTimer():
	mGameTime(0)
{
}

double GameTimer::PopTimeDiff()
{
	double lGameTime = mGameTime + mTimer.PopTimeDiff()*mRealTimeRatio;
	mGameTime = 0;
	return lGameTime;
}

double GameTimer::QueryTimeDiff()
{
	mGameTime += mTimer.PopTimeDiff()*mRealTimeRatio;
	return mGameTime;
}

bool GameTimer::TryStart()
{
	if (mTimer.TryStart())
	{
		mGameTime = 0;
		return true;
	}
	return false;
}

void GameTimer::Start(double pOffset)
{
	mTimer.Start();
	mGameTime = pOffset;
}

bool GameTimer::ResumeFromLapTime()
{
	return mTimer.ResumeFromLapTime();
}

bool GameTimer::ResumeFromStop()
{
	return mTimer.ResumeFromStop();
}

void GameTimer::Stop()
{
	mTimer.Stop();
}

bool GameTimer::IsStarted() const
{
	return mTimer.IsStarted();
}

double GameTimer::QuerySplitTime()
{
	return IsStarted()? QueryTimeDiff() : mGameTime;
}

int GameTimer::GetStartCount() const
{
	return mTimer.GetStartCount();
}

void GameTimer::SetRealTimeRatio(double pRealTimeRatio)
{
	mRealTimeRatio = pRealTimeRatio;
}



}
