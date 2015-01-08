
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Lepra/Include/MemberThread.h"
#include "../../Lepra/Include/Performance.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/Timer.h"
#include "../../Tbc/Include/ChunkyBoneGeometry.h"
#include "../../Tbc/Include/PhysicsManager.h"
#include "../Include/ContextObject.h"



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
	virtual void Suspend();
	virtual void Resume();
	virtual void Profile();	// Make sure it's quick, since it runs outside all profiling timers.
	virtual bool QueryQuit();
};



class GameTicker: public ApplicationTicker, public Tbc::PhysicsManager::TriggerListener, public Tbc::PhysicsManager::ForceFeedbackListener
{
public:
	GameTicker(float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity);
	virtual ~GameTicker();
	const TimeManager* GetTimeManager() const;
	TimeManager* GetTimeManager();
	Tbc::PhysicsManager* GetPhysicsManager(bool pIsThreadSafe) const;
	Lock* GetPhysicsLock();

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
	Tbc::PhysicsManager* mPhysicsManager;
	MemberThread<GameTicker>* mPhysicsWorkerThread;
	Lock mPhysicsLock;
	Semaphore* mPhysicsTickStartSemaphore;
	Semaphore* mPhysicsTickDoneSemaphore;

	logclass();
};



}
