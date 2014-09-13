
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
class GameTicker;
class NetworkAgent;
class ResourceManager;
class Spawner;
class TerrainManager;
class TimeManager;



class GameManager: public Tbc::PhysicsManager::TriggerListener, public Tbc::PhysicsManager::ForceFeedbackListener
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
	LEPRA_DEBUG_CODE(virtual) Tbc::PhysicsManager* GetPhysicsManager() const;
	ConsoleManager* GetConsoleManager() const;
	void SetConsoleManager(ConsoleManager* pConsole);

	void MicroTick(float pTimeDelta);
	void PostPhysicsTick();

	virtual bool IsObjectRelevant(const vec3& pPosition, float pDistance) const;
	ContextObject* CreateContextObject(const str& pClassId, NetworkObjectType pNetworkType, GameObjectId pInstanceId = 0);
	virtual void DeleteContextObject(Cure::GameObjectId pInstanceId);
	void AddContextObject(ContextObject* pObject, NetworkObjectType pNetworkType, GameObjectId pInstanceId);
	virtual ContextObject* CreateLogicHandler(const str& pType);
	virtual Spawner* GetAvatarSpawner(Cure::GameObjectId pLevelId) const;
	virtual bool IsUiMoveForbidden(GameObjectId pObjectId) const;
	virtual void OnLoadCompleted(ContextObject* pObject, bool pOk) = 0;
	virtual void OnCollision(const vec3& pForce, const vec3& pTorque, const vec3& pPosition,
		ContextObject* pObject1, ContextObject* pObject2,
		Tbc::PhysicsManager::BodyID pBody1Id, Tbc::PhysicsManager::BodyID pBody2Id) = 0;
	void OnStopped(ContextObject* pObject, Tbc::PhysicsManager::BodyID pBodyId);
	virtual bool OnPhysicsSend(ContextObject* pObject) = 0;
	virtual bool OnAttributeSend(ContextObject* pObject) = 0;
	virtual bool IsServer() = 0;
	virtual void SendAttach(ContextObject* pObject1, unsigned pId1, ContextObject* pObject2, unsigned pId2) = 0;
	virtual void SendDetach(ContextObject* pObject1, ContextObject* pObject2) = 0;

	bool ValidateVariable(int pSecurityLevel, const str& pVariable, str& pValue) const;

	void UpdateReportPerformance(bool pReport, double pReportInterval);
	void ClearPerformanceData();
	void GetBandwidthData(BandwidthData& mSendBandwidth, BandwidthData& mReceiveBandwidth);

	virtual void OnTrigger(Tbc::PhysicsManager::TriggerID pTrigger, int pTriggerListenerId, int pOtherObjectId, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pNormal);
	virtual void OnForceApplied(int pObjectId, int pOtherObjectId, Tbc::PhysicsManager::BodyID pBodyId, Tbc::PhysicsManager::BodyID pOtherBodyId,
		const vec3& pForce, const vec3& pTorque, const vec3& pPosition, const vec3& pRelativeVelocity);

protected:
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

	logclass();
};



}
