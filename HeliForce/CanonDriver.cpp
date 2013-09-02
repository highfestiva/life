
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "CanonDriver.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/Health.h"
#include "../Lepra/Include/Random.h"
#include "Level.h"



namespace HeliForce
{



CanonDriver::CanonDriver(HeliForceManager* pGame, Cure::GameObjectId pCanonId):
	Parent(pGame->GetResourceManager(), _T("CanonDriver")),
	mGame(pGame),
	mCanonId(pCanonId)
{
	pGame->GetContext()->AddLocalObject(this);
	pGame->GetContext()->EnableTickCallback(this);
}

CanonDriver::~CanonDriver()
{
}



void CanonDriver::OnTick()
{
	Parent::OnTick();

	Cure::ContextObject* lCanon = mManager->GetObject(mCanonId, true);
	if (!lCanon)
	{
		mManager->PostKillObject(GetInstanceId());
		return;
	}
	if (!lCanon->IsLoaded() || lCanon->GetPhysics()->GetEngineCount() < 1)
	{
		return;
	}

	Cure::ContextObject* lAvatar = mManager->GetObject(mGame->GetAvatarInstanceId());
	if (!lAvatar || !lAvatar->IsLoaded())
	{
		return;
	}

	const Vector3DF lTarget(lAvatar->GetPosition() + lAvatar->GetVelocity()*0.2f);
	const Vector2DF d(lTarget.z - lCanon->GetPosition().z, lTarget.x - lCanon->GetPosition().x);
	TBC::ChunkyBoneGeometry* lBarrel = lCanon->GetPhysics()->GetBoneGeometry(1);
	TBC::PhysicsManager::Joint1Diff lDiff;
	mGame->GetPhysicsManager()->GetJoint1Diff(lBarrel->GetBodyId(), lBarrel->GetJointId(), lDiff);
	const float lAngle = -d.GetAngle() - lDiff.mValue;
	const float lTargetAngle = Math::Clamp(lAngle*20, -2.0f, +2.0f);
	lCanon->SetEnginePower(1, -lTargetAngle);

	if (std::abs(lAngle) < 0.4f && mLastShot.QueryTimeDiff() >= 0)
	{
		mLastShot.PopTimeDiff();
		mLastShot.ReduceTimeDiff(Random::Normal(0.2f, 0.05f, 0.15f, 0.25f));
		mGame->Shoot(lCanon, 0);
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, CanonDriver);



}
