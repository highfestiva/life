
// Author: Jonas Bystr�m
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "../Cure/Include/ContextForceListener.h"
#include "Push.h"



namespace Push
{



class Level: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	Level(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Cure::ContextForceListener* pGravelEmitter);
	virtual ~Level();

private:
	virtual void OnForceApplied(Cure::ContextObject* pOtherObject,
		TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque,
		const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);

	Cure::ContextForceListener* mGravelEmitter;

	LOG_CLASS_DECLARE();
};



}