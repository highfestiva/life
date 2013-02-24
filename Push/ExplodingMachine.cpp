
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "ExplodingMachine.h"
#include "../Cure/Include/FloatAttribute.h"
#include "../Life/ProjectileUtil.h"



namespace Push
{



ExplodingMachine::ExplodingMachine(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Life::Launcher* pLauncher):
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

	Cure::FloatAttribute* lHealth = (Cure::FloatAttribute*)GetAttribute(_T("float_health"));
	if (lHealth && lHealth->GetValue() <= 0)
	{
		Life::ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, GetPosition(), GetVelocity(), Vector3DF(), 2);
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, ExplodingMachine);



}
