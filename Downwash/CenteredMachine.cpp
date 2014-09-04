
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "CenteredMachine.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/GameManager.h"



namespace Downwash
{



CenteredMachine::CenteredMachine(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Life::Launcher* pLauncher):
	Parent(pResourceManager, pClassId, pUiManager, pLauncher)
{
}

CenteredMachine::~CenteredMachine()
{
}


bool CenteredMachine::TryComplete()
{
	if (Parent::TryComplete())
	{
		GetManager()->EnableMicroTickCallback(this);
		return true;
	}
	return false;
}

void CenteredMachine::OnMicroTick(float pFrameTime)
{
	Parent::OnMicroTick(pFrameTime);

	if (!mIsDetonated)
	{
		Tbc::PhysicsManager* lPhysicsManager = GetManager()->GetGameManager()->GetPhysicsManager();
		Tbc::PhysicsManager::BodyID lBodyId = GetPhysics()->GetBoneGeometry(0)->GetBodyId();
		vec3 v = lPhysicsManager->GetBodyPosition(lBodyId);
		v.y = 0;
		lPhysicsManager->SetBodyPosition(lBodyId, v);
		lPhysicsManager->GetBodyVelocity(lBodyId, v);
		v.y = 0;
		lPhysicsManager->SetBodyVelocity(lBodyId, v);
	}
}



}
