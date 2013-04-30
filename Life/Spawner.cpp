
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "Spawner.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/GameManager.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../TBC/Include/PhysicsSpawner.h"
#include "RtVar.h"



namespace Life
{



Spawner::Spawner(Cure::ContextManager* pManager):
	Parent(pManager)
{
}

Spawner::~Spawner()
{
}

int Spawner::GetSpawnCount() const
{
	float lSpawnPart;
	CURE_RTVAR_GET(lSpawnPart, =(float), GetManager()->GetGameManager()->GetVariableScope(), RTVAR_GAME_SPAWNPART, 1.0);
	return (int)(GetSpawner()->GetNumber() * lSpawnPart);
}



}
