
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "spawner.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/gamemanager.h"
#include "../cure/include/runtimevariable.h"
#include "../tbc/include/physicsspawner.h"
#include "rtvar.h"



namespace life {



Spawner::Spawner(cure::ContextManager* manager):
	Parent(manager) {
}

Spawner::~Spawner() {
}

int Spawner::GetSpawnCount() const {
	float spawn_part;
	v_get(spawn_part, =(float), GetManager()->GetGameManager()->GetVariableScope(), kRtvarGameSpawnpart, 1.0);
	return (int)(GetSpawner()->GetNumber() * spawn_part);
}



}
