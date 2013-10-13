
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "CanonDriver.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/Health.h"
#include "../Lepra/Include/Random.h"
#include "../Life/ProjectileUtil.h"
#include "Level.h"



namespace Downwash
{



CanonDriver::CanonDriver(DownwashManager* pGame, Cure::GameObjectId pCanonId, int pAmmoType):
	Parent(pGame->GetResourceManager(), _T("CanonDriver")),
	mGame(pGame),
	mCanonId(pCanonId),
	mAmmoType(pAmmoType),
	mDistance(1000),
	mShootPeriod(1),
	mTagSet(false)
{
	pGame->GetContext()->AddLocalObject(this);
	pGame->GetContext()->EnableTickCallback(this);

	Cure::CppContextObject* lCanon = (Cure::CppContextObject*)mManager->GetObject(mCanonId, true);
	deb_assert(lCanon);
	TransformationF lMuzzleTransform;
	Vector3DF _;
	Life::ProjectileUtil::GetBarrelByShooter(lCanon, lMuzzleTransform, _);
	mJointStartAngle = (lMuzzleTransform.GetOrientation() * Vector3DF(0,0,1)).GetAngle(Vector3DF(0,0,1));
}

CanonDriver::~CanonDriver()
{
}



void CanonDriver::OnTick()
{
	Parent::OnTick();

	Cure::CppContextObject* lCanon = (Cure::CppContextObject*)mManager->GetObject(mCanonId, true);
	if (!lCanon)
	{
		mManager->PostKillObject(GetInstanceId());
		return;
	}
	if (!lCanon->IsLoaded() || lCanon->GetPhysics()->GetEngineCount() < 1)
	{
		return;
	}
	if (!mTagSet)
	{
		const TBC::ChunkyClass::Tag* lTag = lCanon->FindTag(_T("behavior"), 2, 0);
		deb_assert(lTag);
		mDistance = lTag->mFloatValueList[0];
		mShootPeriod = 1/lTag->mFloatValueList[1];
		mTagSet = true;
	}

	Cure::ContextObject* lAvatar = mManager->GetObject(mGame->GetAvatarInstanceId());
	if (!lAvatar)
	{
		return;
	}

	const Vector3DF lTarget(lAvatar->GetPosition() + lAvatar->GetVelocity()*0.2f);
	const Vector2DF d(lTarget.z - lCanon->GetPosition().z, lTarget.x - lCanon->GetPosition().x);
	if (d.GetLengthSquared() >= mDistance*mDistance)
	{
		return;	// Don't shoot at distant objects.
	}
	TBC::ChunkyBoneGeometry* lBarrel = lCanon->GetPhysics()->GetBoneGeometry(1);
	TBC::PhysicsManager::Joint1Diff lDiff;
	mGame->GetPhysicsManager()->GetJoint1Diff(lBarrel->GetBodyId(), lBarrel->GetJointId(), lDiff);
	const float lAngle = -d.GetAngle() - (mJointStartAngle + lDiff.mValue);
	const float lTargetAngle = Math::Clamp(lAngle*20, -2.0f, +2.0f);
	lCanon->SetEnginePower(1, -lTargetAngle);

	if (mLastShot.QueryTimeDiff() >= 0)
	{
		float lLowAngle = 0;
		float lHighAngle = 0;
		float lBounce;
		mGame->GetPhysicsManager()->GetJointParams(lBarrel->GetJointId(), lLowAngle, lHighAngle, lBounce);
		if (std::abs(lAngle) < std::abs(lHighAngle-lLowAngle)*0.1f)
		{
			mLastShot.Start(-Random::Normal(mShootPeriod, mShootPeriod/4, mShootPeriod*0.75f, mShootPeriod*1.25f));
			mGame->Shoot(lCanon, mAmmoType);
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, CanonDriver);



}
