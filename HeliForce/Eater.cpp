
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "Eater.h"
#include "../Cure/Include/ContextManager.h"



namespace HeliForce
{



Eater::Eater(Cure::ContextManager* pManager):
	Parent(pManager->GetGameManager()->GetResourceManager(), _T("Eater"))
{
	pManager->AddLocalObject(this);
}

Eater::~Eater()
{
}



void Eater::OnTrigger(TBC::PhysicsManager::TriggerID, ContextObject* pBody, const Vector3DF&)
{
	if (pBody->GetClassId().find(_T("helicopter")) == str::npos)
	{
		GetManager()->PostKillObject(pBody->GetInstanceId());
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Eater);




}
