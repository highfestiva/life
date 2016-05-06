
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "eater.h"
#include "../cure/include/contextmanager.h"



namespace Downwash {



Eater::Eater(cure::ContextManager* manager):
	Parent(manager->GetGameManager()->GetResourceManager(), "Eater") {
	manager->AddLocalObject(this);
}

Eater::~Eater() {
}



void Eater::OnTrigger(tbc::PhysicsManager::BodyID, ContextObject* other_object, tbc::PhysicsManager::BodyID, const vec3&, const vec3&) {
	if (other_object->GetClassId().find("helicopter") == str::npos) {
		GetManager()->PostKillObject(other_object->GetInstanceId());
	}
}



loginstance(kGameContextCpp, Eater);




}
