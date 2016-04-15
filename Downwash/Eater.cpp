
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Eater.h"
#include "../Cure/Include/ContextManager.h"



namespace Downwash
{



Eater::Eater(Cure::ContextManager* pManager):
	Parent(pManager->GetGameManager()->GetResourceManager(), "Eater")
{
	pManager->AddLocalObject(this);
}

Eater::~Eater()
{
}



void Eater::OnTrigger(Tbc::PhysicsManager::BodyID, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID, const vec3&, const vec3&)
{
	if (pOtherObject->GetClassId().find("helicopter") == str::npos)
	{
		GetManager()->PostKillObject(pOtherObject->GetInstanceId());
	}
}



loginstance(GAME_CONTEXT_CPP, Eater);




}
