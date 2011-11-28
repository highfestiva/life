
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "VehicleAi.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/ContextPath.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/Random.h"
#include "Ctf.h"
#include "Cutie.h"
#include "Game.h"
#include "Launcher.h"
#include "Level.h"



#define	SCALE_FACTOR			3	// Hard-coded for Kill Cutie. If ever re-using for other game, drop this.
#define	NORMAL_AIM_AHEAD		10.0f	// How far ahead to try and intersect the path.
#define GOAL_DISTANCE			4.5f	// When to slow down before stopping at goal.
#define OFF_COURSE_DISTANCE		4.5f	// When to start heading back.
#define END_PATH_TIME			0.9999f	// Time is considered at end of path.
#define	OFF_END_PATH_TIME		0.9995f	// Close to end, but not quite.
#define DOUBLE_OFF_END_PATH_TIME	0.9990f	// Close to end, but not quite. Double that.


namespace GrenadeRun
{



VehicleAi::VehicleAi(Game* pGame):
	Parent(pGame->GetResourceManager(), _T("VehicleAi")),
	mGame(pGame)
{
}

VehicleAi::~VehicleAi()
{
}

void VehicleAi::Init()
{
	mActivePath = -1;
	SetMode(MODE_FIND_BEST_PATH);
	GetManager()->EnableTickCallback(this);
}



void VehicleAi::OnTick()
{
	if (!mGame->GetCutie() || !mGame->GetCutie()->IsLoaded() ||
		!mGame->GetLevel() || !mGame->GetLevel()->IsLoaded())
	{
		return;
	}

	const int lModeRunDeltaFrameCount = GetManager()->GetGameManager()->GetTimeManager()->GetCurrentPhysicsFrameDelta(mModeStartFrame);
	const float lModeRunTime = GetManager()->GetGameManager()->GetTimeManager()->ConvertPhysicsFramesToSeconds(lModeRunDeltaFrameCount);

	typedef Cure::ContextPath::SplinePath Spline;
	const Vector3DF lPosition = mGame->GetCutie()->GetPosition();
	switch (mMode)
	{
		case MODE_FIND_BEST_PATH:
		{
			mLog.AHeadline("Trying to find new path...");
			float lStartTime = 0;
			if (mActivePath != -1)
			{
				// Synchronize all paths.
				Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(mActivePath);
				lStartTime = lPath->GetCurrentInterpolationTime();
				mActivePath = -1;
			}
			int lBestPathDistance = 1000000;
			std::vector<int> lRelevantPaths;
			const int lPathCount = mGame->GetLevel()->QueryPath()->GetPathCount();
			for (int x = 0; x < lPathCount; ++x)
			{
				mGame->GetLevel()->QueryPath()->GetPath(x)->GotoAbsoluteTime(lStartTime);
				const int lRoundedNearestDistance = (int)(GetClosestPathDistance(lPosition, x)/SCALE_FACTOR/2);
				if (lRoundedNearestDistance < lBestPathDistance)
				{
					lRelevantPaths.clear();
					lRelevantPaths.push_back(x);
					lBestPathDistance = lRoundedNearestDistance;
				}
				else if (lRoundedNearestDistance == lBestPathDistance)
				{
					lRelevantPaths.push_back(x);
				}
			}
			assert(!lRelevantPaths.empty());
			if (lRelevantPaths.empty())
			{
				return;
			}
			mActivePath = lRelevantPaths[Random::GetRandomNumber() % lRelevantPaths.size()];
			SetMode(MODE_HEADING_BACK_ON_TRACK);
			mLog.Headlinef(_T("Picked path %i (%i pickable)."), mActivePath, lRelevantPaths.size());
		}
		break;
		case MODE_HEADING_BACK_ON_TRACK:
		{
			if (GetClosestPathDistance(lPosition) <= SCALE_FACTOR*OFF_COURSE_DISTANCE)
			{
				// We were able to return to normal, keep on running.
				SetMode(MODE_NORMAL);
				return;
			}
			else if (lModeRunTime > 7.0f)
			{
				SetMode(MODE_FIND_BEST_PATH);
				return;
			}
		}
		// TRICKY: fall through.
		case MODE_NORMAL:
		{
			if (mMode == MODE_NORMAL && lModeRunDeltaFrameCount%20 == 19)
			{
				if (GetClosestPathDistance(lPosition) > SCALE_FACTOR*OFF_COURSE_DISTANCE)
				{
					SetMode(MODE_HEADING_BACK_ON_TRACK);
					return;
				}
			}

			Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(mActivePath);
			Vector3DF lTarget = lPath->GetValue();
			{
				// Step aim ahead.
				const float lActualDistance2 = lTarget.GetDistanceSquared(lPosition);
				const float lWantedDistance = SCALE_FACTOR * NORMAL_AIM_AHEAD;
				if (lActualDistance2 < lWantedDistance*lWantedDistance)
				{
					const float lMoveAhead = lWantedDistance*1.1f - ::sqrt(lActualDistance2);
					const float lPreStepTime = lPath->GetCurrentInterpolationTime();
					lPath->StepInterpolation(lMoveAhead * lPath->GetDistanceNormal());
					if (lPreStepTime > 0.9f && lPath->GetCurrentInterpolationTime() < 0.1f)
					{
						lPath->GotoAbsoluteTime(END_PATH_TIME);
						SetMode(MODE_AT_GOAL);
						return;
					}
					lTarget = lPath->GetValue();
				}
			}
			const Vector3DF lDirection = mGame->GetCutie()->GetOrientation() * Vector3DF(0,1,0);
			const Vector3DF lWantedDirection = lTarget-lPosition;
			if (lPath->GetCurrentInterpolationTime() < OFF_END_PATH_TIME)
			{
				// Move forward.
				mGame->GetCutie()->SetEnginePower(0, +1.0f, 0);
				mGame->GetCutie()->SetEnginePower(2, 0, 0);
			}
			const float lAngle = Vector2DF(lDirection.x, lDirection.y).GetAngle(Vector2DF(lWantedDirection.x, lWantedDirection.y));
			if (mMode == MODE_NORMAL && lModeRunDeltaFrameCount%20 == 10)
			{
				mLog.Infof(_T("Angle: %f"), lAngle);
			}
			mGame->GetCutie()->SetEnginePower(1, -lAngle, 0);
		}
		break;
		case MODE_AT_GOAL:
		{
			Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(mActivePath);
			const Vector3DF lTarget = lPath->GetValue();
			const float lTargetDistance2 = lTarget.GetDistanceSquared(lPosition);
			const float lGoalDistance = GOAL_DISTANCE*SCALE_FACTOR;
			if (lTargetDistance2 > lGoalDistance*lGoalDistance)
			{
				lPath->GotoAbsoluteTime(DOUBLE_OFF_END_PATH_TIME);	// Close to end, but not at end.
				SetMode(MODE_HEADING_BACK_ON_TRACK);
				return;
			}
			// Brake!
			mGame->GetCutie()->SetEnginePower(0, 0, 0);
			mGame->GetCutie()->SetEnginePower(2, +1.0f, 0);
		}
		break;
	}
}

void VehicleAi::SetMode(Mode pMode)
{
	mMode = pMode;
	mModeStartFrame = GetManager()->GetGameManager()->GetTimeManager()->GetCurrentPhysicsFrame();
	const tchar* lModeName = _T("???");
	switch (mMode)
	{
		case MODE_FIND_BEST_PATH:		lModeName = _T("FIND PATH");		break;
		case MODE_NORMAL:			lModeName = _T("NORMAL");		break;
		case MODE_HEADING_BACK_ON_TRACK:	lModeName = _T("GO BACK ON TRACK");	break;
		case MODE_BACKING_UP_DUE_TO_CRASH:	lModeName = _T("BACKING UP");		break;
		case MODE_CIRCUMVENTING_AFTER_BACKING:	lModeName = _T("CIRCUMVENTING");	break;
		case MODE_BACKING_ON_TRACK:		lModeName = _T("BACKING TO TRACK");	break;
		case MODE_REVERSING_DUE_TO_GRENADE:	lModeName = _T("AVOID GRENADE");	break;
		case MODE_AT_GOAL:			lModeName = _T("AT GOAL");		break;
	}
	mLog.Headlinef(_T("Switching mode to %s."), lModeName);
}

float VehicleAi::GetClosestPathDistance(const Vector3DF& pPosition, int pPath) const
{
	typedef Cure::ContextPath::SplinePath Spline;
	Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath((pPath >= 0)? pPath : mActivePath);
	const float lCurrentTime = lPath->GetCurrentInterpolationTime();

	// We can assume the path "current" pointer is a bit ahead, so step back some to get a closer
	// approximation of where to start looking for our closest point on the spline.
	const float lWantedDistance = SCALE_FACTOR * NORMAL_AIM_AHEAD;
	float lDeltaTime = -lWantedDistance * lPath->GetDistanceNormal();
	if (lCurrentTime+lDeltaTime < 0)
	{
		lDeltaTime = -lCurrentTime;
	}
	lPath->StepInterpolation(lDeltaTime);

	float lNearestDistance;
	Vector3DF lClosestPoint;
	const float lSearchStepLength = (pPath >= 0)? -0.2f : 0.025f;
	const int lSearchSteps = (pPath >= 0)? 10 : 3;
	lPath->FindNearestTime(lSearchStepLength, pPosition, lNearestDistance, lClosestPoint, lSearchSteps);

	if (pPath < 0)
	{
		// Step back to target point.
		lPath->GotoAbsoluteTime(lCurrentTime);
	}

	return lNearestDistance + ::fabs(pPosition.z-lNearestDistance);	// Add z diff too, since that is a major factor for "closeness".
}


LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, VehicleAi);



}
