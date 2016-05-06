
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "spawner.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/gamemanager.h"
#include "../lepra/include/random.h"
#include "../tbc/include/physicsspawner.h"
#include "game.h"



namespace grenaderun {



Spawner::Spawner(cure::ContextManager* manager):
	cure::CppContextObject(manager->GetGameManager()->GetResourceManager(), "Spawner") {
	manager->AddLocalObject(this);
}

Spawner::~Spawner() {
}



void Spawner::SetSpawner(const tbc::PhysicsSpawner* spawner) {
	Parent::SetSpawner(spawner);

	for (int x = 0; x < GetSpawner()->GetNumber(); ++x) {
		const str spawn_object = GetSpawner()->GetSpawnObject(Random::Uniform(0.0f, 1.0f));
		ContextObject* object = GetManager()->GetGameManager()->CreateContextObject(spawn_object, cure::kNetworkObjectLocallyControlled);
		AddChild(object);
		vec3 initial_velocity;
		object->SetInitialTransform(GetSpawner()->GetSpawnPoint(parent_->GetPhysics(), vec3(0.5f,0.5f,0.5f), 0, initial_velocity));
		object->SetRootVelocity(initial_velocity);
		object->StartLoading();
	}
}



loginstance(kGameContextCpp, Spawner);




}
