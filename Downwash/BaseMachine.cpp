
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "BaseMachine.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/Health.h"
#include "../Cure/Include/GameManager.h"
#include "../Life/Explosion.h"
#include "../Life/Launcher.h"



namespace Downwash
{



BaseMachine::BaseMachine(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Life::Launcher* pLauncher):
	Parent(pResourceManager, pClassId, pUiManager, pLauncher)
{
	Cure::Health::Set(this, 1);
}

BaseMachine::~BaseMachine()
{
}



void BaseMachine::OnDie()
{
	if (mIsDetonated)
	{
		//GetManager()->DelayKillObject(this, 3);
		return;
	}

	Life::Explosion::FallApart(GetManager()->GetGameManager()->GetPhysicsManager(), this);
	Parent::OnDie();
	//CenterMeshes();
}



}
