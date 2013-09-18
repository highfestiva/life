
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "ExplodingMachine.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/Health.h"
#include "../ProjectileUtil.h"



namespace Life
{



ExplodingMachine::ExplodingMachine(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Launcher* pLauncher):
	Parent(pResourceManager, pClassId, pUiManager),
	mLauncher(pLauncher),
	mTriggerDeathFrame(-1),
	mDeathFrameDelay(1),
	mDisappearAfterDeathDelay(-1),
	mIsDetonated(false),
	mExplosiveStrength(2)
{
}

ExplodingMachine::~ExplodingMachine()
{
}



void ExplodingMachine::SetExplosiveStrength(float pExplosiveStrength)
{
	mExplosiveStrength = pExplosiveStrength;
}

void ExplodingMachine::SetDeathFrameDelay(int pDeathFrameDelay)
{
	mDeathFrameDelay = pDeathFrameDelay;
}

void ExplodingMachine::SetDisappearAfterDeathDelay(float pDisappearDelay)
{
	mDisappearAfterDeathDelay = pDisappearDelay;
}

void ExplodingMachine::OnTick()
{
	Parent::OnTick();

	if (Cure::Health::Get(this, 1) <= 0 && mTriggerDeathFrame < 0)
	{
		mTriggerDeathFrame = 0;
	}
	if (mTriggerDeathFrame >= 0)
	{
		if (++mTriggerDeathFrame > mDeathFrameDelay)
		{
			OnDie();
		}
	}
}

void ExplodingMachine::OnDie()
{
	ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, GetPosition(), GetVelocity(), Vector3DF(), mExplosiveStrength);
	if (mDisappearAfterDeathDelay >= 0)
	{
		GetManager()->GetGameManager()->DeleteContextObjectDelay(this, mDisappearAfterDeathDelay);
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, ExplodingMachine);



}
