
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "VehicleElevator.h"
#include "../TBC/Include/PhysicsTrigger.h"
#include "Vehicle.h"
#include "Game.h"



namespace TireFire
{



VehicleElevator::VehicleElevator(Game* pGame):
	Parent(pGame->GetContext()),
	mGame(pGame)
{
}

VehicleElevator::~VehicleElevator()
{
}

void VehicleElevator::OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody, const Vector3DF& pNormal)
{
	const TBC::PhysicsTrigger* lTrigger = (const TBC::PhysicsTrigger*)GetTrigger(pTriggerId);
	deb_assert(lTrigger);
	if (lTrigger->GetPriority() > -100)
	{
		Parent::OnTrigger(pTriggerId, pBody, pNormal);
		return;
	}
	if (!pBody || !mGame->GetVehicle())
	{
		return;
	}
	// We have ourselves a conditional: only Vehicle allowed to open.
	Cure::ContextObject* lObject = (Cure::ContextObject*)pBody;
	if (lObject->GetInstanceId() == mGame->GetVehicle()->GetInstanceId())
	{
		Parent::OnTrigger(pTriggerId, pBody, pNormal);
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, VehicleElevator);



}