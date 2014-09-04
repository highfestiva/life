

// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/TimeLogger.h"



namespace Lepra
{



TimeLogger::TimeLogger(LogDecorator* pLog, const str& pPrefix):
	mLog(pLog),
	mPrefix(pPrefix),
	mTimer(false)
{
}

TimeLogger::~TimeLogger()
{
	Log();
}

void TimeLogger::Transfer(const str& pPrefix)
{
	Log();
	mPrefix = pPrefix;
}

void TimeLogger::Log()
{
	mLog->Infof((mPrefix+_T(": %g")).c_str(), mTimer.PopTimeDiff());
}



}
