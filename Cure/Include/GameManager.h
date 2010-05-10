
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
	virtual ~GameTicker();
	virtual bool Initialize() = 0;
	virtual bool Tick() = 0;
	virtual float GetPowerSaveAmount() const = 0;
	virtual void Profile();	// Make sure it's quick, since it runs outside all profiling timers.
	virtual bool QueryQuit();
};



class GameManager
{
public:
	GameManager(RuntimeVariableScope* pVariableScope, ResourceManager* pResourceManager);
	virtual ~GameManager();

	void ResetPhysicsTime(int pStartPhysicsFrame);
	virtual bool BeginTick();
	virtual bool EndTick();
	Lock* GetTickLock() const;

	RuntimeVariableScope* GetVariableScope() const;
	ResourceManager* GetResourceManager() const;
	ContextManager* GetContext() const;
	const TimeManager* GetConstTimeManager() const;
	LEPRA_DEBUG_CODE(virtual) TBC::PhysicsManager* GetPhysicsManager() const;
	ConsoleManager* GetConsoleManager() const;
	TimeManager* GetTimeManager();

	ContextObject* CreateContextObject(const str& pClassId, NetworkObjectType pNetworkType,
		GameObjectId pInstanceId = 0);
	virtual ContextObject* CreateTriggerHandler(const str& pType) const = 0;
	virtual bool IsUiMoveForbidden(GameObjectId pObjectId) const;
	virtual void GetSiblings(GameObjectId pObjectId, ContextObject::Array& pSiblingArray) const;
	virtual void OnLoadCompleted(ContextObject* pObject, bool pOk) = 0;
	virtual void OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque,
		ContextObject* pObject1, ContextObject* pObject2) = 0;
	virtual void OnStopped(ContextObject* pObject, TBC::PhysicsManager::BodyID pBodyId) = 0;
	virtual bool OnPhysicsSend(ContextObject* pObject) = 0;
	virtual bool IsConnectAuthorized() = 0;
	virtual void SendAttach(ContextObject* pObject1, unsigned pId1, ContextObject* pObject2, unsigned pId2) = 0;
	virtual void SendDetach(ContextObject* pObject1, ContextObject* pObject2) = 0;

	void TryReportPerformance(double pReportInterval);
	void ClearPerformanceData();

protected:
	void SetConsoleManager(ConsoleManager* pConsole);

	NetworkAgent* GetNetworkAgent() const;
	void SetNetworkAgent(NetworkAgent* pNetwork);

	virtual void TickInput() = 0;
	virtual bool TickNetworkOutput();

	virtual ContextObject* CreateContextObject(const str& pClassId) const = 0;

	void ReportPerformance(const ScopePerformanceData::NodeArray& pNodes, int pRecursion);

	void StartPhysicsTick();
	void WaitPhysicsTick();

	virtual void PhysicsTick();
	bool IsHighImpact(float pScaleFactor, const ContextObject* pObject, const Vector3DF& pForce,
		const Vector3DF& pTorque) const;

	bool IsThreadSafe() const;

private:
	void ScriptTick(float pTimeDelta);
	void ScriptPhysicsTick();

	virtual bool InitializeTerrain() = 0;

	void PhysicsThreadEntry();
	void CreatePhysicsThread();
	void DeletePhysicsThread();

	mutable Lock mLock;
	volatile bool mIsThreadSafe;

	RuntimeVariableScope* mVariableScope;
	ResourceManager* mResource;
	NetworkAgent* mNetwork;
	TimeManager* mTime;
	TBC::PhysicsManager* mPhysics;
	ContextManager* mContext;
	TerrainManager* mTerrain;
	ConsoleManager* mConsole;
	MemberThread<GameManager>* mPhysicsWorkerThread;
	Semaphore* mPhysicsTickStartSemaphore;
	Semaphore* mPhysicsTickDoneSemaphore;

	Timer mPerformanceReportTimer;
	SequencialPerformanceData<uint64> mSendBandwidth;
	SequencialPerformanceData<uint64> mReceiveBandwidth;

	LOG_CLASS_DECLARE();
};



}
