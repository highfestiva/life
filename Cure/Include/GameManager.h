
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#pragma once

#include "../../Lepra/Include/MemberThread.h"
#include "../../Lepra/Include/Performance.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/Timer.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "../Include/PhysicsNode.h"
#include "../Include/ResourceManager.h"



namespace Cure
{



class ConsoleManager;
class ContextManager;
class ContextObject;
class NetworkAgent;
class TerrainManager;
class TimeManager;



class GameTicker
{
public:
	virtual ~GameTicker();
	virtual bool Initialize() = 0;
	virtual bool Tick() = 0;
	virtual float GetPowerSaveAmount() const = 0;
};



class GameManager
{
public:
	GameManager(RuntimeVariableScope* pVariableScope, ResourceManager* pResourceManager,
		bool pForceSynchronous);
	virtual ~GameManager();

	void ResetPhysicsTime(int pStartPhysicsFrame);
	virtual bool BeginTick();
	virtual bool EndTick();
	Lepra::Lock* GetTickLock() const;

	RuntimeVariableScope* GetVariableScope() const;
	ResourceManager* GetResourceManager() const;
	ContextManager* GetContext() const;
	const TimeManager* GetConstTimeManager() const;
	TBC::PhysicsEngine* GetPhysicsManager() const;
	ConsoleManager* GetConsoleManager() const;

	virtual void OnCollision(const Lepra::Vector3DF& pForce, const Lepra::Vector3DF& pTorque,
		ContextObject* pObject1, ContextObject* pObject2) = 0;
	virtual void OnStopped(ContextObject* pObject, TBC::PhysicsEngine::BodyID pBodyId) = 0;
	virtual bool IsConnectAuthorized() = 0;
	virtual void SendAttach(ContextObject* pObject1, PhysicsNode::Id pId1, ContextObject* pObject2, PhysicsNode::Id pId2) = 0;
	virtual void SendDetach(ContextObject* pObject1, ContextObject* pObject2) = 0;

	void ReportPerformance(double pReportInterval);
	void ClearPerformanceData();

protected:
	void SetConsoleManager(ConsoleManager* pConsole);

	NetworkAgent* GetNetworkAgent() const;
	void SetNetworkAgent(NetworkAgent* pNetwork);
	TimeManager* GetTimeManager();

	virtual void TickInput() = 0;
	virtual bool TickNetworkOutput();

	void ReportPerformance(const Lepra::String& pHead, const Lepra::PerformanceData& pPerformance);

	void StartPhysicsTick();
	void WaitPhysicsTick();

	virtual void PhysicsTick();

	bool IsThreadSafe() const;

private:
	void ScriptTick();
	void ScriptPhysicsTick();

	virtual bool InitializeTerrain() = 0;

	void PhysicsThreadEntry();
	void CreatePhysicsThread();
	void DeletePhysicsThread();

	mutable Lepra::Lock mLock;
	volatile bool mIsThreadSafe;

	RuntimeVariableScope* mVariableScope;
	ResourceManager* mResource;
	NetworkAgent* mNetwork;
	TimeManager* mTime;
	TBC::PhysicsEngine* mPhysics;
	ContextManager* mContext;
	TerrainManager* mTerrain;
	ConsoleManager* mConsole;
	Lepra::MemberThread<GameManager>* mPhysicsWorkerThread;
	Lepra::Semaphore* mPhysicsTickStartSemaphore;
	Lepra::Semaphore* mPhysicsTickDoneSemaphore;

	Lepra::Timer mPerformanceReportTimer;
	Lepra::SequencialPerformanceData<Lepra::uint64> mSendBandwidth;
	Lepra::SequencialPerformanceData<Lepra::uint64> mReceiveBandwidth;
	Lepra::PerformanceData mPhysicsTime;
	Lepra::PerformanceData mPhysicsPropagationTime;
	Lepra::PerformanceData mNetworkAndInputTime;
	Lepra::PerformanceData mWaitPhysicsTime;
	Lepra::PerformanceData mWallTime;

	LOG_CLASS_DECLARE();
};



}
