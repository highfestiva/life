
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

void Alarm::Set()
{
	mTimer.UpdateTimer();
	mTimer.ClearTimeDiff();
	mIsActive = true;
}

bool Alarm::IsExpired(double pTime)
{
	mTimer.UpdateTimer();
	return (mIsActive && mTimer.GetTimeDiff() >= pTime);
}

bool Alarm::PopExpired(double pTime)
{
	bool lIsExpired = IsExpired(pTime);
	if (lIsExpired)
	{
		Reset();
	}
	return (lIsExpired);
}



}
