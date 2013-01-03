
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "LauncherAi.h"
#include "../Cure/Include/ContextManager.h"
#include "../Lepra/Include/Random.h"
#include "../TBC/Include/PhysicsEngine.h"
#include "Ctf.h"
#include "Cutie.h"
#include "Game.h"
#include "Launcher.h"



namespace GrenadeRun
{



LauncherAi::LauncherAi(Game* pGame):
	Parent(pGame->GetResourceManager(), _T("LauncherAi")),
	mGame(pGame),
	mDidShoot(false),
	mShotCount(0)
{
}

LauncherAi::~LauncherAi()
{
}

void LauncherAi::Init()
{
	GetManager()->EnableTickCallback(this);
}



void LauncherAi::OnTick()
{
	if (mGame->GetFlybyMode() != Game::FLYBY_INACTIVE ||
		!mGame->GetLauncher() || !mGame->GetLauncher()->IsLoaded() ||
		!mGame->GetCutie() || !mGame->GetCutie()->IsLoaded())
	{
		return;
	}

	Vector3DF lTargetPosition = mGame->GetCutie()->GetPosition();
	Vector3DF lTargetVelocity = (mGame->GetCutie()->GetVelocity() +
		mGame->GetCutie()->GetAcceleration() * 7) * 1.1f;
	const float lTargetSpeed = lTargetVelocity.GetLength();
	const Vector3DF lCtfPosition = mGame->GetCtf()->GetPosition();
	Vector3DF lDirection = lCtfPosition-lTargetPosition;
	const float lCtfDistance = lDirection.GetLength();
	lDirection /= lCtfDistance;
	const bool lHeadingTowardsCtf = (lDirection.Dot(lTargetVelocity/lTargetSpeed) > 0.8f);
	bool lAdjustedForSlowingDown = false;
	const float lDifficulty = mGame->GetComputerDifficulty();
	bool lHandled = false;
	if (mDidShoot && mGame->IsLauncherBarrelFree())
	{
		mLog.AHeadline("Fire in the hole!");
		++mShotCount;
		mDidShoot = false;
	}
	if (lDifficulty > 0.9f)
	{
		if (lCtfDistance < 60*SCALE_FACTOR && lCtfPosition.z-lTargetPosition.z < 7)
		{
			lHandled = true;
			if (mShotCount > 2)
			{
				mShotCount = 0;
			}
			// Alternate between firing twice at CTF platform and once at vehicle.
			if (mShotCount <= 1)
			{
				lTargetPosition = lCtfPosition;
				lTargetPosition.x += -lDirection.x * (2 * SCALE_FACTOR) + (float)Random::Uniform(-0.3f*SCALE_FACTOR, 0.3f*SCALE_FACTOR);
				lTargetPosition.y += -lDirection.y * (2 * SCALE_FACTOR) + (float)Random::Uniform(-0.3f*SCALE_FACTOR, 0.3f*SCALE_FACTOR);
				lTargetVelocity.Set(0, 0, 0);
				mTargetOffset.Set(0, 0, 0);
				lAdjustedForSlowingDown = true;
				mLog.AHeadline("Shooting at CTF platform!");
			}
			else if (lHeadingTowardsCtf)	// Only assume slowdown if going towards our goal.
			{
				lTargetVelocity *= 0.3f;
				mTargetOffset.Set(0, 0, 0);
				lAdjustedForSlowingDown = true;
				mLog.AHeadline("Shooting at slowing vehicle!");
			}
			else
			{
				mTargetOffset.Set(0, 0, 0);
				mLog.AHeadline("Shooting at vehicle plain and simple!");
			}
		}
	}
	else
	{
		if (lCtfDistance < 20*SCALE_FACTOR)
		{
			lHandled = true;
			// She's close, assume she's going to be close and brake hard soon.
			//lTargetPosition = (lTargetPosition+lCtfPosition) * 0.5f;
			lTargetVelocity *= 0.3f;
			lAdjustedForSlowingDown = true;
		}
	}
	if (!lHandled && lHeadingTowardsCtf)
	{
		if (lCtfDistance < 170*SCALE_FACTOR)
		{
			if (lCtfPosition.z-lTargetPosition.z > 7)
			{
				// Nevermind - this gal is far below the goal. Keep aiming at her instead of camping at the goal.
			}
			else if (lTargetSpeed * 10 > lCtfDistance * 1.2f)
			{
				// She's probably going to reach the target and brake hard,
				// so don't aim too far ahead.
				lTargetVelocity *= 0.4f;
				lAdjustedForSlowingDown = true;
			}
			else if (lTargetSpeed * 10 > lCtfDistance)
			{
				// She's going fast, but not superfast and thus she'll brake
				// hard, so don't aim too far ahead.
				lTargetVelocity *= 0.7f;
				lAdjustedForSlowingDown = true;
			}
		}
	}

	//lTargetPosition.x += (float)Random::Uniform(-2, 2);
	//lTargetPosition.y += (float)Random::Uniform(-2, 2);

	float lPitch;
	float lGuidePitch;
	float lYaw;
	float lGuideYaw;
	mGame->GetLauncher()->GetAngles(lTargetPosition+mTargetOffset, lTargetVelocity, lPitch, lGuidePitch, lYaw, lGuideYaw);
	const float lVelocity = lTargetVelocity.GetLength();
	const float lPitchFactor = ::fabs((lPitch - lGuidePitch) * (lVelocity+3) + 0.03f);
	if (lPitch < lGuidePitch)
	{
		mGame->GetLauncher()->SetEnginePower(0, -1*lPitchFactor);
	}
	else
	{
		mGame->GetLauncher()->SetEnginePower(0, +1*lPitchFactor);
	}
	const float lYawFactor = ::fabs((lYaw - lGuideYaw) * (lVelocity+4) + 0.03f);
	if (lYaw < lGuideYaw)
	{
		mGame->GetLauncher()->SetEnginePower(1, -1*lYawFactor);
	}
	else
	{
		mGame->GetLauncher()->SetEnginePower(1, +1*lYawFactor);
	}
	const float lLongestTimeBase = 8.0f * (1-lDifficulty*0.8f);
	const double lLastShotDiff = mLastShot.QueryTimeDiff();
	if (lLastShotDiff > lLongestTimeBase &&	// Wait at least this long.
		((lYawFactor < 0.1f && lPitchFactor < 0.1f) ||	// In range.
		lLastShotDiff > lLongestTimeBase*2))
	{
		if (mGame->Shoot())
		{
			mDidShoot = true;
			mLastShot.ClearTimeDiff();
			if (lDifficulty >= 0.7f)
			{
				// Good player has some tactics.
				if (lTargetSpeed > 2*SCALE_FACTOR && !lAdjustedForSlowingDown &&
					mGame->GetCutie()->GetPhysics()->GetEngineCount() >= 2)
				{
					// Guess direction depending on steering.
					const float lAngle = mGame->GetCutie()->GetPhysics()->GetEngine(1)->GetValue();
					Vector3DF lDirection = mGame->GetCutie()->GetForwardDirection();
					QuaternionF lRotation(lAngle*-0.5f, Vector3DF(0, 0, 1));
					lDirection = lRotation * lDirection;
					lDirection.z = 0;
					const float lAdjustedTargetSpeed = (lTargetSpeed > 10)? 10 : lTargetSpeed;
					mTargetOffset = lDirection * lAdjustedTargetSpeed * 1.0f;
					if (lTargetSpeed < 30)
					{
						mTargetOffset += lTargetVelocity * 2.0f;
					}
				}
				else
				{
					mTargetOffset.Set(0, 0, 0);
				}
			}
			else if (lDifficulty <= 0.3f)
			{
				// Poor player spreads 'em a lot.
				const float o = 16*SCALE_FACTOR / (0.8f+lDifficulty*8);
				mTargetOffset.Set((float)Random::Uniform(-o, o), (float)Random::Uniform(-o, o), 0);
			}
			else
			{
				// Average computer player always spreads 'em a little.
				const float o = 5*SCALE_FACTOR;
				mTargetOffset.Set((float)Random::Uniform(-o, o), (float)Random::Uniform(-o, o), 0);
			}
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, LauncherAi);



}
