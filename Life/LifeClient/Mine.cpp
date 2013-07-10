
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "Mine.h"
#include "../ProjectileUtil.h"



namespace Life
{



Mine::Mine(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Launcher* pLauncher):
	Parent(pResourceManager, pClassId, pUiManager),
	mLauncher(pLauncher),
	mIsDetonated(false)
{
}

Mine::~Mine()
{
	ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, GetPosition(), GetVelocity(), Vector3DF(), 2);
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Mine);



}
