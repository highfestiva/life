
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "eater.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/health.h"
#include "basemachine.h"



namespace Fire {



Eater::Eater(cure::ContextManager* manager):
	Parent(manager->GetGameManager()->GetResourceManager(), "Eater") {
	manager->AddLocalObject(this);
}

Eater::~Eater() {
}



void Eater::OnTrigger(tbc::PhysicsManager::BodyID, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3&, const vec3&) {
	if (other_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId() == body_id) {	// Only if whole car enters "goal", not only wheel.
		BaseMachine* machine = dynamic_cast<BaseMachine*>(other_object);
		if (machine && !machine->did_get_to_town_) {
			machine->did_get_to_town_ = true;
			if (!machine->villain_.empty() && cure::Health::Get(machine) > 0) {
				((FireManager*)GetManager()->GetGameManager())->OnLetThroughTerrorist(machine);
			}
		}

		//GetManager()->PostKillObject(other_object->GetInstanceId());
	}
}



loginstance(kGameContextCpp, Eater);




}
