
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/Alarm.h"



namespace Lepra
{



Alarm::Alarm()
{
	Reset();
}

void Alarm::Reset()
{
	mIsActive = false;
}

void Alarm::Push(double pInitialWaitTime)
{
	if (!mIsActive)
	{
		Set();
		mTimer.ReduceTimeDiff(pInitialWaitTime);
	}
}

void Alarm::Set()
{
	mTimer.UpdateTimer();
	mTimer.ClearTimeDiff();
	mIsActive = true;
}

bool Alarm::IsExpired(double pOptionalWaitTime)
{
	mTimer.UpdateTimer();
	return (mIsActive && mTimer.GetTimeDiff() >= pOptionalWaitTime);
}

bool Alarm::PopExpired(double pOptionalWaitTime)
{
	bool lIsExpired = IsExpired(pOptionalWaitTime);
	if (lIsExpired)
	{
		Reset();
	}
	return (lIsExpired);
}



}
