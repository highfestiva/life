
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "HiResTimer.h"



namespace Lepra
{



class TimeLogger
{
public:
	TimeLogger(LogDecorator* pLog, const str& pPrefix);
	~TimeLogger();
	void Transfer(const str& pPrefix);
	void Log();

private:
	LogDecorator* mLog;
	str mPrefix;
	HiResTimer mTimer;
};



}
