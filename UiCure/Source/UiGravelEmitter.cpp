
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uigravelemitter.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/gamemanager.h"
#include "../../cure/include/runtimevariable.h"
#include "../../lepra/include/random.h"
#include "../../tbc/include/chunkybonegeometry.h"
#include "../../tbc/include/chunkyphysics.h"
#include "../../uitbc/include/uiparticlerenderer.h"
#include "../include/uigameuimanager.h"
#include "../include/uiprops.h"
#include "../include/uiruntimevariablename.h"



namespace UiCure {



GravelEmitter::GravelEmitter(cure::ResourceManager* resource_manager, GameUiManager* ui_manager, float sensitivity, float scale, float amount, float life_time):
	resource_manager_(resource_manager),
	ui_manager_(ui_manager),
	sensitivity_factor_(1/sensitivity),
	scale_(scale),
	delay_(1/amount),
	life_time_(life_time) {
}

GravelEmitter::~GravelEmitter() {
	resource_manager_ = 0;
	ui_manager_ = 0;
}



void GravelEmitter::OnForceApplied(cure::ContextObject* object, cure::ContextObject* other_object,
	tbc::PhysicsManager::BodyID own_body_id, tbc::PhysicsManager::BodyID other_body_id,
	const vec3& force, const vec3& torque,
	const vec3& position, const vec3& relative_velocity) {
	bool particles_enabled;
	v_get(particles_enabled, =, UiCure::GetSettings(), kRtvarUi3DEnableparticles, false);
	if (!particles_enabled) {
		return;
	}
	if (particle_timer_.QueryTimeDiff() < delay_) {
		return;
	}
	const float relative_speed_limit = 2 * sensitivity_factor_;
	if (relative_velocity.GetLengthSquared() < relative_speed_limit*relative_speed_limit) {
		return;
	}
	if (!other_object) {
		return;
	}
	if (object->GetPhysics()->GetBoneGeometry(own_body_id)->GetMaterial() != "grass") {
		return;
	}
	const float distance = 100;	// Only show gravel particles inside this distance.
	if (!object->GetManager()->GetGameManager()->IsObjectRelevant(position, distance)) {
		return;
	}
	const float impact_factor = other_object->GetPhysics()->GetBoneGeometry(other_body_id)->GetImpactFactor();
	const float impact = other_object->GetImpact(other_object->GetManager()->GetGameManager()->GetPhysicsManager()->GetGravity(),
		force, torque, 0, 11) * impact_factor;
	if (impact < 0.1f*sensitivity_factor_) {
		return;
	}
	vec3 _position(position);
	const float angle = Random::Uniform(0.0f, PIF*2);
	_position.x += 0.2f * cos(angle);
	_position.y += 0.2f * sin(angle);
	_position.z += Random::Uniform(+0.1f, +0.2f);
	vec3 _relative_velocity(relative_velocity);
	const vec3 up(0, 0, 1);
	vec3 _torque(torque.Cross(up));
	const float mass_factor = 1/other_object->GetMass();
	_relative_velocity += _torque * mass_factor * 0.1f;
	vec3 rotation_speed;
	other_object->GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyAngularVelocity(other_body_id, rotation_speed);
	const vec3 radius = position - other_object->GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyPosition(other_body_id);
	const vec3 roll_speed(rotation_speed.Cross(radius) * 0.2f);
	_position += roll_speed.GetNormalized(0.3f);
	const float roll_length = roll_speed.GetLength();
	const float collision_length = _relative_velocity.GetLength();
	_relative_velocity += roll_speed;
	_relative_velocity.z += collision_length*0.2f + roll_length*0.2f;
	_relative_velocity += RNDVEC(collision_length*0.2f);
	if (_relative_velocity.GetLengthSquared() < relative_velocity.GetLengthSquared()*200*200) {
		uitbc::ParticleRenderer* particle_renderer = (uitbc::ParticleRenderer*)ui_manager_->GetRenderer()->GetDynamicRenderer("particle");
		const float angular_velocity = Random::Uniform(-5.0f, 5.0f);
		particle_renderer->CreatePebble(life_time_, scale_, angular_velocity, vec3(0.3f, 0.15f, 0.0f), _position, _relative_velocity);
		particle_timer_.ClearTimeDiff();
	}
}



loginstance(kGameContext, GravelEmitter);



}
