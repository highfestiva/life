
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "ExplodingMachine.h"
#include "../../Cure/Include/Health.h"
#include "../ProjectileUtil.h"



namespace Life
{



ExplodingMachine::ExplodingMachine(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Launcher* pLauncher):
	Parent(pResourceManager, pClassId, pUiManager),
	mLauncher(pLauncher),
	mIsDetonated(false)
{
}

ExplodingMachine::~ExplodingMachine()
{
}



void ExplodingMachine::OnTick()
{
	Parent::OnTick();

	if (Cure::Health::Get(this, 1) <= 0)
	{
		OnDie();
	}
}

void ExplodingMachine::OnDie()
{
	ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, GetPosition(), GetVelocity(), Vector3DF(), 2);
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, ExplodingMachine);



}
