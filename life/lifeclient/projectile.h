
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../uicure/include/uimachine.h"
#include "../life.h"



namespace life {



class Launcher;



class Projectile: public UiCure::Machine {
public:
	typedef UiCure::Machine Parent;

	Projectile(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, Launcher* launcher);
	virtual ~Projectile();

private:
	virtual void OnLoaded();
	void OnTick();
	virtual void OnForceApplied(ContextObject* other_object,
		tbc::PhysicsManager::BodyID own_body_id, tbc::PhysicsManager::BodyID other_body_id,
		const vec3& force, const vec3& torque,
		const vec3& position, const vec3& relative_velocity);
	void LoadPlaySound3d(UiCure::UserSound3dResource* sound_resource);

	UiCure::UserSound3dResource* shreek_sound_;
	Launcher* launcher_;
	int tick_counter_;
	bool is_detonated_;
	float explosive_energy_;

	logclass();
};



}
