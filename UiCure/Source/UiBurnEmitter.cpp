
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uiburnemitter.h"
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



BurnEmitter::BurnEmitter(cure::ResourceManager* resource_manager, GameUiManager* ui_manager):
	resource_manager_(resource_manager),
	ui_manager_(ui_manager),
	burn_timeout_(0),
	free_flow_(false) {
}

BurnEmitter::~BurnEmitter() {
	resource_manager_ = 0;
	ui_manager_ = 0;
}



void BurnEmitter::EmitFromTag(const CppContextObject* object, const uitbc::ChunkyClass::Tag& tag, float frame_time, float intensity) {
	bool particles_enabled;
	v_get(particles_enabled, =, UiCure::GetSettings(), kRtvarUi3DEnableparticles, false);
	if (!particles_enabled || intensity <= 0) {
		return;
	}

	enum FloatValue {
		kFvX = 0,
		kFvY,
		kFvZ,
		kFvVx,
		kFvVy,
		kFvVz,
		kFvScale,
		kFvDensity,
		kFvOpacity,
		kFvTtl,
		kFvCount
	};
	if (tag.float_value_list_.size() != kFvCount ||
		tag.string_value_list_.size() != 0 ||
		tag.engine_index_list_.size() != 0 ||
		tag.body_index_list_.size() != 0 ||
		tag.mesh_index_list_.size() < 1) {
		log_.Errorf("The burn tag '%s' has the wrong # of parameters.", tag.tag_name_.c_str());
		deb_assert(false);
		return;
	}
	float burn_intensity;
	v_get(burn_intensity, =(float), UiCure::GetSettings(), kRtvarUi3DExhaustintensity, 1.0);
	burn_timeout_ -= Math::Lerp(0.4f, 1.0f, intensity) * burn_intensity * frame_time * 25;
	if (burn_timeout_ > 0) {
		return;
	}
	const float density = tag.float_value_list_[kFvDensity];
	burn_timeout_ = 1/density;

	const float scale = tag.float_value_list_[kFvScale];
	const float life_time = tag.float_value_list_[kFvTtl];

	const quat original_orientation = object->GetOrientation();
	vec3 offset(tag.float_value_list_[kFvX], tag.float_value_list_[kFvY], tag.float_value_list_[kFvZ]);
	offset = original_orientation*offset;
	vec3 velocity(tag.float_value_list_[kFvVx], tag.float_value_list_[kFvVy], tag.float_value_list_[kFvVz]);
	const float opacity = Math::Lerp(0.6f, 1.0f, intensity) * tag.float_value_list_[kFvOpacity];
	if (!free_flow_) {
		velocity = original_orientation*velocity;
	} else {
		velocity.Set(0, 0, velocity.GetLength()*0.6f);
	}
	velocity += object->GetVelocity();
	uitbc::ParticleRenderer* particle_renderer = (uitbc::ParticleRenderer*)ui_manager_->GetRenderer()->GetDynamicRenderer("particle");
	for (size_t y = 0; y < tag.mesh_index_list_.size(); ++y) {
		tbc::GeometryBase* mesh = object->GetMesh(tag.mesh_index_list_[y]);
		if (mesh) {
			int phys_index = -1;
			str mesh_name;
			xform transform;
			float mesh_scale;
			((uitbc::ChunkyClass*)object->GetClass())->GetMesh(tag.mesh_index_list_[y], phys_index, mesh_name, transform, mesh_scale);
			transform = mesh->GetBaseTransformation() * transform;
			transform.GetPosition() += offset;

			const float angular_velocity = Random::Uniform(-15.5f, +15.5f);
			particle_renderer->CreateFume(life_time, scale, angular_velocity, opacity, transform.GetPosition(), velocity);
		}
	}
}

void BurnEmitter::SetFreeFlow() {
	free_flow_ = true;
}



loginstance(kGameContext, BurnEmitter);



}
