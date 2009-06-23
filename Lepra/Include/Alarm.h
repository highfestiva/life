
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "HiResTimer.h"



namespace Lepra
{



class Alarm
{
public:
	Alarm();

	void Reset();
	void Set();
	// Alarm is expired if a) started AND b) time overdue.
	bool IsExpired(double pTime);
	bool PopExpired(double pTime);

private:
	HiResTimer mTimer;
	bool mIsActive;
};



}
