

// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/timelogger.h"



namespace lepra {



TimeLogger::TimeLogger(LogDecorator* log, const str& prefix):
	log_(log),
	prefix_(prefix),
	timer_(false) {
}

TimeLogger::~TimeLogger() {
	Log();
}

void TimeLogger::Transfer(const str& prefix) {
	Log();
	prefix_ = prefix;
}

void TimeLogger::Log() {
	log_->Infof((prefix_+": %g").c_str(), timer_.PopTimeDiff());
}



}
