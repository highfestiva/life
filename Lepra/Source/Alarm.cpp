
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
	mTimer.PopTimeDiff();
	mIsActive = true;
}

void Alarm::SetIfNotSet()
{
	if (!mIsActive)
	{
		Set();
	}
}

bool Alarm::IsExpired(double pWaitTime)
{
	mTimer.UpdateTimer();
	return (mIsActive && mTimer.GetTimeDiff() >= pWaitTime);
}

bool Alarm::PopExpired(double pWaitTime)
{
	bool lIsExpired = IsExpired(pWaitTime);
	if (lIsExpired)
	{
		Reset();
	}
	return (lIsExpired);
}

double Alarm::GetTimeDiff() const
{
	return mTimer.GetTimeDiff();
}



}
