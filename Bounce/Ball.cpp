
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Ball.h"
#include "../Cure/Include/ContextManager.h"



namespace Bounce
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
	float lAirForce = lVelocity.GetLength();
	lAirForce *= lAirForce * -0.002f;
	lVelocity.x *= lAirForce;
	lVelocity.y *= lAirForce;
	lVelocity.z *= lAirForce * 0.09f;
	lPhysicsManager->AddForce(
		GetPhysics()->GetBoneGeometry(0)->GetBodyId(),
		lVelocity);
}

void Ball::OnLoaded()
{
	GetManager()->EnableMicroTickCallback(this);
	Parent::OnLoaded();
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Ball);



}