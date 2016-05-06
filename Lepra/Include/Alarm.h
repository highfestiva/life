
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "hirestimer.h"



namespace lepra {



class Alarm {
public:
	Alarm();

	void Reset();
	void Push(double initial_wait_time);
	void Set();
	void SetIfNotSet();
	// Alarm is expired if a) started AND b) time overdue.
	bool IsExpired(double wait_time);
	bool PopExpired(double wait_time);
	double GetTimeDiff() const;

private:
	HiResTimer timer_;
	bool is_active_;
};



}
