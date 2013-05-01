
// Author: Jonas Byström
// Copyright (c) 2002-2013, Righteous Games



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



void CenteredMachine::OnMicroTick(float pFrameTime)
{
	Parent::OnMicroTick(pFrameTime);

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
