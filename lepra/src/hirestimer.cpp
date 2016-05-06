
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/hirestimer.h"
#include "../include/lepraos.h"
#ifdef LEPRA_POSIX
#include <sys/time.h>
#endif // Posix



namespace lepra {



HiResTimer::HiResTimer(bool use_shadow):
	prev_counter_(0),
	counter_(0),
	use_shadow_(use_shadow) {
	UpdateTimer();
	prev_counter_ = counter_;
}

HiResTimer::HiResTimer(uint64 count):
	prev_counter_(count),
	counter_(count),
	use_shadow_(true) {
}

HiResTimer::HiResTimer(const HiResTimer& timer):
	prev_counter_(timer.prev_counter_),
	counter_(timer.counter_),
	use_shadow_(timer.use_shadow_) {
}

HiResTimer::~HiResTimer() {
}

void HiResTimer::EnableShadowCounter(bool enable) {
	use_shadow_ = enable;
}

void HiResTimer::StepCounterShadow() {
	last_system_counter_ = GetSystemCounter();
}

double HiResTimer::PopTimeDiff() {
	double __time = QueryTimeDiff();
	ClearTimeDiff();
	return (__time);
}

double HiResTimer::QueryTimeDiff() {
	UpdateTimer();
	return (GetTimeDiff());
}

double HiResTimer::GetTime() const {
	return (counter_*period_);
}

int64 HiResTimer::GetCounter() const {
	return (counter_);
}

void HiResTimer::UpdateTimer() {
	counter_ = GetSystemCounterShadow();
}

void HiResTimer::ClearTimeDiff() {
	prev_counter_ = counter_;
}

void HiResTimer::ReduceTimeDiff(double seconds) {
	prev_counter_ += (int64)(seconds * (double)frequency_);
}

double HiResTimer::GetTimeDiff() const {
	return (((int64)(counter_ - prev_counter_)) * period_);
}

int64 HiResTimer::GetCounterDiff() const {
	return (counter_ - prev_counter_);
}

const HiResTimer& HiResTimer::operator= (const HiResTimer& timer) {
	prev_counter_ = timer.prev_counter_;
	counter_ = timer.counter_;
	frequency_ = timer.frequency_;
	period_ = timer.period_;
	return (timer);
}

bool HiResTimer::operator <  (const HiResTimer& timer) const {
	return (counter_ < timer.counter_);
}

bool HiResTimer::operator >  (const HiResTimer& timer) const {
	return (counter_ > timer.counter_);
}

bool HiResTimer::operator == (const HiResTimer& timer) const {
	return (counter_ == timer.counter_);
}

bool HiResTimer::operator != (const HiResTimer& timer) const {
	return (counter_ != timer.counter_);
}

bool HiResTimer::operator <= (const HiResTimer& timer) const {
	return (counter_ <= timer.counter_);
}

bool HiResTimer::operator >= (const HiResTimer& timer) const {
	return (counter_ >= timer.counter_);
}

HiResTimer& HiResTimer::operator += (const HiResTimer& timer) {
	counter_ += timer.counter_;
	return (*this);
}

HiResTimer& HiResTimer::operator -= (const HiResTimer& timer) {
	counter_ -= timer.counter_;
	return (*this);
}



uint64 HiResTimer::GetSystemCounterShadow() {
	if (!use_shadow_) {
		return GetSystemCounter();
	}
	return last_system_counter_;
}



void HiResTimer::InitFrequency() {
#if defined(LEPRA_WINDOWS)
	LARGE_INTEGER frequency;
	::QueryPerformanceFrequency(&frequency);
	frequency_ = (int64)frequency.QuadPart;
#elif defined(LEPRA_MAC)
	frequency_ = 1000000;
#elif defined(LEPRA_POSIX)
	frequency_ = 1000000000;
#else // <Unknown target>
#error HiResTimer::InitFrequency() not implemented on this platform!
#endif // LEPRA_WINDOWS/LEPRA_POSIX/<Unknown target>
	period_ = 1.0/frequency_;
}

int64 HiResTimer::GetFrequency() {
	return (frequency_);
}

double HiResTimer::GetPeriod() {
	return (period_);
}

uint64 HiResTimer::GetSystemCounter() {
#if defined(LEPRA_WINDOWS)
	LARGE_INTEGER time_counter;
	::QueryPerformanceCounter(&time_counter);
	return ((uint64)time_counter.QuadPart);
#elif defined(LEPRA_MAC)
        timeval time_value;
        ::gettimeofday(&time_value, 0);
	return ((uint64)time_value.tv_sec*1000000 + time_value.tv_usec);
#elif defined(LEPRA_POSIX)
	timespec time_value;
	::clock_gettime(CLOCK_REALTIME, &time_value);
	return ((uint64)time_value.tv_sec*1000000000+time_value.tv_nsec);
#else // <Unknown target>
#error HiResTimer::UpdateCounter() not implemented on this platform!
#endif // LEPRA_WINDOWS/LEPRA_POSIX/<Unknown target>
}



StopWatch::StopWatch():
	Parent((uint64)0),
	is_started_(false),
	start_count_(0) {
}

bool StopWatch::TryStart() {
	if (is_started_) {
		return false;
	}
	Start();
	return true;
}

void StopWatch::Start() {
	PopTimeDiff();
	is_started_ = true;
	++start_count_;
}

bool StopWatch::ResumeFromLapTime() {
	if (is_started_) {
		return false;
	}
	is_started_ = true;
	++start_count_;
	return true;
}

bool StopWatch::ResumeFromStop() {
	if (is_started_) {
		return false;
	}
	const uint64 counter = counter_;
	UpdateTimer();
	prev_counter_ += counter_ - counter;
	is_started_ = true;
	++start_count_;
	return true;
}

void StopWatch::Stop() {
	is_started_ = false;
}

bool StopWatch::IsStarted() const {
	return is_started_;
}

double StopWatch::QuerySplitTime() {
	return is_started_? QueryTimeDiff() : GetTimeDiff();
}

int StopWatch::GetStartCount() const {
	return start_count_;
}



uint64 HiResTimer::frequency_ = 0;
double HiResTimer::period_ = 0;
uint64 HiResTimer::last_system_counter_ = 0;



}
