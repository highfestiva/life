
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "LauncherAi.h"
#include "../Cure/Include/ContextManager.h"
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
	if (!mGame->GetLauncher()->IsLoaded() || !mGame->GetCutie()->IsLoaded())
	{
		return;
	}

	Vector3DF lTargetPosition = mGame->GetCutie()->GetPosition();
	Vector3DF lTargetVelocity = mGame->GetCutie()->GetVelocity();
	if (lTargetPosition.GetDistance(mGame->GetCtf()->GetPosition()) < 20*3)
	{
		lTargetPosition = mGame->GetCtf()->GetPosition();
		lTargetVelocity = Vector3DF();
	}

	float lPitch;
	float lGuidePitch;
	float lYaw;
	float lGuideYaw;
	mGame->GetLauncher()->GetAngles(lTargetPosition, lTargetVelocity, lPitch, lGuidePitch, lYaw, lGuideYaw);
	const float lVelocity = lTargetVelocity.GetLength();
	const float lPitchFactor = ::fabs((lPitch - lGuidePitch) * (lVelocity+5));
	if (lPitch < lGuidePitch)
	{
		mGame->GetLauncher()->SetEnginePower(0, -1*lPitchFactor, 0);
	}
	else
	{
		mGame->GetLauncher()->SetEnginePower(0, +1*lPitchFactor, 0);
	}
	const float lYawFactor = ::fabs((lYaw - lGuideYaw) * (lVelocity+5));
	if (lYaw < lGuideYaw)
	{
		mGame->GetLauncher()->SetEnginePower(1, -1*lYawFactor, 0);
	}
	else
	{
		mGame->GetLauncher()->SetEnginePower(1, +1*lYawFactor, 0);
	}
	if (lYawFactor < 0.1f && lPitchFactor < 0.1f)
	{
		mGame->Shoot();
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, LauncherAi);



}
