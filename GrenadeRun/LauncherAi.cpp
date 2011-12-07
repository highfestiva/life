
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "LauncherAi.h"
#include "../Cure/Include/ContextManager.h"
#include "../Lepra/Include/Random.h"
#include "Ctf.h"
#include "Cutie.h"
#include "Game.h"
#include "Launcher.h"



namespace GrenadeRun
{



LauncherAi::LauncherAi(Game* pGame):
	Parent(pGame->GetResourceManager(), _T("LauncherAi")),
	mGame(pGame)
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
	Vector3DF lTargetVelocity = mGame->GetCutie()->GetVelocity() +
		mGame->GetCutie()->GetAcceleration() * 7;
	const float lTargetSpeed = lTargetVelocity.GetLength();
	const Vector3DF lCtfPosition = mGame->GetCtf()->GetPosition();
	const Vector3DF lDelta = lCtfPosition-lTargetPosition;
	const float lCtfDistance = lDelta.GetLength();
	const bool lHeadingTowardsCtf = ((lDelta/lCtfDistance).Dot(lTargetVelocity/lTargetSpeed) > 0.8f);
	if (lCtfDistance < 20*3)
	{
		// She's close, assume she's going to be close and brake hard soon.
		//lTargetPosition = (lTargetPosition+lCtfPosition) * 0.5f;
		lTargetVelocity *= 0.3f;
	}
	else if (lHeadingTowardsCtf)
	{
		if (lCtfDistance < 170*3)
		{
			if (lTargetSpeed * 10 > lCtfDistance * 1.2f)
			{
				// She's probably going to reach the target and brake hard,
				// so don't aim too far ahead.
				lTargetVelocity *= 0.4f;
			}
			else if (lTargetSpeed * 10 > lCtfDistance)
			{
				// She's going fast, but not superfast and thus she'll brake
				// hard, so don't aim too far ahead.
				lTargetVelocity *= 0.7f;
			}
		}
	}

	//lTargetPosition.x += (float)Random::Uniform(-2, 2);
	//lTargetPosition.y += (float)Random::Uniform(-2, 2);

	float lPitch;
	float lGuidePitch;
	float lYaw;
	float lGuideYaw;
	mGame->GetLauncher()->GetAngles(lTargetPosition, lTargetVelocity, lPitch, lGuidePitch, lYaw, lGuideYaw);
	const float lVelocity = lTargetVelocity.GetLength();
	const float lPitchFactor = ::fabs((lPitch - lGuidePitch) * (lVelocity+3) + 0.03f);
	if (lPitch < lGuidePitch)
	{
		mGame->GetLauncher()->SetEnginePower(0, -1*lPitchFactor, 0);
	}
	else
	{
		mGame->GetLauncher()->SetEnginePower(0, +1*lPitchFactor, 0);
	}
	const float lYawFactor = ::fabs((lYaw - lGuideYaw) * (lVelocity+4) + 0.03f);
	if (lYaw < lGuideYaw)
	{
		mGame->GetLauncher()->SetEnginePower(1, -1*lYawFactor, 0);
	}
	else
	{
		mGame->GetLauncher()->SetEnginePower(1, +1*lYawFactor, 0);
	}
	const double lLastShotDiff = mLastShot.QueryTimeDiff();
	if (lLastShotDiff > 2.5f &&	// Wait at least this long.
		((lYawFactor < 0.1f && lPitchFactor < 0.1f) ||	// In range.
		lLastShotDiff > 5.0f))
	{
		mGame->Shoot();
		mLastShot.ClearTimeDiff();
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, LauncherAi);



}
