
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Grenade.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/GameManager.h"
#include "../TBC/Include/ChunkyBoneGeometry.h"



namespace GrenadeRun
{



Grenade::Grenade(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager)
{
}

Grenade::~Grenade()
{
}



void Grenade::Start()
{
	GetManager()->AddAlarmCallback(this, 1, 1.5f, 0);
}



void Grenade::OnAlarm(int, void*)
{
	const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
	if (lGeometry && lGeometry->GetBodyId() != TBC::INVALID_BODY)
	{
		Vector3DF lVelocity(-4, 4, 30);
		GetManager()->GetGameManager()->GetPhysicsManager()->SetBodyVelocity(lGeometry->GetBodyId(), lVelocity);
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Grenade);



}
