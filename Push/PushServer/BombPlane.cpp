
// Author: Jonas Bystr�m
// Copyright (c) 2002-2010, Righteous Games



#include "BombPlane.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/FloatAttribute.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/Random.h"
#include "../../Life/Launcher.h"
#include "../../Life/ProjectileUtil.h"

#define BOMBING_RADIUS 80.0f



namespace Push
{



BombPlane::BombPlane(Cure::ResourceManager* pResourceManager, const str& pClassId, Life::Launcher* pLauncher, const Vector3DF& pTarget):
	Parent(pResourceManager, pClassId),
	mLauncher(pLauncher),
	mTarget(pTarget),
	mLastBombTick(0),
	mIsDetonated(false)
{
	new Cure::FloatAttribute(this, _T("float_health"), 0.1f);	// Partially excempted in explosion logic, so definitely higher in practice.

	// Randomize so bombers won't have perfect (robotic) synchronization.
	mBombingRadiusSquared = BOMBING_RADIUS*BOMBING_RADIUS * Random::Uniform(0.6f, 1.7f);
	mDropInterval = Random::Uniform(0.5f, 0.8f);
}

BombPlane::~BombPlane()
{
}



void BombPlane::OnLoaded()
{
	SetEnginePower(0, 1.0f);	// Jet.
	SetEnginePower(1, 1.0f);	// Hoover.

	Parent::OnLoaded();
}

void BombPlane::OnTick()
{
	const float lHealth = ((Cure::FloatAttribute*)GetAttribute(_T("float_health")))->GetValue();
	if (lHealth <= 0)
	{
		Life::ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, GetPosition(), GetVelocity(), Vector3DF(), 1);
	}

	const Cure::TimeManager* lTimeManager = GetManager()->GetGameManager()->GetTimeManager();
	if (lTimeManager->ConvertPhysicsFramesToSeconds(lTimeManager->GetCurrentPhysicsFrameDelta(mLastBombTick)) < mDropInterval)
	{
		return;
	}

	const Vector3DF lPosition = GetPosition();
	const Vector3DF lVelocity = GetVelocity();

	// g*t^2/2 - v0*t + h = 0
	const float g2 = +9.82f/2;	// Positive.
	const float v0 = 0;//lVelocity.z;
	const float h = mTarget.z - lPosition.z;	// Negative.
	float t1;
	float t2;
	if (Math::CalculateRoot(g2, v0, h, t1, t2))
	{
		Vector3DF lHorizontalProjectedTarget(mTarget.x, mTarget.y, lPosition.z);
		const float d = lHorizontalProjectedTarget.GetDistanceSquared(lPosition + lVelocity*t1);
		if (d < mBombingRadiusSquared)
		{
			mLastBombTick = lTimeManager->GetCurrentPhysicsFrame();
			mLauncher->Shoot(this, -10);
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, BombPlane);



}
