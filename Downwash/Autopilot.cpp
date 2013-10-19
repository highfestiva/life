
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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
	Parent(pGame->GetResourceManager(), _T("Autopilot")),
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
	mPath = new Cure::ContextPath::SplinePath(*mGame->GetLevel()->QueryPath()->GetPath(_T("player_path")));
	mPath->StartInterpolation(0);
	mStalledRotorTimer.Stop();
}

Vector3DF Autopilot::GetSteering()
{
	Level* lLevel = mGame->GetLevel();
	if (!lLevel || lLevel->QueryPath()->GetPathCount() < 1)
	{
		return Vector3DF();
	}
	Cure::ContextObject* lChopper = mGame->GetAvatar();
	if (!lChopper || !lChopper->IsLoaded() || lChopper->GetPhysics()->GetEngineCount() < 3)
	{
		mPath->GotoAbsoluteTime(0);
		return Vector3DF();
	}

	CheckStalledRotor(lChopper);

	mLastAvatarPosition = lChopper->GetPosition();
	const Vector3DF lVelocity = lChopper->GetVelocity();
	const Vector3DF lUp = lChopper->GetOrientation() * Vector3DF(0,0,1);
	const Vector3DF lTowards = mLastAvatarPosition + lVelocity*AHEAD_TIME;

	mClosestPathDistance = GetClosestPathDistance(lTowards, mClosestPathPosition);
	Vector3DF lAim = mClosestPathPosition - lTowards;
	Vector3DF lAimNear(0, 0, ::std::max(lAim.z, 0.0f));
	const float lSpeedLimit = (mPath->GetDistanceLeft() < 20.0f) ? 4.0f : 60.0f;
	if (lSpeedLimit < 30.0f && (-lVelocity.x<0) == (lAim.x<0))
	{
		lAimNear.x = lAim.x;
	}
	lAim = Math::Lerp(lAim, lAimNear-lVelocity, std::min(1.0f, lVelocity.GetLength()/lSpeedLimit));

	// Brake before upcoming drops.
	const float lTime = mPath->GetCurrentInterpolationTime();
	GetClosestPathDistance(mLastAvatarPosition + lVelocity*AHEAD_TIME*20, mClosestPathPosition);
	const Vector3DF lUpcomingSlope = mPath->GetSlope().GetNormalized();
	mPath->GotoAbsoluteTime(lTime);
	lAim.x += Math::Lerp(-15.0f, -50.0f, ::fabs(lUpcomingSlope.z)) * lUp.x;
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

Vector3DF Autopilot::GetClosestPathVector() const
{
	return mClosestPathPosition-mLastAvatarPosition;
}

Vector3DF Autopilot::GetLastAvatarPosition() const
{
	return mLastAvatarPosition;
}


void Autopilot::CheckStalledRotor(Cure::ContextObject* pChopper)
{
	const int lRotorIndex = pChopper->GetPhysics()->GetChildIndex(0, 0);
	TBC::ChunkyBoneGeometry* lBone = pChopper->GetPhysics()->GetBoneGeometry(lRotorIndex);
	Vector3DF lRotorSpeed;
	mGame->GetPhysicsManager()->GetBodyAngularVelocity(lBone->GetBodyId(), lRotorSpeed);
	if (lRotorSpeed.GetLengthSquared() < 40.0f*Lepra::GameTimer::GetRealTimeRatio())
	{
		mStalledRotorTimer.TryStart();
		if (mStalledRotorTimer.QueryTimeDiff() > 2.0f)
		{
			Cure::Health::Add(pChopper, -0.015f, true);
		}
	}
	else
	{
		mStalledRotorTimer.Stop();
	}
}

float Autopilot::GetClosestPathDistance(const Vector3DF& pPosition, Vector3DF& pClosestPoint) const
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



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Autopilot);



}
