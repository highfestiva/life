
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/timemanager.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/gametimer.h"
#include "../../lepra/include/math.h"
#include "../../lepra/include/performance.h"
#include "../include/runtimevariable.h"



namespace cure {



static const int kTimeWrapLimit = 10*60*60;	// Unit is seconds. Anything small is good enough.
static const int kPhysicsFrameWrapLimit = kTimeWrapLimit*kCureStandardFrameRate;



TimeManager::TimeManager():
	target_frame_rate_(kCureStandardFrameRate),
	time_(false) {
	Clear(0);
}

TimeManager::~TimeManager() {
}



void TimeManager::Clear(int physics_frame_counter) {
	time_.PopTimeDiff();
	tick_time_modulo_ = 0;
	physics_speed_adjustment_time_ = 0;
	physics_speed_adjustment_frame_count_ = 0;
	absolute_time_ = 0;
	tick_time_overhead_ = 0;
	real_time_ratio_ = 1.0;
	game_real_time_ratio_ = 1.0f;
	physics_frame_counter_ = physics_frame_counter;
	current_frame_time_ = 1/(float)target_frame_rate_;
	average_frame_time_ = current_frame_time_;
	physics_frame_time_ = average_frame_time_;
	physics_step_count_ = 1;
}

void TimeManager::Tick() {
	v_get(target_frame_rate_, =, cure::GetSettings(), kRtvarPhysicsFps, 2);
	v_get(real_time_ratio_, =(float), cure::GetSettings(), kRtvarPhysicsRtr, 1.0);
	bool physics_halt;
	v_get(physics_halt, =, cure::GetSettings(), kRtvarPhysicsHalt, false);
	game_real_time_ratio_ = physics_halt? 0 : real_time_ratio_;
	GameTimer::SetRealTimeRatio(game_real_time_ratio_);
	bool is_fixed_fps;
	v_get(is_fixed_fps, =, cure::GetSettings(), kRtvarPhysicsIsfixedfps, false);

	current_frame_time_ = (float)time_.PopTimeDiff();
	if (current_frame_time_ > 1.0) {	// Never take longer steps than one second.
		current_frame_time_ = 1.0;
	}
	absolute_time_ += current_frame_time_;
	absolute_time_ = ::fmod(absolute_time_, (float)kTimeWrapLimit);

	if (is_fixed_fps) {
		physics_frame_time_ = real_time_ratio_/target_frame_rate_;
		physics_frame_counter_ = GetCurrentPhysicsFrameAddFrames(1);
		average_frame_time_ = Math::Lerp(average_frame_time_, current_frame_time_, 0.2f);
		tick_time_overhead_ = 0;
		physics_step_count_ = 1;
		return;
	}

	tick_time_modulo_ += current_frame_time_;

	const float target_period = 1/(float)target_frame_rate_;
	physics_frame_time_ = target_period;
	while (physics_frame_time_*2 < average_frame_time_) {	// Half framerate if we're on a slow platform (multiplayer over network), reiterate.
		physics_frame_time_ *= 2;
		log_debug("Halfing physics frame rate!");
	}
	physics_step_count_ = (int)::floor(tick_time_modulo_ / physics_frame_time_);
	physics_frame_time_ *= real_time_ratio_;

	/*static int c = 0;
	if (++c >= 11) {
		log_.Infof("Instance %p: tick time modulo is %.0f %%.", this, tick_time_modulo_*100/target_period);
		c = 0;
	}*/

	if (physics_step_count_ > 0) {
		average_frame_time_ = Math::Lerp(average_frame_time_, current_frame_time_, 0.01f);

		const float this_step_time = GetAffordedPhysicsTotalTime() / real_time_ratio_;
		int target_step_count = (int)::floorf(this_step_time * target_frame_rate_);

		physics_frame_counter_ = GetCurrentPhysicsFrameAddFrames(target_step_count);
		//physics_frame_counter_ += target_step_count;
		tick_time_modulo_ -= this_step_time;

		if (physics_speed_adjustment_frame_count_ > 0) {
			if (target_step_count > physics_speed_adjustment_frame_count_) {
				target_step_count = physics_speed_adjustment_frame_count_;
			}
			tick_time_modulo_ += physics_speed_adjustment_time_*target_step_count / target_frame_rate_;
			physics_speed_adjustment_frame_count_ -= target_step_count;
		} else {
			physics_speed_adjustment_time_ = 0;
		}
	}
	/*if (c == 0) {
		log_.Infof("Instance %p: modulo offset is %.0f %%.", this, lModuloOffset*100/target_period);
	}*/
	const float desired_modulo_offset = 0.4f;
	/*const float current_target_overhead = tick_time_modulo_ - desired_modulo_offset*target_period;
	tick_time_overhead_ = Math::Lerp(tick_time_overhead_, 2 * current_target_overhead, 0.5f);*/
	const float current_target_overhead = 1.6f*tick_time_modulo_ - desired_modulo_offset*target_period;
	tick_time_overhead_ = Math::Lerp(tick_time_overhead_, current_target_overhead, 0.6f);
	//log_volatile(log_.Debugf("AverageFrameTime %f s, modulo placement %f %%, reduction %f %%, target_period %f s.", average_frame_time_, tick_time_modulo_*100/target_period, tick_time_overhead_*100/target_period, target_period));
}

float TimeManager::GetAbsoluteTime(float offset) const {
	if (offset) return ::fmod(absolute_time_+offset, (float)kTimeWrapLimit);
	return (absolute_time_);
}

float TimeManager::GetAbsoluteTimeDiff(float end, float start) {
	float diff = end - start;
	if (diff < 0) {
		diff += kTimeWrapLimit;
	}
	return diff;
}

float TimeManager::GetCurrentFrameTime() const {
	return (current_frame_time_);
}

int TimeManager::GetCurrentPhysicsFrame() const {
	return physics_frame_counter_;
}

int TimeManager::GetCurrentPhysicsFrameAddFrames(int frames) const {
	return GetPhysicsFrameAddFrames(physics_frame_counter_, frames);
}

int TimeManager::GetCurrentPhysicsFrameAddSeconds(float seconds) const {
	deb_assert(seconds <= 10*60);
	return GetCurrentPhysicsFrameAddFrames(ConvertSecondsToPhysicsFrames(seconds));
}

int TimeManager::GetPhysicsFrameAddFrames(int frame_counter, int frames) const {
	deb_assert(frames > -kPhysicsFrameWrapLimit/2 &&
		frames < kPhysicsFrameWrapLimit/2);

	int new_frame = frame_counter + frames;
	if (new_frame < 0) {
		new_frame += kPhysicsFrameWrapLimit;
	} else if (new_frame >= kPhysicsFrameWrapLimit) {
		new_frame -= kPhysicsFrameWrapLimit;
	}

	deb_assert(new_frame >= 0 && new_frame < kPhysicsFrameWrapLimit);
	return new_frame;
}

int TimeManager::GetCurrentPhysicsFrameDelta(int start) const {
	return GetPhysicsFrameDelta(physics_frame_counter_, start);
}

int TimeManager::GetPhysicsFrameDelta(int end, int start) const {
	if (start >= kPhysicsFrameWrapLimit || end >= kPhysicsFrameWrapLimit) {
		deb_assert(false);
		return -1;
	}
	deb_assert(end >= 0);
	deb_assert(start >= 0);

	int diff = end - start;
	if (diff > kPhysicsFrameWrapLimit/2) {
		diff -= kPhysicsFrameWrapLimit;
	} else if (diff < -kPhysicsFrameWrapLimit/2) {
		diff += kPhysicsFrameWrapLimit;
	}

	deb_assert(diff >= -kPhysicsFrameWrapLimit/2 && diff <= kPhysicsFrameWrapLimit/2);
	return diff;
}

float TimeManager::GetNormalFrameTime() const {
	return average_frame_time_ * real_time_ratio_;
}

float TimeManager::GetNormalGameFrameTime() const {
	return average_frame_time_ * game_real_time_ratio_;
}

float TimeManager::GetRealNormalFrameTime() const {
	return average_frame_time_;
}

float TimeManager::GetRealTimeRatio() const {
	return real_time_ratio_;
}

void TimeManager::SetCurrentPhysicsFrame(int physics_frame) {
	physics_frame_counter_ = physics_frame;
}

int TimeManager::GetAffordedPhysicsStepCount() const {
	return (physics_step_count_);
}

int TimeManager::GetAffordedPhysicsMicroStepCount() const {
	int micro_steps;
	v_get(micro_steps, =, cure::GetSettings(), kRtvarPhysicsMicrosteps, 1);
	float slowdown = target_frame_rate_ * average_frame_time_ * 0.5f;
	if (slowdown < 1) {
		slowdown = 1;
	}
	micro_steps = (int)(micro_steps / slowdown);
	if (micro_steps < 1) {
		micro_steps = 1;
	}
	return micro_steps;
}

float TimeManager::GetAffordedPhysicsStepTime() const {
	return (physics_frame_time_);
}

float TimeManager::GetAffordedPhysicsTotalTime() const {
	return (GetAffordedPhysicsStepCount() * GetAffordedPhysicsStepTime());
}

int TimeManager::GetDesiredMicroSteps() const {
	int micro_steps;
	v_get(micro_steps, =, cure::GetSettings(), kRtvarPhysicsMicrosteps, 1);
	return (target_frame_rate_ * micro_steps);
}

float TimeManager::GetTickLoopTimeReduction() const {
	return tick_time_overhead_;
}



void TimeManager::SetPhysicsSpeedAdjustment(float time, int pFrameCount) {
	physics_speed_adjustment_time_ = time;
	physics_speed_adjustment_frame_count_ = pFrameCount;
}



int TimeManager::ConvertSecondsToPhysicsFrames(float seconds) const {
	return ((int)(seconds*target_frame_rate_));
}

float TimeManager::ConvertPhysicsFramesToSeconds(int steps) const {
	return (steps/(float)target_frame_rate_);
}



loginstance(kPhysics, TimeManager);



}
