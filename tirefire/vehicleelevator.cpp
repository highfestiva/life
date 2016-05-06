
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "vehicleelevator.h"
#include "../tbc/include/physicstrigger.h"
#include "vehicle.h"
#include "game.h"



namespace tirefire {



VehicleElevator::VehicleElevator(Game* game):
	Parent(game->GetContext()),
	game_(game) {
}

VehicleElevator::~VehicleElevator() {
}

void VehicleElevator::OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal) {
	const tbc::PhysicsTrigger* trigger = (const tbc::PhysicsTrigger*)GetTrigger(trigger_id);
	deb_assert(trigger);
	if (trigger->GetPriority() > -100) {
		Parent::OnTrigger(trigger_id, other_object, body_id, position, normal);
		return;
	}
	if (!other_object || !game_->GetVehicle()) {
		return;
	}
	// We have ourselves a conditional: only Vehicle allowed to open.
	cure::ContextObject* object = (cure::ContextObject*)other_object;
	if (object->GetInstanceId() == game_->GetVehicle()->GetInstanceId()) {
		Parent::OnTrigger(trigger_id, other_object, body_id, position, normal);
	}
}



loginstance(kGameContextCpp, VehicleElevator);



}