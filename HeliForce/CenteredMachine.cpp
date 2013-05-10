
// Author: Jonas Byström
// Copyright (c) 2002-2013, Righteous Games



#include "CenteredMachine.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/FloatAttribute.h"
#include "../Cure/Include/GameManager.h"
#include "../Life/Explosion.h"
#include "../Life/Launcher.h"



namespace HeliForce
{



CenteredMachine::CenteredMachine(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Life::Launcher* pLauncher):
	Parent(pResourceManager, pClassId, pUiManager, pLauncher)
{
	new Cure::FloatAttribute(this, _T("float_health"), 1);
}

CenteredMachine::~CenteredMachine()
{
}


void CenteredMachine::OnMicroTick(float pFrameTime)
{
	Parent::OnMicroTick(pFrameTime);

	if (!mIsDetonated)
	{
		TBC::PhysicsManager* lPhysicsManager = GetManager()->GetGameManager()->GetPhysicsManager();
		TBC::PhysicsManager::BodyID lBodyId = GetPhysics()->GetBoneGeometry(0)->GetBodyId();
		Vector3DF v = lPhysicsManager->GetBodyPosition(lBodyId);
		v.y = 0;
		lPhysicsManager->SetBodyPosition(lBodyId, v);
		lPhysicsManager->GetBodyVelocity(lBodyId, v);
		v.y = 0;
		lPhysicsManager->SetBodyVelocity(lBodyId, v);
	}
}

void CenteredMachine::OnDie()
{
	if (mIsDetonated)
	{
		GetManager()->GetGameManager()->DeleteContextObjectDelay(this, 3.0);
		return;
	}
	mIsDetonated = true;

	mLauncher->Detonate(this, GetPhysics()->GetBoneGeometry(0), GetPosition(), GetVelocity(), Vector3DF(), 2);
	Life::Explosion::FallApart(GetManager()->GetGameManager()->GetPhysicsManager(), this);
	CenterMeshes();
	GetManager()->GetGameManager()->DeleteContextObjectDelay(this, 3.0);
}



}
