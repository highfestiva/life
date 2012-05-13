
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Ball.h"
#include "../Cure/Include/ContextManager.h"



namespace Magnetic
{



Ball::Ball(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager)
{
}

Ball::~Ball()
{
}



void Ball::OnMicroTick(float pFrameTime)
{
	Parent::OnMicroTick(pFrameTime);

	if (!IsLoaded())
	{
		return;
	}
	TBC::PhysicsManager* lPhysicsManager = GetManager()->GetGameManager()->GetPhysicsManager();
	Vector3DF lVelocity;
	lPhysicsManager->GetBodyVelocity(
		GetPhysics()->GetBoneGeometry(0)->GetBodyId(),
		lVelocity);
	const float lAirForce = lVelocity.GetLength();
	const Vector3DF lResistance = lVelocity * (lAirForce * lAirForce * -0.0008f);
	lPhysicsManager->AddForce(
		GetPhysics()->GetBoneGeometry(0)->GetBodyId(),
		lResistance);
}

void Ball::OnLoaded()
{
	GetManager()->EnableMicroTickCallback(this);
	Parent::OnLoaded();
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Ball);



}
