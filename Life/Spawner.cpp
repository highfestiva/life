
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Spawner.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/GameManager.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Tbc/Include/PhysicsSpawner.h"
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
	v_get(lSpawnPart, =(float), GetManager()->GetGameManager()->GetVariableScope(), RTVAR_GAME_SPAWNPART, 1.0);
	return (int)(GetSpawner()->GetNumber() * lSpawnPart);
}



}
