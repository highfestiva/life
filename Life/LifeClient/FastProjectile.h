
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../uicure/include/uimachine.h"
#include "../life.h"



namespace life {



class Launcher;



class FastProjectile: public UiCure::Machine {
public:
	typedef UiCure::Machine Parent;

	FastProjectile(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, Launcher* launcher);
	virtual ~FastProjectile();

protected:
	virtual void OnLoaded();
	void StartBullet(float muzzle_velocity);
	virtual void OnMicroTick(float frame_time);
	virtual void OnTick();
	virtual void OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal);
	void LoadPlaySound3d(UiCure::UserSound3dResource* sound_resource);

	UiCure::UserSound3dResource* shreek_sound_;
	Launcher* launcher_;
	float max_velocity_;
	float acceleration_;
	float explosive_energy_;
	int tick_count_;
	bool is_detonated_;

	logclass();
};



}
