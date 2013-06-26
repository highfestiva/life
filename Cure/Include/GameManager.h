
// Author: Jonas Bystr�m
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
class GameTicker;
class NetworkAgent;
class Spawner;
class TerrainManager;
class TimeManager;



class GameManager: public TBC::PhysicsManager::TriggerListener, public TBC::PhysicsManager::ForceFeedbackListener
{
public:
	typedef SequencialPerformanceData<uint64> BandwidthData;

	GameManager(const TimeManager* pTime, RuntimeVariableScope* pVariableScope, ResourceManager* pResourceManager);
	virtual ~GameManager();
	const GameTicker* GetTicker() const;
	void SetTicker(const GameTicker* pTicker);

	virtual bool IsPrimaryManager() const;
	virtual bool BeginTick();
	virtual void PreEndTick();
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

	void MicroTick(float pTimeDelta);
	void PostPhysicsTick();

	virtual bool IsObjectRelevant(const Vector3DF& pPosition, float pDistance) const;
	ContextObject* CreateContextObject(const str& pClassId, NetworkObjectType pNetworkType, GameObjectId pInstanceId = 0);
	virtual void DeleteContextObject(Cure::GameObjectId pInstanceId);
	virtual void DeleteContextObjectDelay(Cure::ContextObject* pObject, float pDelay);
	void AddContextObject(ContextObject* pObject, NetworkObjectType pNetworkType, GameObjectId pInstanceId);
	virtual ContextObject* CreateLogicHandler(const str& pType);
	virtual Spawner* GetAvatarSpawner(Cure::GameObjectId pLevelId) const;
	virtual bool IsUiMoveForbidden(GameObjectId pObjectId) const;
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

	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTrigger, int pTriggerListenerId, int pOtherBodyId, const Vector3DF& pNormal);
	virtual void OnForceApplied(int pObjectId, int pOtherObjectId, TBC::PhysicsManager::BodyID pBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);

protected:
	void SetConsoleManager(ConsoleManager* pConsole);

	NetworkAgent* GetNetworkAgent() const;
	void SetNetworkAgent(NetworkAgent* pNetwork);

	virtual void TickInput() = 0;

	virtual ContextObject* CreateContextObject(const str& pClassId) const = 0;
	virtual void ScriptPhysicsTick();

	void ReportPerformance(const ScopePerformanceData::NodeArray& pNodes, int pRecursion);

	bool IsThreadSafe() const;

private:
	virtual void HandleWorldBoundaries();

	mutable Lock mLock;
	volatile bool mIsThreadSafe;

	RuntimeVariableScope* mVariableScope;
	ResourceManager* mResource;
	NetworkAgent* mNetwork;
	const GameTicker* mTicker;
	const TimeManager* mTime;
	ContextManager* mContext;
	TerrainManager* mTerrain;
	ConsoleManager* mConsole;

	Timer mPerformanceReportTimer;
	BandwidthData mSendBandwidth;
	BandwidthData mReceiveBandwidth;

	LOG_CLASS_DECLARE();
};



}
