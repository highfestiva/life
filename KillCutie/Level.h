
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "../Cure/Include/ContextForceListener.h"
#include "Game.h"



namespace Cure
{
class ContextPath;
}



namespace GrenadeRun
{



class Level: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	Level(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Cure::ContextForceListener* pGravelEmitter);
	virtual ~Level();

	Cure::ContextPath* QueryPath();

private:
	virtual void OnForceApplied(Cure::ContextObject* pOtherObject,
		Tbc::PhysicsManager::BodyID pOwnBodyId, Tbc::PhysicsManager::BodyID pOtherBodyId,
		const vec3& pForce, const vec3& pTorque,
		const vec3& pPosition, const vec3& pRelativeVelocity);

	Cure::ContextPath* mPath;
	Cure::ContextForceListener* mGravelEmitter;

	logclass();
};



}
