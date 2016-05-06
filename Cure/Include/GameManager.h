
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
class GameTicker;
class NetworkAgent;
class ResourceManager;
class Spawner;
class TerrainManager;
class TimeManager;



class GameManager: public tbc::PhysicsManager::TriggerListener, public tbc::PhysicsManager::ForceFeedbackListener {
public:
	typedef SequencialPerformanceData<uint64> BandwidthData;

	GameManager(const TimeManager* time, RuntimeVariableScope* variable_scope, ResourceManager* resource_manager);
	virtual ~GameManager();
	const GameTicker* GetTicker() const;
	void SetTicker(const GameTicker* ticker);

	virtual bool IsPrimaryManager() const;
	virtual bool BeginTick();
	virtual void PreEndTick();
	virtual bool EndTick();
	virtual bool TickNetworkOutput();
	Lock* GetTickLock() const;

	RuntimeVariableScope* GetVariableScope() const;
	void SetVariableScope(RuntimeVariableScope* scope);
	ResourceManager* GetResourceManager() const;
	ContextManager* GetContext() const;
	const TimeManager* GetTimeManager() const;
	LEPRA_DEBUG_CODE(virtual) tbc::PhysicsManager* GetPhysicsManager() const;
	ConsoleManager* GetConsoleManager() const;
	void SetConsoleManager(ConsoleManager* console);

	void MicroTick(float time_delta);
	void PostPhysicsTick();

	virtual bool IsObjectRelevant(const vec3& position, float distance) const;
	ContextObject* CreateContextObject(const str& class_id, NetworkObjectType network_type, GameObjectId instance_id = 0);
	virtual void DeleteContextObject(cure::GameObjectId instance_id);
	void AddContextObject(ContextObject* object, NetworkObjectType network_type, GameObjectId instance_id);
	virtual ContextObject* CreateLogicHandler(const str& type);
	virtual Spawner* GetAvatarSpawner(cure::GameObjectId level_id) const;
	virtual bool IsUiMoveForbidden(GameObjectId object_id) const;
	virtual void OnLoadCompleted(ContextObject* object, bool ok) = 0;
	virtual void OnCollision(const vec3& force, const vec3& torque, const vec3& position,
		ContextObject* object1, ContextObject* object2,
		tbc::PhysicsManager::BodyID body1_id, tbc::PhysicsManager::BodyID body2_id) = 0;
	void OnStopped(ContextObject* object, tbc::PhysicsManager::BodyID body_id);
	virtual bool OnPhysicsSend(ContextObject* object) = 0;
	virtual bool OnAttributeSend(ContextObject* object) = 0;
	virtual bool IsServer() = 0;
	virtual void SendAttach(ContextObject* object1, unsigned id1, ContextObject* object2, unsigned id2) = 0;
	virtual void SendDetach(ContextObject* object1, ContextObject* object2) = 0;

	bool ValidateVariable(int security_level, const str& variable, str& value) const;

	void UpdateReportPerformance(bool report, double report_interval);
	void ClearPerformanceData();
	void GetBandwidthData(BandwidthData& send_bandwidth_, BandwidthData& receive_bandwidth_);

	virtual void OnTrigger(tbc::PhysicsManager::BodyID trigger, int trigger_listener_id, int other_object_id, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal);
	virtual void OnForceApplied(int object_id, int other_object_id, tbc::PhysicsManager::BodyID body_id, tbc::PhysicsManager::BodyID other_body_id,
		const vec3& force, const vec3& torque, const vec3& position, const vec3& relative_velocity);

protected:
	NetworkAgent* GetNetworkAgent() const;
	void SetNetworkAgent(NetworkAgent* network);

	virtual void TickInput() = 0;

	virtual ContextObject* CreateContextObject(const str& class_id) const = 0;
	virtual void ScriptPhysicsTick();

	void ReportPerformance(const ScopePerformanceData::NodeArray& nodes, int recursion);

	bool IsThreadSafe() const;

private:
	virtual void HandleWorldBoundaries();

	mutable Lock lock_;
	volatile bool is_thread_safe_;

	RuntimeVariableScope* variable_scope_;
	ResourceManager* resource_;
	NetworkAgent* network_;
	const GameTicker* ticker_;
	const TimeManager* time_;
	ContextManager* context_;
	TerrainManager* terrain_;
	ConsoleManager* console_;

	Timer performance_report_timer_;
	BandwidthData send_bandwidth_;
	BandwidthData receive_bandwidth_;

	logclass();
};



}
