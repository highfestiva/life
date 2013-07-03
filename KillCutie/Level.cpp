
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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
		mPath = new Cure::ContextPath(GetResourceManager(), _T("ContextPath"));
		GetManager()->AddLocalObject(mPath);
	}
	return mPath;
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
