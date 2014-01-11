
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "BaseMachine.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/Health.h"
#include "../Cure/Include/GameManager.h"
#include "../Life/Explosion.h"
#include "../Life/Launcher.h"



namespace Fire
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
		//GetManager()->GetGameManager()->DeleteContextObjectDelay(this, 3.0);
		return;
	}

	Life::Explosion::FallApart(GetManager()->GetGameManager()->GetPhysicsManager(), this);
	Parent::OnDie();
	//CenterMeshes();
}



}
