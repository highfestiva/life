
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uiprops.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/gamemanager.h"
#include "../../cure/include/timemanager.h"
#include "../../lepra/include/random.h"
#include "../include/uigameuimanager.h"



namespace UiCure {



Props::Props(cure::ResourceManager* resource_manager, const str& class_id, GameUiManager* ui_manager):
	Parent(resource_manager, class_id, ui_manager),
	particle_type_(kParticleNone),
	scale_(1),
	time_(0),
	life_time_(2),
	fade_out_time_(0.6f),
	opacity_(1),
	is_fading_out_(false) {
	EnablePixelShader(false);
	SetPhysicsTypeOverride(cure::kPhysicsOverrideBones);
}

Props::~Props() {
}



void Props::SetOpacity(float opacity) {
	opacity_ = opacity;
}

void Props::StartParticle(ParticleType particle_type, const vec3& start_velocity, float scale, float angular_range, float time) {
	//deb_assert(start_velocity.GetLengthSquared() < 1000*1000);
	particle_type_ = particle_type;
	velocity_ = start_velocity;
	scale_ = scale;
	angular_velocity_.Set(Random::Uniform(-angular_range, angular_range),
		Random::Uniform(-angular_range, angular_range),
		Random::Uniform(-angular_range*0.1f, angular_range*0.1f));
	life_time_ = time;
	GetManager()->AddGameAlarmCallback(this, 5, life_time_, 0);
}

void Props::SetFadeOutTime(float time) {
	fade_out_time_ = time;
}


void Props::DispatchOnLoadMesh(UserGeometryReferenceResource* mesh_resource) {
	Parent::DispatchOnLoadMesh(mesh_resource);
	if (particle_type_ != kParticleNone && mesh_resource->GetLoadState() == cure::kResourceLoadComplete) {
		mesh_resource->GetRamData()->SetExcludeCulling();
		mesh_resource->GetRamData()->SetScale(Random::Uniform(0.4f, 1.6f) * scale_);
	}
}

void Props::TryAddTexture() {
	for (size_t x = 0; x < mesh_resource_array_.size(); ++x) {
		if (mesh_resource_array_[x]->GetLoadState() == cure::kResourceLoadComplete) {
			if (particle_type_ == kParticleGas) {
				mesh_resource_array_[x]->GetRamData()->GetBasicMaterialSettings().alpha_ = 0.01f;
			} else if (particle_type_ == kParticleSolid) {
				mesh_resource_array_[x]->GetRamData()->GetBasicMaterialSettings().alpha_ = opacity_;
			}
		}
	}
	Parent::TryAddTexture();
}

void Props::OnTick() {
	const float frame_time = GetManager()->GetGameManager()->GetTimeManager()->GetNormalFrameTime();
	time_ += frame_time;
	switch (particle_type_) {
		case kParticleSolid: {
			SetRootPosition(GetPosition() + velocity_*frame_time);
			velocity_.z -= 9.82f*frame_time;

			quat orientation = GetOrientation();
			orientation.RotateAroundOwnX(frame_time * angular_velocity_.x * velocity_.x);
			orientation.RotateAroundOwnY(frame_time * angular_velocity_.y * velocity_.y);
			orientation.RotateAroundOwnZ(frame_time * angular_velocity_.z * velocity_.z);
			SetRootOrientation(orientation);

			const float time_left = life_time_-time_;
			if (time_left < fade_out_time_) {
				const float fade_out_opacity = opacity_ * time_left / fade_out_time_;
				for (size_t x = 0; x < mesh_resource_array_.size(); ++x) {
					UserGeometryReferenceResource* mesh = mesh_resource_array_[x];
					if (!is_fading_out_ && opacity_ >= 1) {
						if (ui_manager_->CanRender()) {
							ui_manager_->GetRenderer()->ChangeMaterial(mesh->GetData(), uitbc::Renderer::kMatSingleColorBlended);
						}
					}
					mesh->GetRamData()->GetBasicMaterialSettings().alpha_ = fade_out_opacity;
				}
				is_fading_out_ = true;
			}
		} break;
		case kParticleGas: {
			const float lerp = Math::GetIterateLerpTime(0.08f, frame_time);
			velocity_.x = Math::Lerp(velocity_.x, 0.0f, lerp);
			velocity_.y = Math::Lerp(velocity_.y, 0.0f, lerp);
			velocity_.z = Math::Lerp(velocity_.z, 3.0f, lerp);
			SetRootPosition(GetPosition() + velocity_*frame_time);

			quat orientation = GetOrientation();
			orientation.RotateAroundOwnX(frame_time * angular_velocity_.x * velocity_.x);
			orientation.RotateAroundOwnY(frame_time * angular_velocity_.y * velocity_.y);
			orientation.RotateAroundOwnZ(frame_time * angular_velocity_.z * velocity_.z);
			SetRootOrientation(orientation);

			const float gas_opacity = opacity_ * sin(time_/life_time_*PIF);
			for (size_t x = 0; x < mesh_resource_array_.size(); ++x) {
				if (mesh_resource_array_[x]->GetLoadState() == cure::kResourceLoadComplete) {
					mesh_resource_array_[x]->GetRamData()->GetBasicMaterialSettings().alpha_ = gas_opacity;
				}
			}
		} break;
	}

	Parent::OnTick();	// TRICKY: not a vehicle in this sense.
}

void Props::OnAlarm(int alarm_id, void* /*extra_data*/) {
	if (alarm_id == 5) {
		deb_assert(particle_type_ != kParticleNone);
		GetManager()->PostKillObject(GetInstanceId());
	}
}



loginstance(kGameContextCpp, Props);



}
