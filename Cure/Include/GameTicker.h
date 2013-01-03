
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Lepra/Include/MemberThread.h"
#include "../../Lepra/Include/Performance.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/Timer.h"
#include "../../TBC/Include/ChunkyBoneGeometry.h"
#include "../../TBC/Include/PhysicsManager.h"
#include "../Include/ContextObject.h"
#include "../Include/ResourceManager.h"



namespace Cure
{



class ConsoleManager;
class ContextManager;
class NetworkAgent;
class TerrainManager;
class TimeManager;



class ApplicationTicker
{
public:
	ApplicationTicker();
	virtual ~ApplicationTicker();
	virtual bool Initialize() = 0;
	virtual bool Tick() = 0;
	virtual void PollRoundTrip() = 0;	// Polls network for any incoming to yield lower latency.
	virtual float GetTickTimeReduction() const = 0;	// Returns how much quicker the tick loop should be; can be negative.
	virtual float GetPowerSaveAmount() const = 0;
	virtual void Profile();	// Make sure it's quick, since it runs outside all profiling timers.
	virtual bool QueryQuit();
};



class GameTicker: public ApplicationTicker, public TBC::PhysicsManager::TriggerListener, public TBC::PhysicsManager::ForceFeedbackListener
{
public:
	GameTicker(float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity);
	virtual ~GameTicker();
	const TimeManager* GetTimeManager() const;
	TimeManager* GetTimeManager();
	TBC::PhysicsManager* GetPhysicsManager(bool pIsThreadSafe) const;

	void StartPhysicsTick();
	void WaitPhysicsTick();

protected:
	virtual void PhysicsTick();

private:
	void CreatePhysicsThread();
	void DeletePhysicsThread();
	void PhysicsThreadEntry();
	virtual void WillMicroTick(float pTimeDelta) = 0;
	virtual void DidPhysicsTick() = 0;

	TimeManager* mTimeManager;
	TBC::PhysicsManager* mPhysicsManager;
	MemberThread<GameTicker>* mPhysicsWorkerThread;
	Semaphore* mPhysicsTickStartSemaphore;
	Semaphore* mPhysicsTickDoneSemaphore;

	LOG_CLASS_DECLARE();
};



}
