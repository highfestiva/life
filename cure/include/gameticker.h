
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/memberthread.h"
#include "../../lepra/include/performance.h"
#include "../../lepra/include/string.h"
#include "../../lepra/include/timer.h"
#include "../../tbc/include/chunkybonegeometry.h"
#include "../../tbc/include/physicsmanager.h"
#include "../include/contextobject.h"



namespace cure {



class ConsoleManager;
class ContextManager;
class NetworkAgent;
class TerrainManager;
class TimeManager;



class ApplicationTicker {
public:
	ApplicationTicker();
	virtual ~ApplicationTicker();
	virtual bool Initialize() = 0;
	virtual bool Tick() = 0;
	virtual void PollRoundTrip() = 0;	// Polls network for any incoming to yield lower latency.
	virtual float GetTickTimeReduction() const = 0;	// Returns how much quicker the tick loop should be; can be negative.
	virtual float GetPowerSaveAmount() const = 0;
	virtual void Resume(bool hard);
	virtual void Suspend(bool hard);
	virtual void Profile();	// Make sure it's quick, since it runs outside all profiling timers.
	virtual bool QueryQuit();
};



class GameTicker: public ApplicationTicker, public tbc::PhysicsManager::TriggerListener, public tbc::PhysicsManager::ForceFeedbackListener {
public:
	GameTicker(float physics_radius, int physics_levels, float physics_sensitivity);
	virtual ~GameTicker();
	const TimeManager* GetTimeManager() const;
	TimeManager* GetTimeManager();
	tbc::PhysicsManager* GetPhysicsManager(bool is_thread_safe) const;
	Lock* GetPhysicsLock();

	void StartPhysicsTick();
	void WaitPhysicsTick();

protected:
	virtual void PhysicsTick();

private:
	void CreatePhysicsThread();
	void DeletePhysicsThread();
	void PhysicsThreadEntry();
	virtual void WillMicroTick(float time_delta) = 0;
	virtual void DidPhysicsTick() = 0;

	TimeManager* time_manager_;
	tbc::PhysicsManager* physics_manager_;
	MemberThread<GameTicker>* physics_worker_thread_;
	Lock physics_lock_;
	Semaphore* physics_tick_start_semaphore_;
	Semaphore* physics_tick_done_semaphore_;

	logclass();
};



}
