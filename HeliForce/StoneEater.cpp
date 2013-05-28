
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "StoneEater.h"
#include "../Cure/Include/ContextManager.h"



namespace HeliForce
{



StoneEater::StoneEater(Cure::ContextManager* pManager):
	Parent(pManager->GetGameManager()->GetResourceManager(), _T("StoneEater"))
{
	pManager->AddLocalObject(this);
}

StoneEater::~StoneEater()
{
}



void StoneEater::OnTrigger(TBC::PhysicsManager::TriggerID, ContextObject* pBody, const Vector3DF&)
{
	if (pBody->GetClassId().find(_T("stone")) != str::npos)
	{
		GetManager()->PostKillObject(pBody->GetInstanceId());
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, StoneEater);




}
