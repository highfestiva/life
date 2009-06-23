
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
	void Push(double pInitialWaitTime);
	void Set();
	// Alarm is expired if a) started AND b) time overdue.
	bool IsExpired(double pOptionalWaitTime);
	bool PopExpired(double pOptionalWaitTime);

private:
	HiResTimer mTimer;
	bool mIsActive;
};



}
