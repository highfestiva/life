
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uijetengineemitter.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/gamemanager.h"
#include "../../cure/include/runtimevariable.h"
#include "../../lepra/include/random.h"
#include "../../tbc/include/physicsengine.h"
#include "../../uicure/include/uigameuimanager.h"
#include "../../uitbc/include/uiparticlerenderer.h"
#include "../include/uiprops.h"
#include "../include/uiruntimevariablename.h"



namespace UiCure {



JetEngineEmitter::JetEngineEmitter(cure::ResourceManager* resource_manager, GameUiManager* ui_manager):
	resource_manager_(resource_manager),
	ui_manager_(ui_manager),
	interleave_timeout_(0) {
}

JetEngineEmitter::~JetEngineEmitter() {
	resource_manager_ = 0;
	ui_manager_ = 0;
}



void JetEngineEmitter::EmitFromTag(const CppContextObject* object, const uitbc::ChunkyClass::Tag& tag, float frame_time) {
	bool particles_enabled;
	v_get(particles_enabled, =, UiCure::GetSettings(), kRtvarUi3DEnableparticles, false);
	if (!particles_enabled) {
		return;
	}

	enum FloatValue {
		kFvStartR = 0,
		kFvStartG,
		kFvStartB,
		kFvEndR,
		kFvEndG,
		kFvEndB,
		kFvX,
		kFvY,
		kFvZ,
		kFvRadiusX,
		kFvRadiusY,
		kFvRadiusZ,
		kFvScaleX,
		kFvScaleY,
		kFvScaleZ,
		kFvDirectionX,
		kFvDirectionY,
		kFvDirectionZ,
		kFvDensity,
		kFvOpacity,
		kFvOvershootOpacity,
		kFvOvershootCutoffDot,
		kFvOvershootDistanceUpscale,
		kFvOvershootEngineFactorBase,
		kFvCount
	};
	if (tag.float_value_list_.size() != kFvCount ||
		tag.string_value_list_.size() != 0 ||
		tag.body_index_list_.size() != 0 ||
		tag.engine_index_list_.size() != 1 ||
		tag.mesh_index_list_.size() < 1) {
		log_.Errorf("The fire tag '%s' has the wrong # of parameters.", tag.tag_name_.c_str());
		deb_assert(false);
		return;
	}
	const int engine_index = tag.engine_index_list_[0];
	if (engine_index >= object->GetPhysics()->GetEngineCount()) {
		return;
	}
	const tbc::PhysicsEngine* engine = object->GetPhysics()->GetEngine(engine_index);
	const float throttle_up_speed = Math::GetIterateLerpTime(tag.float_value_list_[kFvOvershootEngineFactorBase]*0.5f, frame_time);
	const float throttle_down_speed = Math::GetIterateLerpTime(tag.float_value_list_[kFvOvershootEngineFactorBase], frame_time);
	const float engine_throttle = engine->GetLerpThrottle(throttle_up_speed, throttle_down_speed, true);
	const quat orientation = object->GetOrientation();
	vec3 _radius(tag.float_value_list_[kFvRadiusX], tag.float_value_list_[kFvRadiusY], tag.float_value_list_[kFvRadiusZ]);
	_radius.x *= Math::Lerp(1.0f, tag.float_value_list_[kFvScaleX], engine_throttle);
	_radius.y *= Math::Lerp(1.0f, tag.float_value_list_[kFvScaleY], engine_throttle);
	_radius.z *= Math::Lerp(1.0f, tag.float_value_list_[kFvScaleZ], engine_throttle);
	vec3 _position(tag.float_value_list_[kFvX], tag.float_value_list_[kFvY], tag.float_value_list_[kFvZ]);
	_position = orientation * _position;
	const vec3 _color(tag.float_value_list_[kFvEndR], tag.float_value_list_[kFvEndB], tag.float_value_list_[kFvEndB]);

	bool create_particle = false;
	const float density = tag.float_value_list_[kFvDensity];
	float exhaust_intensity;
	v_get(exhaust_intensity, =(float), UiCure::GetSettings(), kRtvarUi3DExhaustintensity, 1.0);
	interleave_timeout_ -= Math::Lerp(0.3f, 1.0f, engine_throttle) * exhaust_intensity * frame_time;
	if (interleave_timeout_ <= 0) {	// Release particle this frame?
		create_particle = true;
		interleave_timeout_ = 0.05f / density;
	} else {
		create_particle = false;
	}

	const float dx = tag.float_value_list_[kFvRadiusX];
	const float dy = tag.float_value_list_[kFvRadiusY];
	const float dz = tag.float_value_list_[kFvRadiusZ];
	const vec3 start_color(tag.float_value_list_[kFvStartR], tag.float_value_list_[kFvStartB], tag.float_value_list_[kFvStartB]);
	const float _opacity = tag.float_value_list_[kFvOpacity];
	const vec3 direction = orientation * vec3(tag.float_value_list_[kFvDirectionX], tag.float_value_list_[kFvDirectionY], tag.float_value_list_[kFvDirectionZ]);
	const vec3 velocity = direction + object->GetVelocity();
	uitbc::ParticleRenderer* particle_renderer = (uitbc::ParticleRenderer*)ui_manager_->GetRenderer()->GetDynamicRenderer("particle");
	const float particle_time = density;
	float particle_size;	// Pick second size.
	if (dx > dy && dy > dz) {
		particle_size = dy;
	} else if (dy > dx && dx > dz) {
		particle_size = dx;
	} else {
		particle_size = dz;
	}
	particle_size *= 0.2f;

	const float _distance_scale_factor = tag.float_value_list_[kFvOvershootDistanceUpscale];
	for (size_t y = 0; y < tag.mesh_index_list_.size(); ++y) {
		tbc::GeometryBase* mesh = object->GetMesh(tag.mesh_index_list_[y]);
		if (mesh) {
			int phys_index = -1;
			str mesh_name;
			xform transform;
			float mesh_scale;
			((uitbc::ChunkyClass*)object->GetClass())->GetMesh(tag.mesh_index_list_[y], phys_index, mesh_name, transform, mesh_scale);
			transform = mesh->GetBaseTransformation() * transform;
			vec3 mesh_pos = transform.GetPosition() + _position;

			const vec3 cam_distance = mesh_pos - ui_manager_->GetRenderer()->GetCameraTransformation().GetPosition();
			const float distance = cam_distance.GetLength();
			const vec3 cam_direction(cam_distance / distance);
			float overshoot_factor = -(cam_direction*direction);
			if (overshoot_factor > tag.float_value_list_[kFvOvershootCutoffDot]) {
				overshoot_factor = Math::Lerp(overshoot_factor*0.5f, overshoot_factor, engine_throttle);
				const float opacity = (overshoot_factor+0.6f) * tag.float_value_list_[kFvOvershootOpacity];
				DrawOvershoot(mesh_pos, _distance_scale_factor*distance, _radius, _color, opacity, cam_direction);
			}

			if (create_particle) {
				const float sx = Random::Normal(0.0f, dx*0.5f, -dx, +dx);
				const float sy = Random::Normal(0.0f, dy*0.5f, -dy, +dy);
				const float sz = Random::Normal(0.0f, dz*0.5f, -dz, +dz);
				mesh_pos += orientation * vec3(sx, sy, sz);
				particle_renderer->CreateGlow(particle_time, particle_size, start_color, _color, _opacity, mesh_pos, velocity);
			}
		}
	}
}



void JetEngineEmitter::DrawOvershoot(const vec3& position, float distance_scale_factor, const vec3& radius, const vec3& color, float opacity, const vec3& camera_direction) {
	float s = std::max(std::max(radius.x, radius.y), radius.z);
	uitbc::ParticleRenderer* particle_renderer = (uitbc::ParticleRenderer*)ui_manager_->GetRenderer()->GetDynamicRenderer("particle");
	const float max_flame_distance = 3 * s;
	s += s * distance_scale_factor * 0.1f;
	particle_renderer->RenderFireBillboard(0, s, color, opacity, position-camera_direction*max_flame_distance);
}



loginstance(kGameContext, JetEngineEmitter);



}
