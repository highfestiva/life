
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "AutoPathDriver.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/Health.h"
#include "Level.h"

#define AIM_DISTANCE			6.0f
#define REAR_WHEEL_STEERING_DISTANCE	10.0f



namespace Fire
{



AutoPathDriver::AutoPathDriver(FireManager* pGame, Cure::GameObjectId pVehicleId, const str& pPathName):
	Parent(pGame->GetResourceManager(), _T("AutoPathDriver")),
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

	const Vector3DF lVehicleDirection3d = lVehicle->GetOrientation()*Vector3DF(0,1,0);
	const Vector3DF lPosition = lVehicle->GetPosition() + lVehicleDirection3d * REAR_WHEEL_STEERING_DISTANCE;
	Vector3DF lClosestPoint;
	GetClosestPathDistance(lPosition, lClosestPoint, AIM_DISTANCE);
	const Vector3DF lDirection(lClosestPoint - lPosition);

	lVehicle->SetEnginePower(0, 1);
	const Vector2DF lWantedDirection(lDirection.x, lDirection.y);
	//lWantedDirection.Normalize();
	const Vector2DF lVehicleDirection(lVehicleDirection3d.x, lVehicleDirection3d.y);
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

void AutoPathDriver::GetClosestPathDistance(const Vector3DF& pPosition, Vector3DF& pClosestPoint, float pWantedDistance)
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



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, AutoPathDriver);



}
