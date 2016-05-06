
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../../cure/include/gameticker.h"
#include "../../lepra/include/loglistener.h"
#include "../life.h"



namespace life {



class GameServerManager;
class MasterServerConnection;
struct ServerInfo;



class GameServerTicker: public cure::GameTicker {
	typedef cure::GameTicker Parent;
public:
	GameServerTicker(cure::ResourceManager* resource_manager, float physics_radius, int physics_levels, float physics_sensitivity);
	virtual ~GameServerTicker();
	virtual void StartConsole(InteractiveConsoleLogListener* console_logger);
	void SetMasterServerConnection(MasterServerConnection* connection);

protected:
	virtual bool Initialize();
	virtual bool Tick();
	virtual void PollRoundTrip();
	virtual float GetTickTimeReduction() const;
	virtual float GetPowerSaveAmount() const;

	virtual void WillMicroTick(float time_delta);
	virtual void DidPhysicsTick();

	virtual void OnTrigger(tbc::PhysicsManager::BodyID trigger, int trigger_listener_id, int other_object_id, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal);
	virtual void OnForceApplied(int object_id, int other_object_id, tbc::PhysicsManager::BodyID body_id, tbc::PhysicsManager::BodyID other_body_id,
		const vec3& force, const vec3& torque, const vec3& position, const vec3& relative_velocity);

	cure::ResourceManager* resource_manager_;
	GameServerManager* game_manager_;
	MasterServerConnection* master_connection_;

	logclass();
};



}
