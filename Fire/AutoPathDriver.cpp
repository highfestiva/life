
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "AutoPathDriver.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/Health.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "BaseMachine.h"
#include "Level.h"

#define AIM_DISTANCE			10.0f



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

	BaseMachine* lVehicle = (BaseMachine*)mManager->GetObject(mVehicleId, true);
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
	const Vector3DF lPosition = lVehicle->GetPosition();
	Vector3DF lClosestPoint;
	GetClosestPathDistance(lPosition, lClosestPoint, AIM_DISTANCE);
	const Vector3DF lDirection(lClosestPoint - lPosition);

	const float lEnginePower = Math::Lerp(0.4f, 1.0f, lVehicle->mPanicLevel) * lVehicle->mLevelSpeed;
	lVehicle->SetEnginePower(0, lEnginePower);
	const Vector2DF lWantedDirection(lDirection.x, lDirection.y);
	const Vector2DF lVehicleDirection(lVehicleDirection3d.x, lVehicleDirection3d.y);
	const float lSteeringAngle = lWantedDirection.GetAngle(lVehicleDirection);
	lVehicle->SetEnginePower(1, lSteeringAngle);
	const float lBrakeLimit = Math::Lerp(10.0f, 50.0f, lVehicle->mPanicLevel);
	const float lSpeed = lVehicle->GetVelocity().GetLength();
	if (lSpeed > lBrakeLimit/2)
	{
		float lVelocityBrakeFactor = Math::Clamp(lSpeed, 0.0f, lBrakeLimit) / lBrakeLimit;
		const float lBrakePower = std::max(0.0f, (std::abs(lSteeringAngle)-0.5f)*0.5f*lVelocityBrakeFactor);
		lVehicle->SetEnginePower(2, lBrakePower);
	}

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
