
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Autopilot.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/Health.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "DownwashManager.h"
#include "Level.h"

#define AIM_DISTANCE			2.0f
#define AHEAD_TIME			0.5f



namespace Downwash
{



Autopilot::Autopilot(DownwashManager* pGame):
	Parent(pGame->GetResourceManager(), "Autopilot"),
	mGame(pGame),
	mClosestPathDistance(5.0f),
	mPath(0)
{
	mGame->GetContext()->AddLocalObject(this);
}

Autopilot::~Autopilot()
{
	delete mPath;
	mPath = 0;
}



void Autopilot::Reset()
{
	Level* lLevel = mGame->GetLevel();
	if (!lLevel || lLevel->QueryPath()->GetPathCount() < 1)
	{
		return;
	}
	delete mPath;
	mPath = new Cure::ContextPath::SplinePath(*mGame->GetLevel()->QueryPath()->GetPath("player_path"));
	mPath->StartInterpolation(0);
	mStalledRotorTimer.Stop();
}

vec3 Autopilot::GetSteering()
{
	Level* lLevel = mGame->GetLevel();
	if (!lLevel || lLevel->QueryPath()->GetPathCount() < 1)
	{
		return vec3();
	}
	Cure::ContextObject* lChopper = mGame->GetAvatar();
	if (!lChopper || !lChopper->IsLoaded() || lChopper->GetPhysics()->GetEngineCount() < 3)
	{
		mPath->GotoAbsoluteTime(0);
		return vec3();
	}

	CheckStalledRotor(lChopper);

	mLastAvatarPosition = lChopper->GetPosition();
	const vec3 lVelocity = lChopper->GetVelocity();
	const vec3 lUp = lChopper->GetOrientation() * vec3(0,0,1);
	const vec3 lTowards = mLastAvatarPosition + lVelocity*AHEAD_TIME;

	mClosestPathDistance = GetClosestPathDistance(lTowards, mClosestPathPosition);
	vec3 lAim = mClosestPathPosition - lTowards;
	vec3 lAimNear(0, 0, ::std::max(lAim.z, 0.0f));
	const bool lGoingWrongWay = (lVelocity*lAim <= 0);
	const float lSpeedLimit = (lGoingWrongWay || mPath->GetDistanceLeft() < 20.0f) ? 4.0f : 60.0f;
	if (!lGoingWrongWay)
	{
		lAimNear.x = lAim.x;
	}
	lAim = Math::Lerp(lAim, lAimNear-lVelocity, std::min(1.0f, lVelocity.GetLength()/lSpeedLimit));

	// Brake before upcoming drops.
	if (mClosestPathDistance < 20)
	{
		const float lTime = mPath->GetCurrentInterpolationTime();
		GetClosestPathDistance(mLastAvatarPosition + lVelocity*AHEAD_TIME*20, mClosestPathPosition);
		const vec3 lUpcomingSlope = mPath->GetSlope().GetNormalized();
		mPath->GotoAbsoluteTime(lTime);
		lAim.x += Math::Lerp(-15.0f, -50.0f, ::fabs(lUpcomingSlope.z)) * lUp.x;
	}
	// End braking before drops.

	lAim.x = Math::Clamp(lAim.x, -0.9f, +0.9f);
	lAim.z = Math::Clamp(lAim.z, -0.0f, +1.0f);
	lAim.z = Math::Lerp(0.05f, 0.9f, lAim.z);
	return lAim;
}

void Autopilot::AttemptCloserPathDistance()
{
	float lStartTime = mPath->GetCurrentInterpolationTime();
	float lShortestTime = lStartTime;
	float lShortestDistance = mPath->GetValue().GetDistanceSquared(mLastAvatarPosition);
	for (float x = 0; x < 1; x += 0.05f)
	{
		const float lTime = fmod(lStartTime + x, 1.0f);
		mPath->GotoAbsoluteTime(lTime);
		const float lDistance = mPath->GetValue().GetDistanceSquared(mLastAvatarPosition);
		if (lDistance < lShortestDistance)
		{
			lShortestTime = lTime;
			lShortestDistance = lDistance;
		}
	}
	mPath->GotoAbsoluteTime(lShortestTime);
}

float Autopilot::GetClosestPathDistance() const
{
	return mClosestPathDistance;
}

vec3 Autopilot::GetClosestPathVector() const
{
	return mClosestPathPosition-mLastAvatarPosition;
}

vec3 Autopilot::GetLastAvatarPosition() const
{
	return mLastAvatarPosition;
}

float Autopilot::GetRotorSpeed(const Cure::ContextObject* pChopper) const
{
	if (pChopper->GetPhysics()->GetEngineCount() < 3)
	{
		return 0;
	}
	const int lRotorIndex = pChopper->GetPhysics()->GetChildIndex(0, 0);
	Tbc::ChunkyBoneGeometry* lBone = pChopper->GetPhysics()->GetBoneGeometry(lRotorIndex);
	vec3 lRotorSpeed;
	mGame->GetPhysicsManager()->GetBodyAngularVelocity(lBone->GetBodyId(), lRotorSpeed);
	return lRotorSpeed.GetLength();
}

void Autopilot::CheckStalledRotor(Cure::ContextObject* pChopper)
{
	const float lRotorSpeed = GetRotorSpeed(pChopper);
	if (lRotorSpeed < 6.0f*Lepra::GameTimer::GetRealTimeRatio())
	{
		mStalledRotorTimer.TryStart();
		if (mStalledRotorTimer.QueryTimeDiff() > 3.0f)
		{
			Cure::Health::Add(pChopper, -0.02f, true);
		}
	}
	else
	{
		mStalledRotorTimer.Stop();
	}
}

float Autopilot::GetClosestPathDistance(const vec3& pPosition, vec3& pClosestPoint) const
{
	const float lCurrentTime = mPath->GetCurrentInterpolationTime();

	float lNearestDistance;
	const float lSearchStepLength = 0.06f;
	const int lSearchSteps = 3;
	mPath->FindNearestTime(lSearchStepLength, pPosition, lNearestDistance, pClosestPoint, lSearchSteps);

	{
		const float lWantedDistance = AIM_DISTANCE;
		float lDeltaTime = lWantedDistance * mPath->GetDistanceNormal();
		if (lCurrentTime+lDeltaTime < 0.965f)	// Only move forward if we stay within the curve; otherwise we would loop.
		{
			if (lCurrentTime+lDeltaTime < 0)
			{
				lDeltaTime = -lCurrentTime;
			}
			mPath->StepInterpolation(lDeltaTime);
			pClosestPoint = mPath->GetValue();
		}
	}

	return lNearestDistance;
}



loginstance(GAME_CONTEXT_CPP, Autopilot);



}
