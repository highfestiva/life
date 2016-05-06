
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// A time class that counts milliseconds and seconds.

// Each time you call UpdateTimer(), the time difference
// since the last clear will be updated. You can clear or
// reduce this time difference by calling ClearTimeDiff()
// or ReduceTimeDiff().



#pragma once

#include "pch.h"
#include "../include/timer.h"
#include "../include/lepraos.h"



namespace lepra {



Timer::Timer() :
	milli_secs_(0),
	prev_milli_secs_(0) {
	UpdateTimer();
	ClearTimeDiff();
}

Timer::Timer(uint64 milli_secs) :
	milli_secs_(milli_secs),
	prev_milli_secs_(milli_secs) {
}

Timer::Timer(const Timer& timer) :
	milli_secs_(timer.milli_secs_),
	prev_milli_secs_(timer.prev_milli_secs_) {
}

Timer::~Timer() {
}

double Timer::PopTimeDiff() {
	double time = QueryTimeDiff();
	ClearTimeDiff();
	return (time);
}

double Timer::QueryTimeDiff() {
	UpdateTimer();
	return (GetTimeDiff());
}

double Timer::GetTime() {
	return (double)milli_secs_ * (1.0 / 1000.0);
}

void Timer::UpdateTimer() {
#ifdef LEPRA_WINDOWS
	SetTime(::GetTickCount());
#elif defined(LEPRA_POSIX)
	// Almost the same implementation as HiResTimer.
	timeval time_value;
	::gettimeofday(&time_value, 0);
	SetTime(((uint64)time_value.tv_sec * 1000 + time_value.tv_usec / 1000));
#else // <Unknown target>
#error Timer::UpdateTimer() not implemented on this platform!
#endif // LEPRA_WINDOWS/LEPRA_POSIX/<Unknown target>
}

double Timer::GetTimeDiff() const {
	return ((double)(milli_secs_ - prev_milli_secs_) * (1.0 / 1000.0));
}

void Timer::ClearTimeDiff() {
	prev_milli_secs_ = milli_secs_;
}

void Timer::ReduceTimeDiff(uint64 milli_secs) {
	prev_milli_secs_ += milli_secs;

	if (prev_milli_secs_ > milli_secs_) {
		prev_milli_secs_ = milli_secs_;
	}
}

void Timer::ReduceTimeDiff(double secs) {
	prev_milli_secs_ += (uint64)(secs * 1000.0);

	if (prev_milli_secs_ > milli_secs_) {
		prev_milli_secs_ = milli_secs_;
	}
}

const Timer& Timer::operator = (const Timer& timer) {
	milli_secs_ = timer.milli_secs_;
	prev_milli_secs_ = timer.prev_milli_secs_;
	return *this;
}

bool Timer::operator <  (const Timer& timer) const {
	return milli_secs_ < timer.milli_secs_;
}

bool Timer::operator >  (const Timer& timer) const {
	return milli_secs_ > timer.milli_secs_;
}

bool Timer::operator == (const Timer& timer) const {
	return milli_secs_ == timer.milli_secs_;
}

bool Timer::operator != (const Timer& timer) const {
	return milli_secs_ != timer.milli_secs_;
}

bool Timer::operator <= (const Timer& timer) const {
	return milli_secs_ <= timer.milli_secs_;
}

bool Timer::operator >= (const Timer& timer) const {
	return milli_secs_ >= timer.milli_secs_;
}

Timer& Timer::operator += (const Timer& timer) {
	milli_secs_ += timer.milli_secs_;
	return *this;
}

Timer& Timer::operator -= (const Timer& timer) {
	milli_secs_ -= timer.milli_secs_;
	return *this;
}

void Timer::SetTime(uint64 milli_secs) {
	milli_secs_ = milli_secs;
}



}
