
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "Autopilot.h"
#include "../Cure/Include/Health.h"
#include "HeliForceManager.h"
#include "Level.h"

#define AIM_DISTANCE			2.0f
#define AHEAD_TIME			0.5f



namespace HeliForce
{



Autopilot::Autopilot(HeliForceManager* pGame):
	Parent(pGame->GetResourceManager(), _T("Autopilot")),
	mGame(pGame),
	mClosestPathDistance(5.0f),
	mPath(0)
{
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

	Vector3DF lClosestPoint;
	mClosestPathDistance = GetClosestPathDistance(lTowards, lClosestPoint);
	Vector3DF lAim = lClosestPoint - lTowards;
	Vector3DF lAimNear(0, 0, ::std::max(lAim.z, 0.0f));
	const float lSpeedLimit = (mPath->GetDistanceLeft() < 20.0f) ? 4.0f : 60.0f;
	if (lSpeedLimit < 30.0f && (-lVelocity.x<0) == (lAim.x<0))
	{
		lAimNear.x = lAim.x;
	}
	lAim = Math::Lerp(lAim, lAimNear-lVelocity, lVelocity.GetLength()/lSpeedLimit);

	// Brake before upcoming drops.
	const float lTime = mPath->GetCurrentInterpolationTime();
	GetClosestPathDistance(mLastAvatarPosition + lVelocity*AHEAD_TIME*20, lClosestPoint);
	const Vector3DF lUpcomingSlope = mPath->GetSlope().GetNormalized();
	mPath->GotoAbsoluteTime(lTime);
	lAim.x += Math::Lerp(-15.0f, -50.0f, ::fabs(lUpcomingSlope.z)) * lUp.x;
	// End braking before drops.

	lAim.x = Math::Clamp(lAim.x, -0.7f, +0.7f);
	lAim.z = Math::Clamp(lAim.z, -0.0f, +1.0f);
	lAim.z = Math::Lerp(0.05f, 0.7f, lAim.z);
	return lAim;
}

float Autopilot::GetClosestPathDistance() const
{
	return mClosestPathDistance;
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
	if (lRotorSpeed.GetLengthSquared() < 20.0f)
	{
		mStalledRotorTimer.TryStart();
		if (mStalledRotorTimer.QueryTimeDiff() > 2.0f)
		{
			Cure::Health::Add(pChopper, -0.01f, true);
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
