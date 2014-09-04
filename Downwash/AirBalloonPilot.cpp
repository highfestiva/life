
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "AirBalloonPilot.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/ContextPath.h"
#include "DownwashManager.h"
#include "Level.h"

#define AIM_DISTANCE			10.0f



namespace Downwash
{



typedef Cure::ContextPath::SplinePath Spline;



AirBalloonPilot::AirBalloonPilot(DownwashManager* pGame, Cure::GameObjectId pBalloonId):
	Parent(pGame->GetResourceManager(), _T("AirBalloonPilot")),
	mGame(pGame),
	mBalloonId(pBalloonId),
	mPath(0)
{
	pGame->GetContext()->AddLocalObject(this);
	pGame->GetContext()->EnableTickCallback(this);
}

AirBalloonPilot::~AirBalloonPilot()
{
	delete mPath;
	mPath = 0;
}



void AirBalloonPilot::OnTick()
{
	Parent::OnTick();

	Cure::ContextObject* lBalloon = mManager->GetObject(mBalloonId, true);
	if (!lBalloon || !mGame->GetLevel())
	{
		mManager->PostKillObject(GetInstanceId());
		return;
	}
	if (!lBalloon->IsLoaded() || lBalloon->GetPhysics()->GetEngineCount() < 1)
	{
		return;
	}

	const vec3 lPosition = lBalloon->GetPosition();
	vec3 lClosestPoint;
	GetClosestPathDistance(lPosition, lClosestPoint);
	const vec3 lDirectionForce = (lClosestPoint - lPosition) * 0.2f;
	const vec3 lAntiRotationForce = lBalloon->GetAngularVelocity() * -50.0f;
	const vec3 lForce(lDirectionForce.x+lAntiRotationForce.y, lDirectionForce.y-lAntiRotationForce.x, lDirectionForce.z);
	lBalloon->SetEnginePower(0, Math::Clamp(lForce.y, -0.1f, +0.1f));
	lBalloon->SetEnginePower(1, Math::Clamp(lForce.x, -0.1f, +0.1f));
	lBalloon->SetEnginePower(3, Math::Clamp(lForce.z, +0.4f, +1.0f));
}



void AirBalloonPilot::GetClosestPathDistance(const vec3& pPosition, vec3& pClosestPoint)
{
	if (!mPath)
	{
		mPath = new Cure::ContextPath::SplinePath(*mGame->GetLevel()->QueryPath()->GetPath(_T("air_balloon_path")));
		mPath->StartInterpolation(0);
	}
	const float lCurrentTime = mPath->GetCurrentInterpolationTime();

	float lNearestDistance;
	const float lSearchStepLength = 0.06f;
	const int lSearchSteps = 3;
	mPath->FindNearestTime(lSearchStepLength, pPosition, lNearestDistance, pClosestPoint, lSearchSteps);

	{
		const float lWantedDistance = AIM_DISTANCE;
		float lDeltaTime = lWantedDistance * mPath->GetDistanceNormal();
		if (lCurrentTime+lDeltaTime < 0)
		{
			lDeltaTime = -lCurrentTime;
		}
		mPath->StepInterpolation(lDeltaTime);
		pClosestPoint = mPath->GetValue();
	}
}



loginstance(GAME_CONTEXT_CPP, AirBalloonPilot);



}
