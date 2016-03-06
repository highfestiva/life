
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "VehicleElevator.h"
#include "../Tbc/Include/PhysicsTrigger.h"
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

void VehicleElevator::OnTrigger(Tbc::PhysicsManager::BodyID pTriggerId, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pPosition, const vec3& pNormal)
{
	const Tbc::PhysicsTrigger* lTrigger = (const Tbc::PhysicsTrigger*)GetTrigger(pTriggerId);
	deb_assert(lTrigger);
	if (lTrigger->GetPriority() > -100)
	{
		Parent::OnTrigger(pTriggerId, pOtherObject, pBodyId, pPosition, pNormal);
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
		Parent::OnTrigger(pTriggerId, pOtherObject, pBodyId, pPosition, pNormal);
	}
}



loginstance(GAME_CONTEXT_CPP, VehicleElevator);



}