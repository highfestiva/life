
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "CutieElevator.h"
#include "../../../TBC/Include/PhysicsTrigger.h"
#include "Cutie.h"
#include "Game.h"



namespace GrenadeRun
{



CutieElevator::CutieElevator(Game* pGame):
	Parent(pGame->GetContext()),
	mGame(pGame)
{
}

CutieElevator::~CutieElevator()
{
}

void CutieElevator::OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody, const Vector3DF& pNormal)
{
	const TBC::PhysicsTrigger* lTrigger = (const TBC::PhysicsTrigger*)GetTrigger(pTriggerId);
	deb_assert(lTrigger);
	if (lTrigger->GetPriority() > -100)
	{
		Parent::OnTrigger(pTriggerId, pBody, pNormal);
		return;
	}
	if (!pBody || !mGame->GetCutie())
	{
		return;
	}
	// We have ourselves a conditional: only Cutie allowed to open.
	Cure::ContextObject* lObject = (Cure::ContextObject*)pBody;
	if (lObject->GetInstanceId() == mGame->GetCutie()->GetInstanceId())
	{
		Parent::OnTrigger(pTriggerId, pBody, pNormal);
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, CutieElevator);



}