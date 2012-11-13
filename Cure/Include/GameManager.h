
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



class GameTicker
{
public:
	GameTicker();
	virtual ~GameTicker();
	virtual bool Initialize() = 0;
	virtual bool Tick() = 0;
	virtual void PollRoundTrip() = 0;	// Polls network for any incoming to yield lower latency.
	virtual float GetTickTimeReduction() const = 0;	// Returns how much quicker the tick loop should be; can be negative.
	virtual float GetPowerSaveAmount() const = 0;
	const TimeManager* GetTimeManager() const;
	TimeManager* GetTimeManager();
	virtual void Profile();	// Make sure it's quick, since it runs outside all profiling timers.
	virtual bool QueryQuit();

private:
	TimeManager* mTimeManager;
};



class GameManager: public TBC::PhysicsManager::TriggerListener, public TBC::PhysicsManager::ForceFeedbackListener
{
public:
	typedef SequencialPerformanceData<uint64> BandwidthData;

	GameManager(const TimeManager* pTime, RuntimeVariableScope* pVariableScope, ResourceManager* pResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity);
	virtual ~GameManager();

	virtual bool BeginTick();
	virtual bool EndTick();
	virtual bool TickNetworkOutput();
	Lock* GetTickLock() const;

	RuntimeVariableScope* GetVariableScope() const;
	void SetVariableScope(RuntimeVariableScope* pScope);
	ResourceManager* GetResourceManager() const;
	ContextManager* GetContext() const;
	const TimeManager* GetTimeManager() const;
	LEPRA_DEBUG_CODE(virtual) TBC::PhysicsManager* GetPhysicsManager() const;
	ConsoleManager* GetConsoleManager() const;

	virtual bool IsObjectRelevant(const Vector3DF& pPosition, float pDistance) const;
	ContextObject* CreateContextObject(const str& pClassId, NetworkObjectType pNetworkType, GameObjectId pInstanceId = 0);
	void AddContextObject(ContextObject* pObject, NetworkObjectType pNetworkType, GameObjectId pInstanceId);
	virtual ContextObject* CreateLogicHandler(const str& pType);
	virtual bool IsUiMoveForbidden(GameObjectId pObjectId) const;
	virtual void GetSiblings(GameObjectId pObjectId, ContextObject::Array& pSiblingArray) const;
	virtual void OnLoadCompleted(ContextObject* pObject, bool pOk) = 0;
	virtual void OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
		ContextObject* pObject1, ContextObject* pObject2,
		TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id) = 0;
	void OnStopped(ContextObject* pObject, TBC::PhysicsManager::BodyID pBodyId);
	virtual bool OnPhysicsSend(ContextObject* pObject) = 0;
	virtual bool OnAttributeSend(ContextObject* pObject) = 0;
	virtual bool IsServer() = 0;
	virtual void SendAttach(ContextObject* pObject1, unsigned pId1, ContextObject* pObject2, unsigned pId2) = 0;
	virtual void SendDetach(ContextObject* pObject1, ContextObject* pObject2) = 0;
	virtual void OnAlarm(int pAlarmId, ContextObject* pObject, void* pExtraData);

	bool ValidateVariable(int pSecurityLevel, const str& pVariable, str& pValue) const;

	void UpdateReportPerformance(bool pReport, double pReportInterval);
	void ClearPerformanceData();
	void GetBandwidthData(BandwidthData& mSendBandwidth, BandwidthData& mReceiveBandwidth);

protected:
	void SetConsoleManager(ConsoleManager* pConsole);

	NetworkAgent* GetNetworkAgent() const;
	void SetNetworkAgent(NetworkAgent* pNetwork);

	virtual void TickInput() = 0;

	virtual ContextObject* CreateContextObject(const str& pClassId) const = 0;

	void ReportPerformance(const ScopePerformanceData::NodeArray& pNodes, int pRecursion);

	void StartPhysicsTick();
	void WaitPhysicsTick();

	virtual void PhysicsTick();

	bool IsThreadSafe() const;

private:
	void ScriptTick(float pTimeDelta);
	void ScriptPhysicsTick();
	virtual void HandleWorldBoundaries();

	virtual bool InitializeTerrain() = 0;

	void PhysicsThreadEntry();
	void CreatePhysicsThread();
	void DeletePhysicsThread();
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTrigger, int pTriggerListenerId, int pOtherBodyId);
	virtual void OnForceApplied(int pObjectId, int pOtherObjectId, TBC::PhysicsManager::BodyID pBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);

	mutable Lock mLock;
	volatile bool mIsThreadSafe;

	RuntimeVariableScope* mVariableScope;
	ResourceManager* mResource;
	NetworkAgent* mNetwork;
	const TimeManager* mTime;
	TBC::PhysicsManager* mPhysics;
	ContextManager* mContext;
	TerrainManager* mTerrain;
	ConsoleManager* mConsole;
	MemberThread<GameManager>* mPhysicsWorkerThread;
	Semaphore* mPhysicsTickStartSemaphore;
	Semaphore* mPhysicsTickDoneSemaphore;

	Timer mPerformanceReportTimer;
	BandwidthData mSendBandwidth;
	BandwidthData mReceiveBandwidth;

	LOG_CLASS_DECLARE();
};



}
