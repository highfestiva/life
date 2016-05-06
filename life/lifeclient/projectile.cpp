
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "projectile.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/gamemanager.h"
#include "../../uicure/include/uigameuimanager.h"
#include "../../uicure/include/uisoundreleaser.h"
#include "../launcher.h"
#include "../projectileutil.h"



namespace life {



Projectile::Projectile(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, Launcher* launcher):
	Parent(resource_manager, class_id, ui_manager),
	shreek_sound_(0),
	launcher_(launcher),
	tick_counter_(0),
	is_detonated_(false),
	explosive_energy_(1) {
	EnableRootShadow(false);
}

Projectile::~Projectile() {
	delete shreek_sound_;
	shreek_sound_ = 0;
	ProjectileUtil::Detonate(this, &is_detonated_, launcher_, GetPosition(), GetVelocity(), vec3(), explosive_energy_, -1);
}



void Projectile::OnLoaded() {
	Parent::OnLoaded();

	const tbc::ChunkyClass::Tag* tag = FindTag("ammo", 4, -1);
	deb_assert(tag);
	explosive_energy_ = tag->float_value_list_[3];

	str launch_sound_name;
	str shreek_sound_name;
	const float pitch = ProjectileUtil::GetShotSounds(GetManager(), tag->string_value_list_, launch_sound_name, shreek_sound_name);
	if (!launch_sound_name.empty()) {
		xform parent_transform;
		vec3 parent_velocity;
		if (!ProjectileUtil::GetBarrel(this, parent_transform, parent_velocity)) {
			parent_transform.SetPosition(GetPosition());
			parent_velocity = GetVelocity();
		}
		UiCure::UserSound3dResource* launch_sound = new UiCure::UserSound3dResource(GetUiManager(), uilepra::SoundManager::kLoopNone);
		new UiCure::SoundReleaser(GetResourceManager(), ui_manager_, GetManager(), launch_sound_name, launch_sound, parent_transform.GetPosition(), parent_velocity, 5.0f, pitch);
	}
	if (!shreek_sound_name.empty()) {
		shreek_sound_ = new UiCure::UserSound3dResource(GetUiManager(), uilepra::SoundManager::kLoopForward);
		shreek_sound_->Load(GetResourceManager(), shreek_sound_name,
			UiCure::UserSound3dResource::TypeLoadCallback(this, &Projectile::LoadPlaySound3d));
	}
}

void Projectile::OnTick() {
	++tick_counter_;
	if (shreek_sound_ && shreek_sound_->GetLoadState() == cure::kResourceLoadComplete) {
		// Point the projectile in the velocity direction.
		const vec3 _position = GetPosition();
		vec3 velocity = GetVelocity();
		ui_manager_->GetSoundManager()->SetSoundPosition(shreek_sound_->GetData(), _position, velocity);
	}
	Parent::OnTick();
}

void Projectile::OnForceApplied(ContextObject* other_object,
	tbc::PhysicsManager::BodyID own_body_id, tbc::PhysicsManager::BodyID other_body_id,
	const vec3& force, const vec3& torque,
	const vec3& position, const vec3& relative_velocity) {
	(void)own_body_id;
	(void)other_body_id;
	(void)force;
	(void)torque;
	(void)position;

	if (tick_counter_ < 3 && other_object->GetInstanceId() == GetOwnerInstanceId()) {	// Don't detonate on oneself immediately.
		return;
	}

	ProjectileUtil::Detonate(this, &is_detonated_, launcher_, position, relative_velocity, vec3(), explosive_energy_, 0);
}

void Projectile::LoadPlaySound3d(UiCure::UserSound3dResource* sound_resource) {
	deb_assert(sound_resource->GetLoadState() == cure::kResourceLoadComplete);
	if (sound_resource->GetLoadState() == cure::kResourceLoadComplete) {
		ui_manager_->GetSoundManager()->SetSoundPosition(sound_resource->GetData(), GetPosition(), GetVelocity());
		ui_manager_->GetSoundManager()->Play(sound_resource->GetData(), 0.7f, 1.0);
	}
}



loginstance(kGameContextCpp, Projectile);



}
