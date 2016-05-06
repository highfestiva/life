
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/alarm.h"



namespace lepra {



Alarm::Alarm() {
	Reset();
}

void Alarm::Reset() {
	is_active_ = false;
}

void Alarm::Push(double initial_wait_time) {
	if (!is_active_) {
		Set();
		timer_.ReduceTimeDiff(initial_wait_time);
	}
}

void Alarm::Set() {
	timer_.PopTimeDiff();
	is_active_ = true;
}

void Alarm::SetIfNotSet() {
	if (!is_active_) {
		Set();
	}
}

bool Alarm::IsExpired(double wait_time) {
	timer_.UpdateTimer();
	return (is_active_ && timer_.GetTimeDiff() >= wait_time);
}

bool Alarm::PopExpired(double wait_time) {
	bool is_expired = IsExpired(wait_time);
	if (is_expired) {
		Reset();
	}
	return (is_expired);
}

double Alarm::GetTimeDiff() const {
	return timer_.GetTimeDiff();
}



}
