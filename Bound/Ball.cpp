
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "Ball.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/GameManager.h"



namespace Bound
{



Ball::Ball(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager)
{
}

Ball::~Ball()
{
}



void Ball::OnTick()
{
	Parent::OnTick();

	Vector3DF lVelocity = GetVelocity();
	float lSpeed = lVelocity.GetLength();
	if (lSpeed < 1.0f)
	{
		lSpeed = 5.0f;
		lVelocity.Normalize(lSpeed);
		const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
		mManager->GetGameManager()->GetPhysicsManager()->SetBodyVelocity(lGeometry->GetBodyId(), lVelocity);
	}
}



}
