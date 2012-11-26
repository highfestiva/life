
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Level.h"



namespace Push
{



Level::Level(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Cure::ContextForceListener* pGravelEmitter):
	Parent(pResourceManager, pClassId, pUiManager),
	mGravelEmitter(pGravelEmitter)
{
}

Level::~Level()
{
	delete mGravelEmitter;
	mGravelEmitter = 0;
}



void Level::OnForceApplied(Cure::ContextObject* pOtherObject,
	TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
	const Vector3DF& pForce, const Vector3DF& pTorque,
	const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity)
{
	Parent::OnForceApplied(pOtherObject, pOwnBodyId, pOtherBodyId, pForce, pTorque, pPosition, pRelativeVelocity);

	mGravelEmitter->OnForceApplied(this, pOtherObject, pOwnBodyId, pOtherBodyId, pForce, pTorque, pPosition, pRelativeVelocity);
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Level);



}
