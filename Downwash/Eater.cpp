
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "Eater.h"
#include "../Cure/Include/ContextManager.h"



namespace Downwash
{



Eater::Eater(Cure::ContextManager* pManager):
	Parent(pManager->GetGameManager()->GetResourceManager(), _T("Eater"))
{
	pManager->AddLocalObject(this);
}

Eater::~Eater()
{
}



void Eater::OnTrigger(TBC::PhysicsManager::TriggerID, ContextObject* pOtherObject, TBC::PhysicsManager::BodyID, const Vector3DF&)
{
	if (pOtherObject->GetClassId().find(_T("helicopter")) == str::npos)
	{
		GetManager()->PostKillObject(pOtherObject->GetInstanceId());
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Eater);




}
