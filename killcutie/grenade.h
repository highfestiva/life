
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uicure/include/uimachine.h"



namespace grenaderun {



using namespace lepra;



class Grenade: public UiCure::Machine {
public:
	typedef UiCure::Machine Parent;

	Grenade(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, float muzzle_velocity);
	virtual ~Grenade();

	bool IsUserFired() const;

private:
	void Launch();
	void OnTick();
	virtual bool TryComplete();
	virtual void OnForceApplied(cure::ContextObject* other_object,
		tbc::PhysicsManager::BodyID own_body_id, tbc::PhysicsManager::BodyID other_body_id,
		const vec3& force, const vec3& torque,
		const vec3& position, const vec3& relative_velocity);
	void LoadPlaySound3d(UiCure::UserSound3dResource* sound_resource);

	UiCure::UserSound3dResource* shreek_sound_;
	UiCure::UserSound3dResource* launch_sound_;
	int time_frame_created_;
	float muzzle_velocity_;
	bool is_launched_;
	bool exploded_;
	bool is_user_fired_;

	logclass();
};



}
