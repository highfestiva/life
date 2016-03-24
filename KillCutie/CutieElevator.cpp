
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "CutieElevator.h"
#include "../Tbc/Include/PhysicsTrigger.h"
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

void CutieElevator::OnTrigger(Tbc::PhysicsManager::BodyID pTriggerId, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pPosition, const vec3& pNormal)
{
	const Tbc::PhysicsTrigger* lTrigger = (const Tbc::PhysicsTrigger*)GetTrigger(pTriggerId);
	deb_assert(lTrigger);
	if (lTrigger->GetPriority() > -100)
	{
		Parent::OnTrigger(pTriggerId, pOtherObject, pBodyId, pPosition, pNormal);
		return;
	}
	if (!pOtherObject || !mGame->GetCutie())
	{
		return;
	}
	// We have ourselves a conditional: only Cutie allowed to open.
	Cure::ContextObject* lObject = (Cure::ContextObject*)pOtherObject;
	if (lObject->GetInstanceId() == mGame->GetCutie()->GetInstanceId())
	{
		Parent::OnTrigger(pTriggerId, pOtherObject, pBodyId, pPosition, pNormal);
	}
}



loginstance(GAME_CONTEXT_CPP, CutieElevator);



}