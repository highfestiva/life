
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "hirestimer.h"



namespace lepra {



class TimeLogger {
public:
	TimeLogger(LogDecorator* log, const str& prefix);
	~TimeLogger();
	void Transfer(const str& prefix);
	void Log();

private:
	LogDecorator* log_;
	str prefix_;
	HiResTimer timer_;
};



}
