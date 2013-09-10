
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "Mine.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/Health.h"
#include "../ProjectileUtil.h"



namespace Life
{



Mine::Mine(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Launcher* pLauncher):
	Parent(pResourceManager, pClassId, pUiManager, pLauncher),
	mEnableDeleteDetonation(true)
{
	Cure::Health::Set(this, 1);
}

Mine::~Mine()
{
	if (mEnableDeleteDetonation)
	{
		OnDie();
	}
}

void Mine::EnableDeleteDetonation(bool pEnable)
{
	mEnableDeleteDetonation = pEnable;
}

void Mine::OnDie()
{
	Parent::OnDie();
	GetManager()->PostKillObject(GetInstanceId());
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Mine);



}
