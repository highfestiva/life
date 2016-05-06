
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// This is the heart of the physics and network physics and also the client prediction.
// The time manager is responsible for the game time and for the physics steps and their
// lengths.



#pragma once

#include "../../lepra/include/hirestimer.h"
#include "cure.h"



namespace cure {



class TimeManager {
public:
	TimeManager();
	virtual ~TimeManager();

	void Clear(int physics_frame_counter);
	// Increases time modulo m by the elapsed time and calculates the corresponding number
	// of physics steps s, s >= 0.
	// If s > 0 this method increases the physics frame counter, while at the same time
	// reducing m so that only the remainder is left (m -= s*fps). Also adjusts m for the
	// increase or decrease in physics simulation speed.
	void Tick();
	// Returns the actual (calendar) current time since this instance was created.
	// TRICKY: absolute time wraps - don't use if you don't know what you're doing. Be sure to use
	// GetAbsoluteTimeDiff() when checking if certain time elapsed!
	float GetAbsoluteTime(float offset = 0) const;
	// Returns the actual (calendar) time diff between end and start (handles time wrap case).
	static float GetAbsoluteTimeDiff(float end, float start);
	// Returns the actual (calendar) current time difference between this and the previous
	// time tick.
	float GetCurrentFrameTime() const;
	// Returns the current physics frame (counter). This is increased by ticking physics.
	// TRICKY: physics frames wraps - don't use if you don't know what you're doing. Be sure to use
	// GetPhysicsFrameDiff() when checking if certain time elapsed!
	int GetCurrentPhysicsFrame() const;
	// Returns the current physics frame (counter) to expect in x seconds.
	int GetCurrentPhysicsFrameAddFrames(int frames) const;
	// Returns the current physics frame (counter) to expect in x seconds.
	int GetCurrentPhysicsFrameAddSeconds(float seconds) const;
	// Adds two physical frame (counters) together (handles time wrap case).
	int GetPhysicsFrameAddFrames(int frame_counter, int frames) const;
	// See GetPhysicsFrameDelta() for more info (operates on current frame counter as end).
	int GetCurrentPhysicsFrameDelta(int start) const;
	// Returns the physics frame (counter) delta between end and start (handles time wrap case). The returned value
	// is the number of frames between end and start, not a new frame - use GetPhysicsFrameAdd() for that.
	int GetPhysicsFrameDelta(int end, int start) const;
	// Returns the frame time we normally reach on this system (filtered, sliding average frame time, includes real time ratio).
	float GetNormalFrameTime() const;
	// Returns the game frame time we normally reach on this system (filtered, sliding average frame time, includes real time ratio and halted physics).
	float GetNormalGameFrameTime() const;
	// Returns the frame time we normally reach on this system IRL, i.e. not counting real time ratio.
	float GetRealNormalFrameTime() const;
	// Returns the physics:real time ratio.
	float GetRealTimeRatio() const;
	// Setting the current physics frame is only done on the client during login, all other times
	// are set using adjustment rather than direct setting.
	void SetCurrentPhysicsFrame(int physics_frame);
	// Returns how many "actual" physics steps a we can take this frame without ending up lagged behind. a >= 1.
	int GetAffordedPhysicsStepCount() const;
	// Returns how many physics micro-steps we should take this frame to avoid lag.
	int GetAffordedPhysicsMicroStepCount() const;
	// Return how long each physics step should be if we want to progress total time of s in a steps.
	float GetAffordedPhysicsStepTime() const;
	// Return how long each physics step should be if we want to progress total time of s in a steps.
	float GetAffordedPhysicsTotalTime() const;
	// Returns the desired physics kFps in micro steps.
	int GetDesiredMicroSteps() const;
	// Returns how much quicker the tick loop should be in order to achieve perfect loop time.
	// A negative value means the tick loop should be that much longer.
	float GetTickLoopTimeReduction() const;

	// Temporarily sets a physics speed adjustmed, which is active for a number of
	// physics frames. When the speed has been adjusted by a certain time, it goes back to
	// a 1:1 physics simulation speed. (Normally this speed change only happens on clients
	// to compensate for network latencies and jitter in our client prediction model.)
	void SetPhysicsSpeedAdjustment(float time, int pFrameCount);

	int ConvertSecondsToPhysicsFrames(float seconds) const;
	float ConvertPhysicsFramesToSeconds(int steps) const;

private:
	int target_frame_rate_;	// Physics frame rate (at which the physics system is updated).
	HiResTimer time_;	// Absolute time.
	float absolute_time_;	// Cache of absolute time.
	float current_frame_time_;	// Time diff between current frame and previous.
	float tick_time_modulo_;	// Contains the time that was "left over" since last physics step.
	float tick_time_overhead_;	// How much slower the call frequency is compared to what we would want.
	float real_time_ratio_;
	float game_real_time_ratio_;
	float physics_speed_adjustment_time_;	// Total number of seconds that our our physics time needs adjusting.
	int physics_speed_adjustment_frame_count_;	// The number of physics steps to adjust our physics time over.
	int physics_frame_counter_;	// Holds the index of the current physics frame.
	float average_frame_time_;	// The sliding average step length.
	float physics_frame_time_;	// How much time we should move forward each physics step.
	int physics_step_count_;

	logclass();
};



}
