
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "AutoPathDriver.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/Health.h"
#include "Level.h"

#define AIM_DISTANCE			3.0f
#define REAR_WHEEL_STEERING_DISTANCE	5.0f



namespace Downwash
{



AutoPathDriver::AutoPathDriver(DownwashManager* pGame, Cure::GameObjectId pVehicleId, const str& pPathName):
	Parent(pGame->GetResourceManager(), "AutoPathDriver"),
	mGame(pGame),
	mVehicleId(pVehicleId),
	mPathName(pPathName),
	mPath(0)
{
	pGame->GetContext()->AddLocalObject(this);
	pGame->GetContext()->EnableTickCallback(this);
}

AutoPathDriver::~AutoPathDriver()
{
	delete mPath;
	mPath = 0;
}



void AutoPathDriver::OnTick()
{
	Parent::OnTick();

	Cure::ContextObject* lVehicle = mManager->GetObject(mVehicleId, true);
	if (!lVehicle)
	{
		mManager->PostKillObject(GetInstanceId());
		return;
	}
	if (!lVehicle->IsLoaded() || lVehicle->GetPhysics()->GetEngineCount() < 3)
	{
		return;
	}

	const vec3 lVehicleDirection3d = lVehicle->GetOrientation()*vec3(0,1,0);
	const vec3 lPosition = lVehicle->GetPosition() + lVehicleDirection3d * REAR_WHEEL_STEERING_DISTANCE;
	vec3 lClosestPoint;
	GetClosestPathDistance(lPosition, lClosestPoint, AIM_DISTANCE);
	const vec3 lDirection(lClosestPoint - lPosition);

	lVehicle->SetEnginePower(0, 1);
	const vec2 lWantedDirection(lDirection.x, lDirection.y);
	//lWantedDirection.Normalize();
	const vec2 lVehicleDirection(lVehicleDirection3d.x, lVehicleDirection3d.y);
	//lVehicleDirection.Normalize();
	const float lSteeringAngle = lWantedDirection.GetAngle(lVehicleDirection);
	lVehicle->SetEnginePower(1, lSteeringAngle);

	if (lVehicle->GetVelocity().GetLengthSquared() < 1.0f)
	{
		mStillTimer.TryStart();
		if (mStillTimer.QueryTimeDiff() > 4.0f)
		{
			Cure::Health::Set(lVehicle, 0);
		}
	}
	else
	{
		mStillTimer.Stop();
	}
}

void AutoPathDriver::GetClosestPathDistance(const vec3& pPosition, vec3& pClosestPoint, float pWantedDistance)
{
	if (!mPath)
	{
		if (!mGame->GetLevel() || !mGame->GetLevel()->QueryPath()->GetPath(mPathName))
		{
			mManager->PostKillObject(GetInstanceId());
			return;
		}
		mPath = new Cure::ContextPath::SplinePath(*mGame->GetLevel()->QueryPath()->GetPath(mPathName));
		mPath->StartInterpolation(0);
	}

	const float lCurrentTime = mPath->GetCurrentInterpolationTime();

	float lNearestDistance;
	const float lSearchStepLength = 0.06f;
	const int lSearchSteps = 3;
	mPath->FindNearestTime(lSearchStepLength, pPosition, lNearestDistance, pClosestPoint, lSearchSteps);

	{
		float lDeltaTime = pWantedDistance * mPath->GetDistanceNormal();
		if (lCurrentTime+lDeltaTime < 0)
		{
			lDeltaTime = -lCurrentTime;
		}
		mPath->StepInterpolation(lDeltaTime);
		pClosestPoint = mPath->GetValue();
	}
}



loginstance(GAME_CONTEXT_CPP, AutoPathDriver);



}
