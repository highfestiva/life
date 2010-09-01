
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

// This is the heart of the physics and network physics and also the client prediction.
// The time manager is responsible for the game time and for the physics steps and their
// lengths.



#pragma once

#include "../../Lepra/Include/HiResTimer.h"
#include "Cure.h"



namespace Cure
{



class TimeManager
{
public:
	TimeManager();
	virtual ~TimeManager();

	void Clear(int pPhysicsFrameCounter);
	// Increases time modulo m by the elapsed time and calculates the corresponding number
	// of physics steps s, s >= 0.
	void TickTime();
	// If s > 0 this method increases the physics frame counter, while at the same time
	// reducing m so that only the remainder is left (m -= s*fps). Also adjusts m for the
	// increase or decrease in physics simulation speed.
	void TickPhysics();
	// Returns the actual (calendar) current time since this instance was created.
	// TRICKY: absolute time wraps - don't use if you don't know what you're doing. Be sure to use
	// GetAbsoluteTimeDiff() when checking if certain time elapsed!
	float GetAbsoluteTime(float pOffset = 0) const;
	// Returns the actual (calendar) time diff between end and start (handles time wrap case).
	static float GetAbsoluteTimeDiff(float pEnd, float pStart);
	// Returns the actual (calendar) current time difference between this and the previous
	// time tick.
	float GetCurrentFrameTime() const;
	// Returns the current physics frame (counter). This is increased by ticking physics.
	// TRICKY: physics frames wraps - don't use if you don't know what you're doing. Be sure to use
	// GetPhysicsFrameDiff() when checking if certain time elapsed!
	int GetCurrentPhysicsFrame() const;
	// Returns the current physics frame (counter) to expect in x seconds.
	int GetCurrentPhysicsFrameAddFrames(int pFrames) const;
	// Returns the current physics frame (counter) to expect in x seconds.
	int GetCurrentPhysicsFrameAddSeconds(float pSeconds) const;
	// Adds two physical frame (counters) together (handles time wrap case).
	int GetPhysicsFrameAddFrames(int pFrameCounter, int pFrames) const;
	// See GetPhysicsFrameDelta() for more info (operates on current frame counter as end).
	int GetCurrentPhysicsFrameDelta(int pStart) const;
	// Returns the physics frame (counter) delta between end and start (handles time wrap case). The returned value
	// is the number of frames between end and start, not a new frame - use GetPhysicsFrameAdd() for that.
	int GetPhysicsFrameDelta(int pEnd, int pStart) const;
	// Returns the frame time we normally reach on this system (filtered, sliding average frame time, includes real time ratio).
	float GetNormalFrameTime() const;
	// Returns the frame time we normally reach on this system IRL, i.e. not counting real time ratio.
	float GetRealNormalFrameTime() const;
	// Returns the physics:real time ratio.
	float GetRealTimeRatio() const;
	// Setting the current physics frame is only done on the client during login, all other times
	// are set using adjustment rather than direct setting.
	void SetCurrentPhysicsFrame(int pPhysicsFrame);
	// Returns how many "actual" physics steps a we can take this frame without ending up lagged behind. a >= 1.
	int GetAffordedPhysicsStepCount() const;
	// Return how long each physics step should be if we want to progress total time of s in a steps.
	float GetAffordedPhysicsStepTime() const;
	// Return how long each physics step should be if we want to progress total time of s in a steps.
	float GetAffordedPhysicsTotalTime() const;
	// Returns the desired physics FPS in micro steps.
	int GetDesiredMicroSteps() const;

	// Temporarily sets a physics speed adjustmed, which is active for a number of
	// physics frames. When the speed has been adjusted by a certain time, it goes back to
	// a 1:1 physics simulation speed. (Normally this speed change only happens on clients
	// to compensate for network latencies and jitter in our client prediction model.)
	void SetPhysicsSpeedAdjustment(float pTime, int pFrameCount);

	int ConvertSecondsToPhysicsFrames(float pSeconds) const;
	float ConvertPhysicsFramesToSeconds(int pSteps) const;

private:
	int mTargetFrameRate;	// Physics frame rate (at which the physics system is updated).
	HiResTimer mTime;	// Absolute time.
	float mAbsoluteTime;	// Cache of absolute time.
	float mCurrentFrameTime;	// Time diff between current frame and previous.
	float mTickTimeModulo;	// Contains the time that was "left over" since last physics step.
	float mRealTimeRatio;
	float mPhysicsSpeedAdjustmentTime;	// Total number of seconds that our our physics time needs adjusting.
	int mPhysicsSpeedAdjustmentFrameCount;	// The number of physics steps to adjust our physics time over.
	int mPhysicsFrameCounter;	// Holds the index of the current physics frame.
	float mAverageFrameTime;	// The sliding average step length.
	float mPhysicsFrameTime;	// How much time we should move forward each physics step.
	int mPhysicsStepCount;
	int mTargetPhysicsStepCount;	// The number of discrete physics steps to perform this frame.
	int mPhysicsFrameWrapLimit;

	LOG_CLASS_DECLARE();
};



}
