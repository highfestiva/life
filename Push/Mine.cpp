
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Mine.h"
#include "../Life/ProjectileUtil.h"



namespace Push
{



Mine::Mine(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Life::Launcher* pLauncher):
	Parent(pResourceManager, pClassId, pUiManager),
	mLauncher(pLauncher),
	mIsDetonated(false)
{
}

Mine::~Mine()
{
	Life::ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, GetPosition(), GetVelocity(), Vector3DF(), 1);
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Mine);



}
