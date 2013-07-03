
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "RoboBall.h"
#include "../TBC/Include/PhysicsEngine.h"
#include "Cutie.h"



namespace GrenadeRun
{



RoboBall::RoboBall(const Game* pGame, const str& pClassId):
	Parent(pGame->GetResourceManager(), pClassId, pGame->GetUiManager()),
	mGame(pGame),
	mSound(0),
	mTimeFrameBlown(-1)
{
	mHeadAwayTimer.EnableShadowCounter(true);
}

RoboBall::~RoboBall()
{
	delete mSound;
	mSound = 0;
	mGame = 0;
}



void RoboBall::OnTick()
{
	Parent::OnTick();

	if (mGame->GetCutie() && mGame->GetCutie()->IsLoaded())
	{
		float lStrength = Math::Lerp(0.25f, 1.0f, mGame->GetComputerDifficulty());
		const Vector3DF lPosition = GetPosition();
		const Vector3DF lCutiePosition = mGame->GetCutie()->GetPosition();
		const Vector3DF lDirection = lCutiePosition - lPosition;
		Vector3DF lSteerDirection;
		if (mHeadAwayTimer.QueryTimeDiff() > 2.0)
		{
			const Vector3DF lVelocity = GetVelocity();
			const Vector3DF lCutieVelocity = mGame->GetCutie()->GetVelocity();
			const float lDistance = lPosition.GetDistance(lCutiePosition);
			const float lSpeed = lVelocity.GetLength();
			if (lSpeed < 0.1f)
			{
				return;
			}
			const float lTimeTilImpact = Math::Clamp(lDistance / lSpeed, 0.1f, 3.0f);
			const float lSpeedDiff2 = lVelocity.GetDistanceSquared(lCutieVelocity);
			if ((lTimeTilImpact < 1 || lSpeed < 1) &&
				lDistance < 10 &&
				lSpeedDiff2 < 1)
			{
				log_adebug("Too close and too slow, backing up!");
				mHeadAwayTimer.PopTimeDiff();
			}
			const Vector3DF lFuturePosition = lPosition + lVelocity*lTimeTilImpact;
			const Vector3DF lFutureCutiePosition = lCutiePosition + lCutieVelocity*lTimeTilImpact;
			const Vector3DF lFutureDirection = lFutureCutiePosition - lFuturePosition;
			if (lFutureDirection.Dot(lDirection) > 1)
			{
				lSteerDirection = lFutureDirection;
			}
			else
			{
				// Just use the current position, to not get a vector of opposite direction when
				// the future position indicates we will have passed the target. Which hopefully
				// ends up in a big crash! :)
				lSteerDirection = lDirection;
			}
			if (lStrength < 0.6f && lDistance > 30.0f && lSpeed < 2)
			{
				++mBadSpeedCounter;
			}
			else
			{
				mBadSpeedCounter = 0;
			}
		}
		else
		{
			lSteerDirection = Vector3DF(100, 100, 13) - lPosition;
		}
		if (mBadSpeedCounter > 2)
		{
			lStrength = 0.6f;
		}
		lSteerDirection.z = 0;
		lSteerDirection.Normalize();
		GetPhysics()->GetEngine(0)->SetValue(0, lSteerDirection.y * lStrength);
		GetPhysics()->GetEngine(0)->SetValue(1, lSteerDirection.x * lStrength);
		static int c = 0;
		if (++c > 20)
		{
			c = 0;
			log_volatile(mLog.Debugf(_T("RoboBall at (%.1f, %.1f, %.1f), heading towards (%.1f, %.1f), diff (%.1f, %.1f, %.1f)."),
				lPosition.x, lPosition.y, lPosition.z,
				lSteerDirection.x, lSteerDirection.y,
				lDirection.x, lDirection.y, lDirection.z));
		}

		static int d = 0;
		if (++d > 20)
		{
			d = 0;
			if (lPosition.z < -50 || lPosition.z > 200)
			{
				mLog.AWarning("Fell off of playing field, resetting position!");
				const Cure::ObjectPositionalData* lPlacement;
				UpdateFullPosition(lPlacement);
				Cure::ObjectPositionalData* lNewPlacement = (Cure::ObjectPositionalData*)lPlacement->Clone();
				lNewPlacement->mPosition.mTransformation.GetPosition().Set(0, 0, 30);
				lNewPlacement->mPosition.mVelocity.Set(0, 0, 0);
				SetFullPosition(*lNewPlacement, 0);
			}
		}
	}
}


LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, RoboBall);



}
