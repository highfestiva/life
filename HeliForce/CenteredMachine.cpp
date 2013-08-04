
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#include "CenteredMachine.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/GameManager.h"



namespace HeliForce
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



}