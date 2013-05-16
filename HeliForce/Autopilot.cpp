
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "Autopilot.h"
//#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/ContextPath.h"
/*#include "../Cure/Include/Elevator.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/Random.h"*/
#include "HeliForceManager.h"
#include "Level.h"

#define AIM_DISTANCE			5.0f
#define AHEAD_TIME			2.0f



namespace HeliForce
{



typedef Cure::ContextPath::SplinePath Spline;



Autopilot::Autopilot(HeliForceManager* pGame):
	Parent(pGame->GetResourceManager(), _T("Autopilot")),
	mGame(pGame)
{
}

Autopilot::~Autopilot()
{
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

	const Vector3DF lPosition = lChopper->GetPosition();
	const Vector3DF lVelocity = lChopper->GetVelocity();
	const Vector3DF lTowards = lPosition + lVelocity*AHEAD_TIME;

	const float lWillHaveDistance = GetClosestPathDistance(lTowards);
	const float lCurrentDistance = GetClosestPathDistance(lPosition);
	Spline* lPath = lLevel->QueryPath()->GetPath(0);
	Vector3DF lAim;
	if (lWillHaveDistance < lCurrentDistance+0.1f)
	{
		GetClosestPathDistance(lTowards);
		lAim = lPath->GetValue() - lTowards;
		lAim *= 0.3f;
	}
	else
	{
		lAim = lPath->GetValue() - lVelocity*AHEAD_TIME - lTowards;
	}
	lAim.x = Math::Clamp(lAim.x, -1.0f, +1.0f);
	lAim.z = Math::Clamp(lAim.z, -0.0f, +1.0f);
	return lAim;
}



float Autopilot::GetClosestPathDistance(const Vector3DF& pPosition) const
{
	Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(0);
	const float lCurrentTime = lPath->GetCurrentInterpolationTime();

	float lNearestDistance;
	Vector3DF lClosestPoint;
	const float lSearchStepLength = 0.0125f;
	const int lSearchSteps = 2;
	lPath->FindNearestTime(lSearchStepLength, pPosition, lNearestDistance, lClosestPoint, lSearchSteps);

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
