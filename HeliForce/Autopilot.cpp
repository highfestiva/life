
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "Autopilot.h"
#include "../Cure/Include/ContextPath.h"
#include "HeliForceManager.h"
#include "Level.h"

#define AIM_DISTANCE			2.0f
#define AHEAD_TIME			0.5f



namespace HeliForce
{



typedef Cure::ContextPath::SplinePath Spline;



Autopilot::Autopilot(HeliForceManager* pGame):
	Parent(pGame->GetResourceManager(), _T("Autopilot")),
	mGame(pGame),
	mClosestPathDistance(5.0f)
{
}

Autopilot::~Autopilot()
{
}



void Autopilot::Reset()
{
	Level* lLevel = mGame->GetLevel();
	if (!lLevel || lLevel->QueryPath()->GetPathCount() < 1)
	{
		return;
	}
	Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(0);
	lPath->GotoAbsoluteTime(0);
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
		Spline* lPath = lLevel->QueryPath()->GetPath(0);
		lPath->GotoAbsoluteTime(0);
		return Vector3DF();
	}

	mLastAvatarPosition = lChopper->GetPosition();
	const Vector3DF lVelocity = lChopper->GetVelocity();
	const Vector3DF lUp = lChopper->GetOrientation() * Vector3DF(0,0,1);
	const Vector3DF lTowards = mLastAvatarPosition + lVelocity*AHEAD_TIME;

	Vector3DF lClosestPoint;
	mClosestPathDistance = GetClosestPathDistance(lTowards, lClosestPoint);
	Spline* lPath = lLevel->QueryPath()->GetPath(0);
	Vector3DF lAim = lPath->GetValue() - lTowards;
	Vector3DF lAimNear(0, 0, ::std::max(lAim.z, 0.0f));
	const float lSpeedLimit = (lPath->GetDistanceLeft() < 20.0f) ? 4.0f : 60.0f;
	if (lSpeedLimit < 30.0f && (-lVelocity.x<0) == (lAim.x<0))
	{
		lAimNear.x = lAim.x;
	}
	lAim = Math::Lerp(lAim, lAimNear-lVelocity, lVelocity.GetLength()/lSpeedLimit);
	lAim.x += -30 * lUp.x;
	lAim.x = Math::Clamp(lAim.x, -0.7f, +0.7f);
	lAim.z = Math::Clamp(lAim.z, -0.0f, +1.0f);
	lAim.z = Math::Lerp(0.05f, 0.6f, lAim.z);
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



float Autopilot::GetClosestPathDistance(const Vector3DF& pPosition, Vector3DF& pClosestPoint) const
{
	Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(0);
	const float lCurrentTime = lPath->GetCurrentInterpolationTime();

	float lNearestDistance;
	const float lSearchStepLength = 0.03f;
	const int lSearchSteps = 3;
	lPath->FindNearestTime(lSearchStepLength, pPosition, lNearestDistance, pClosestPoint, lSearchSteps);

	{
		const float lWantedDistance = AIM_DISTANCE;
		float lDeltaTime = lWantedDistance * lPath->GetDistanceNormal();
		if (lCurrentTime+lDeltaTime < 0)
		{
			lDeltaTime = -lCurrentTime;
		}
		lPath->StepInterpolation(lDeltaTime);
	}

	return lNearestDistance;
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Autopilot);



}
