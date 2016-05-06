
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "servermine.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/health.h"
#include "../../cure/include/gamemanager.h"
#include "../launcher.h"
#include "../projectileutil.h"



namespace life {



ServerMine::ServerMine(cure::ResourceManager* resource_manager, const str& class_id, Launcher* launcher):
	Parent(resource_manager, class_id),
	launcher_(launcher),
	ticks_til_fully_activated_(100),
	ticks_til_detonation_(-1),
	is_detonated_(false) {
	cure::Health::Set(this, 0.34f);
}

ServerMine::~ServerMine() {
}



void ServerMine::OnTick() {
	if (ticks_til_fully_activated_ > 0) {
		--ticks_til_fully_activated_;
	}

	if (ticks_til_detonation_ >= 0) {
		if (--ticks_til_detonation_ == 0) {
			ProjectileUtil::Detonate(this, &is_detonated_, launcher_, GetPosition(), GetVelocity(), vec3(), 1, 0);
		}
		return;
	}

	const float health = cure::Health::Get(this);
	if (health <= -0.5f) {
		ticks_til_detonation_ = 1;
	} else if (health <= -0.1f) {
		ticks_til_detonation_ = 2;
	} else if (health <= -0.05f) {
		ticks_til_detonation_ = 6;
	} else if (health <= 0) {
		ticks_til_detonation_ = 12;
	}
}

void ServerMine::OnForceApplied(cure::ContextObject* other_object,
	tbc::PhysicsManager::BodyID own_body_id, tbc::PhysicsManager::BodyID other_body_id,
	const vec3& force, const vec3& torque,
	const vec3& position, const vec3& relative_velocity) {
	(void)other_object;
	(void)own_body_id;
	(void)other_body_id;
	(void)torque;
	(void)position;
	(void)relative_velocity;

	float _force = force.GetLength()/GetMass() * 0.002f;
	if (ticks_til_fully_activated_ == 0 ||
		(other_object->GetPhysics()->GetPhysicsType() == tbc::ChunkyPhysics::kDynamic &&
		!dynamic_cast<ServerMine*>(other_object))) {
		_force *= 10;
	}
	if (_force > 1) {
		cure::Health::Add(this, _force * -0.045f, true);
	}
}



loginstance(kGameContextCpp, ServerMine);



}
