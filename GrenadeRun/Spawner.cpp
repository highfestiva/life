
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#include "Spawner.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/GameManager.h"
#include "../Lepra/Include/Random.h"
#include "../TBC/Include/PhysicsSpawner.h"
#include "Game.h"



namespace GrenadeRun
{



Spawner::Spawner(Cure::ContextManager* pManager):
	Cure::CppContextObject(pManager->GetGameManager()->GetResourceManager(), _T("Spawner"))
{
	pManager->AddLocalObject(this);
}

Spawner::~Spawner()
{
}



void Spawner::SetSpawner(const TBC::PhysicsSpawner* pSpawner)
{
	Parent::SetSpawner(pSpawner);

	for (int x = 0; x < GetSpawner()->GetNumber(); ++x)
	{
		const str lSpawnObject = GetSpawner()->GetSpawnObject((float)Random::Uniform(0, 1));
		ContextObject* lObject = GetManager()->GetGameManager()->CreateContextObject(lSpawnObject, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
		AddChild(lObject);
		lObject->SetInitialTransform(GetSpawner()->GetSpawnPoint(mParent->GetPhysics(), Vector3DF(0.5f,0.5f,0.5f)));
		lObject->StartLoading();
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Spawner);




}