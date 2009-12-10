
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games

// This is the heart of the physics and network physics and also the client prediction.
// The time manager is responsible for the game time and for the physics steps and their
// lengths.



#pragma once

#include "../../Lepra/Include/HiResTimer.h"



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
	float GetAbsoluteTime() const;
	// Returns the actual (calendar) current time difference between this and the previous
	// time tick.
	float GetCurrentFrameTime() const;
	// Returns the current physics frame (counter). This is increased by ticking physics.
	int GetCurrentPhysicsFrame() const;
	// Returns the frame time we normally reach on this system (filtered, sliding average frame time).
	float GetNormalFrameTime() const;
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
	Lepra::HiResTimer mTime;	// Absolute time.
	float mAbsoluteTime;	// Cache of absolute time.
	float mTickTimeModulo;	// Contains the time that was "left over" since last physics step.
	float mPhysicsSpeedAdjustmentTime;	// Total number of seconds that our our physics time needs adjusting.
	int mPhysicsSpeedAdjustmentFrameCount;	// The number of physics steps to adjust our physics time over.
	int mPhysicsFrameCounter;	// Holds the index of the current physics frame.
	float mAverageFrameTime;	// The sliding average step length.
	float mPhysicsFrameTime;	// How much time we should move forward each physics step.
	int mPhysicsStepCount;
	int mTargetPhysicsStepCount;	// The number of discrete physics steps to perform this frame.
	int mReportFrame;	// The physics frame we will print status next time.

	LOG_CLASS_DECLARE();
};



}
