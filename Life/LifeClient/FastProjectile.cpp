
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "fastprojectile.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/gamemanager.h"
#include "../../uicure/include/uigameuimanager.h"
#include "../../uicure/include/uisoundreleaser.h"
#include "../launcher.h"
#include "../projectileutil.h"



namespace life {



FastProjectile::FastProjectile(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, Launcher* launcher):
	Parent(resource_manager, class_id, ui_manager),
	shreek_sound_(0),
	launcher_(launcher),
	max_velocity_(0),
	acceleration_(0),
	explosive_energy_(0),
	tick_count_(0),
	is_detonated_(false) {
	EnableRootShadow(false);
	EnableMeshSlide(false);
}

FastProjectile::~FastProjectile() {
	delete shreek_sound_;
	shreek_sound_ = 0;

	if (explosive_energy_ && GetNetworkObjectType() != cure::kNetworkObjectLocalOnly) {
		ProjectileUtil::Detonate(this, &is_detonated_, launcher_, GetPosition(), GetVelocity(), vec3(), explosive_energy_, 0);
	}
}



void FastProjectile::OnLoaded() {
	Parent::OnLoaded();

	const tbc::ChunkyClass::Tag* tag = FindTag("ammo", 4, -1);
	deb_assert(tag);

	const float _muzzle_velocity = tag->float_value_list_[0];
	StartBullet(_muzzle_velocity);
	max_velocity_ = tag->float_value_list_[1];
	acceleration_ = tag->float_value_list_[2];
	explosive_energy_ = tag->float_value_list_[3];

	str launch_sound_name;
	str shreek_sound_name;
	const float pitch = ProjectileUtil::GetShotSounds(GetManager(), tag->string_value_list_, launch_sound_name, shreek_sound_name);
	if (!launch_sound_name.empty()) {
		xform parent_transform;
		vec3 parent_velocity;
		if (ProjectileUtil::GetBarrel(this, parent_transform, parent_velocity)) {
			UiCure::UserSound3dResource* launch_sound = new UiCure::UserSound3dResource(GetUiManager(), uilepra::SoundManager::kLoopNone);
			new UiCure::SoundReleaser(GetResourceManager(), ui_manager_, GetManager(), launch_sound_name, launch_sound, parent_transform.GetPosition(), parent_velocity, 5.0f, pitch);
		}
	}
	if (!shreek_sound_name.empty()) {
		shreek_sound_ = new UiCure::UserSound3dResource(GetUiManager(), uilepra::SoundManager::kLoopForward);
		shreek_sound_->Load(GetResourceManager(), shreek_sound_name,
			UiCure::UserSound3dResource::TypeLoadCallback(this, &FastProjectile::LoadPlaySound3d));
	}
}

void FastProjectile::StartBullet(float muzzle_velocity) {
	const bool is_synchronized = !GetManager()->IsLocalGameObjectId(GetInstanceId());
	const bool has_barrel = (GetOwnerInstanceId() != 0);
	ProjectileUtil::StartBullet(this, muzzle_velocity, !is_synchronized && has_barrel);

	if (is_synchronized && has_barrel) {
		// Move mesh to muzzle and let it lerp towards object.
		xform transform;
		vec3 velocity;
		ProjectileUtil::GetBarrel(this, transform, velocity);
		for (size_t x = 0; x < mesh_resource_array_.size(); ++x) {
			UiCure::UserGeometryReferenceResource* resource = mesh_resource_array_[x];
			tbc::GeometryBase* gfx_geometry = resource->GetRamData();
			gfx_geometry->SetTransformation(transform);
		}
		EnableMeshSlide(true);
		ActivateLerp();
	}
}

void FastProjectile::OnMicroTick(float frame_time) {
	Parent::OnMicroTick(frame_time);
	ProjectileUtil::BulletMicroTick(this, frame_time, max_velocity_, acceleration_);
}

void FastProjectile::OnTick() {
	Parent::OnTick();

	const vec3 _position = GetPosition();
	if (shreek_sound_ && shreek_sound_->GetLoadState() == cure::kResourceLoadComplete) {
		const vec3 velocity = GetVelocity();
		ui_manager_->GetSoundManager()->SetSoundPosition(shreek_sound_->GetData(), _position, velocity);
	}
	/*if (_position.GetLengthSquared() > 3000*3000) {
		GetManager()->PostKillObject(GetInstanceId());
	}*/
}

void FastProjectile::OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal) {
	(void)trigger_id;
	(void)body_id;
	(void)position;

	if (++tick_count_ < 10 && other_object->GetInstanceId() == GetOwnerInstanceId()) {	// Disallow self-hit during the first few frames.
		return;
	}

	if (explosive_energy_) {
		ProjectileUtil::Detonate(this, &is_detonated_, launcher_, GetPosition(), GetVelocity(), normal, explosive_energy_, 0);
	} else {
		ProjectileUtil::OnBulletHit(this, &is_detonated_, launcher_, other_object);
	}
}

void FastProjectile::LoadPlaySound3d(UiCure::UserSound3dResource* sound_resource) {
	deb_assert(sound_resource->GetLoadState() == cure::kResourceLoadComplete);
	if (sound_resource->GetLoadState() == cure::kResourceLoadComplete) {
		ui_manager_->GetSoundManager()->SetSoundPosition(sound_resource->GetData(), GetPosition(), GetVelocity());
		ui_manager_->GetSoundManager()->Play(sound_resource->GetData(), 0.7f, 1.0);
	}
}



loginstance(kGameContextCpp, FastProjectile);



}
