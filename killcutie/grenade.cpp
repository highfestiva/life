
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "grenade.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/runtimevariable.h"
#include "../cure/include/timemanager.h"
#include "../tbc/include/chunkybonegeometry.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uicure/include/uiprops.h"
#include "game.h"



namespace grenaderun {



Grenade::Grenade(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, float muzzle_velocity):
	Parent(resource_manager, class_id, ui_manager),
	shreek_sound_(0),
	launch_sound_(0),
	time_frame_created_(-1),
	muzzle_velocity_(muzzle_velocity),
	is_launched_(false),
	exploded_(false),
	is_user_fired_(false) {
	SetForceLoadUnique(true);	// Needs to be unique as physics are reloaded often with shared IDs.
}

Grenade::~Grenade() {
	delete launch_sound_;
	launch_sound_ = 0;
	delete shreek_sound_;
	shreek_sound_ = 0;
}



bool Grenade::IsUserFired() const {
	return is_user_fired_;
}



void Grenade::Launch() {
	is_launched_ = true;

	shreek_sound_ = new UiCure::UserSound3dResource(GetUiManager(), uilepra::SoundManager::kLoopForward);
	shreek_sound_->Load(GetResourceManager(), "incoming.wav",
		UiCure::UserSound3dResource::TypeLoadCallback(this, &Grenade::LoadPlaySound3d));

	launch_sound_ = new UiCure::UserSound3dResource(GetUiManager(), uilepra::SoundManager::kLoopNone);
	launch_sound_->Load(GetResourceManager(), "launch.wav",
		UiCure::UserSound3dResource::TypeLoadCallback(this, &Grenade::LoadPlaySound3d));

	const tbc::ChunkyBoneGeometry* geometry = physics_->GetBoneGeometry(physics_->GetRootBone());
	GetManager()->GetGameManager()->GetPhysicsManager()->EnableGravity(geometry->GetBodyId(), true);
	vec3 velocity = GetOrientation() * vec3(0, 0, muzzle_velocity_);
	GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyVelocity(geometry->GetBodyId(), velocity);

	((Game*)GetManager()->GetGameManager())->FreeLauncherBarrel();
	is_user_fired_ = (((Game*)GetManager()->GetGameManager())->GetComputerIndex() != 1);
}

void Grenade::OnTick() {
	if (shreek_sound_ && shreek_sound_->GetLoadState() == cure::kResourceLoadComplete) {
		const vec3 _position = GetPosition();
		vec3 velocity = GetVelocity();
		ui_manager_->GetSoundManager()->SetSoundPosition(shreek_sound_->GetData(), _position, velocity);
		if (velocity.GetLengthSquared() > 1*1) {
			tbc::PhysicsManager::BodyID body_id = physics_->GetBoneGeometry(0)->GetBodyId();
			const float l = velocity.GetLength();
			const float pitch = -velocity.GetAngle(vec3(0,0,l));
			vec2 xy = vec2(velocity.x, velocity.y);
			const float l2 = xy.GetLength();
			if (l2) {
				const float yaw = xy.GetAngle(vec2(0,l2));
				quat q;
				//q.SetEulerAngles(yaw, pitch, 0);
				q.RotateAroundWorldZ(_position.z/100);
				q.RotateAroundWorldX(pitch);
				q.RotateAroundWorldZ(-yaw);
				xform t;
				t.GetOrientation() = q * physics_->GetOriginalBoneTransformation(0).GetOrientation();
				t.SetPosition(_position);
				GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyTransform(body_id, t);
			}
		}
	}
	if (launch_sound_ && launch_sound_->GetLoadState() == cure::kResourceLoadComplete) {
		vec3 _position;
		vec3 velocity;
		((Game*)GetManager()->GetGameManager())->GetVehicleMotion(_position, velocity);
		ui_manager_->GetSoundManager()->SetSoundPosition(launch_sound_->GetData(), _position, velocity);
	}
	if (!is_launched_ && time_frame_created_ >= 0) {
		xform transform;
		((Game*)GetManager()->GetGameManager())->GetLauncherTransform(transform);
		const cure::TimeManager* time_manager = GetManager()->GetGameManager()->GetTimeManager();
		const float time = time_manager->ConvertPhysicsFramesToSeconds(time_manager->GetCurrentPhysicsFrameDelta(time_frame_created_));
		const float launcher_length = 1.1f;
		float real_time_ratio;
		v_get(real_time_ratio, =(float), cure::GetSettings(), kRtvarPhysicsRtr, 1.0);
		float h = launcher_length/2+3 - time*time*4.0f*real_time_ratio;
		h = std::max(-launcher_length/2, h);
		const vec3 falling = transform.GetOrientation() * vec3(0, 0, h);
		transform.GetPosition() += falling;
		transform.GetOrientation() *= GetPhysics()->GetOriginalBoneTransformation(0).GetOrientation().GetInverse();
		GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyTransform(GetPhysics()->GetBoneGeometry(0)->GetBodyId(), transform);
		if (h <= -launcher_length/2+0.1f) {
			Launch();
		}
	}
	if (GetPosition().z <= -300) {
		GetManager()->PostKillObject(GetInstanceId());
	}
	Parent::OnTick();
}

bool Grenade::TryComplete() {
	bool ok = Parent::TryComplete();
	if (is_loaded_) {
		GetPhysics()->EnableGravity(GetManager()->GetGameManager()->GetPhysicsManager(), false);
		time_frame_created_ = GetManager()->GetGameManager()->GetTimeManager()->GetCurrentPhysicsFrame();
	}
	return ok;
}

void Grenade::OnForceApplied(cure::ContextObject* other_object,
	tbc::PhysicsManager::BodyID own_body_id, tbc::PhysicsManager::BodyID other_body_id,
	const vec3& force, const vec3& torque,
	const vec3& position, const vec3& relative_velocity) {
	(void)own_body_id;
	(void)other_body_id;
	(void)relative_velocity;

	if (exploded_) {
		return;
	}
	exploded_ = true;

	GetManager()->PostKillObject(GetInstanceId());
	((Game*)GetManager()->GetGameManager())->Detonate(force, torque, position,
		this, (cure::ContextObject*)other_object, own_body_id, other_body_id);
}

void Grenade::LoadPlaySound3d(UiCure::UserSound3dResource* sound_resource) {
	deb_assert(sound_resource->GetLoadState() == cure::kResourceLoadComplete);
	if (sound_resource->GetLoadState() == cure::kResourceLoadComplete) {
		if (sound_resource == shreek_sound_) {
			ui_manager_->GetSoundManager()->SetSoundPosition(sound_resource->GetData(), GetPosition(), GetVelocity());
			ui_manager_->GetSoundManager()->Play(sound_resource->GetData(), 0.7f, 1.0);
		} else {
			vec3 _position;
			vec3 velocity;
			((Game*)GetManager()->GetGameManager())->GetVehicleMotion(_position, velocity);
			ui_manager_->GetSoundManager()->SetSoundPosition(sound_resource->GetData(), _position, velocity);
			ui_manager_->GetSoundManager()->Play(sound_resource->GetData(), 5.0f, 1.0);
		}
	}
}



loginstance(kGameContextCpp, Grenade);



}
