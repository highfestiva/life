
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Level.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/ContextPath.h"
#include "Game.h"



namespace GrenadeRun
{



Level::Level(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Cure::ContextForceListener* pGravelEmitter):
	Parent(pResourceManager, pClassId, pUiManager),
	mPath(0),
	mGravelEmitter(pGravelEmitter)
{
}

Level::~Level()
{
	delete mGravelEmitter;
	mGravelEmitter = 0;
	mPath = 0;
}



Cure::ContextPath* Level::QueryPath()
{
	if (!mPath)
	{
		mPath = new Cure::ContextPath(GetResourceManager(), "ContextPath");
		GetManager()->AddLocalObject(mPath);
	}
	return mPath;
}



void Level::OnForceApplied(Cure::ContextObject* pOtherObject,
	Tbc::PhysicsManager::BodyID pOwnBodyId, Tbc::PhysicsManager::BodyID pOtherBodyId,
	const vec3& pForce, const vec3& pTorque,
	const vec3& pPosition, const vec3& pRelativeVelocity)
{
	Parent::OnForceApplied(pOtherObject, pOwnBodyId, pOtherBodyId, pForce, pTorque, pPosition, pRelativeVelocity);

	mGravelEmitter->OnForceApplied(this, pOtherObject, pOwnBodyId, pOtherBodyId, pForce, pTorque, pPosition, pRelativeVelocity);
}



loginstance(GAME_CONTEXT_CPP, Level);



}
