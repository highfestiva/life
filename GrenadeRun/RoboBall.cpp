
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



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
		const Vector3DF lPosition = GetPosition();
		float lAngle;
		if (mHeadAwayTimer.QueryTimeDiff() > 2.0)
		{
			const Vector3DF lVelocity = GetVelocity();
			const Vector3DF lCutiePosition = mGame->GetCutie()->GetPosition();
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
				mLog.AHeadline("Too close and too slow, backing up!");
				mHeadAwayTimer.PopTimeDiff();
			}
			const Vector3DF lFuturePosition = lPosition + lVelocity*lTimeTilImpact;
			const Vector3DF lFutureCutiePosition = lCutiePosition + lCutieVelocity*lTimeTilImpact;
			const Vector3DF lFutureDirection = lFutureCutiePosition - lFuturePosition;
			const Vector3DF lDirection = lCutiePosition - lPosition;
			if (lFutureDirection.Dot(lDirection) > 1)
			{
				lAngle = -LEPRA_XY_ANGLE(lFutureDirection, Vector3DF(1, 0, 0));
			}
			else
			{
				// Just use the current position, to not get a vector of opposite direction when
				// the future position indicates we will have passed the target. Which hopefully
				// ends up in a big crash! :)
				lAngle = -LEPRA_XY_ANGLE(lDirection, Vector3DF(1, 0, 0));
			}
		}
		else
		{
			const Vector3DF lDirection = Vector3DF(100, 100, 13) - lPosition;
			lAngle = -LEPRA_XY_ANGLE(lDirection, Vector3DF(1, 0, 0));
		}
		const float lStrength = Math::Lerp(0.25f, 1.0f, mGame->GetComputerDifficulty());
		GetPhysics()->GetEngine(0)->SetValue(0, lStrength, lAngle);
		static int c = 0;
		if (++c > 20)
		{
			c = 0;
			mLog.Headlinef(_T("RoboBall at (%.1f, %.1f, %.1f). Direction %.0f."), lPosition.x, lPosition.y, lPosition.z, lAngle/PIF*180);
		}
	}
}


LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, RoboBall);



}
