
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

void VehicleElevator::OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pOtherObject, TBC::PhysicsManager::BodyID pBodyId, const Vector3DF& pNormal)
{
	const TBC::PhysicsTrigger* lTrigger = (const TBC::PhysicsTrigger*)GetTrigger(pTriggerId);
	deb_assert(lTrigger);
	if (lTrigger->GetPriority() > -100)
	{
		Parent::OnTrigger(pTriggerId, pOtherObject, pBodyId, pNormal);
		return;
	}
	if (!pOtherObject || !mGame->GetVehicle())
	{
		return;
	}
	// We have ourselves a conditional: only Vehicle allowed to open.
	Cure::ContextObject* lObject = (Cure::ContextObject*)pOtherObject;
	if (lObject->GetInstanceId() == mGame->GetVehicle()->GetInstanceId())
	{
		Parent::OnTrigger(pTriggerId, pOtherObject, pBodyId, pNormal);
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, VehicleElevator);



}