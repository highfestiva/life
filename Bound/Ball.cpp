
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "Ball.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/GameManager.h"



namespace Bound
{



Ball::Ball(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager),
	mAverageSpeed(0)
{
}

Ball::~Ball()
{
}



void Ball::OnTick()
{
	Parent::OnTick();

	Vector3DF lVelocity = GetVelocity();
	bool lNormalize = false;
	float lSpeed = lVelocity.GetLength();
	mAverageSpeed = Math::Lerp(mAverageSpeed, lSpeed, 0.1f);
	if (lSpeed > 4.1f)
	{
		lSpeed = 3.9f;
		lNormalize = true;
	}
	else if (mAverageSpeed < 1.0f)
	{
		lSpeed = 3.9f;
		mAverageSpeed = 3.9f;
		lNormalize = true;
	}
	if (lNormalize)
	{
		lVelocity.Normalize(lSpeed);
		const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());
		mManager->GetGameManager()->GetPhysicsManager()->SetBodyVelocity(lGeometry->GetBodyId(), lVelocity);
	}
}



}
