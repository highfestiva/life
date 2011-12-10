
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
#include "Grenade.h"
#include "Launcher.h"
#include "Level.h"



#define	NORMAL_AIM_AHEAD		10.0f	// How far ahead to try and intersect the path.
#define GOAL_DISTANCE			4.2f	// When to slow down before stopping at goal.
#define OFF_COURSE_DISTANCE		4.5f	// When to start heading back.
#define END_PATH_TIME			0.9999f	// Time is considered at end of path.
#define	OFF_END_PATH_TIME		0.9995f	// Close to end, but not quite.
#define DOUBLE_OFF_END_PATH_TIME	0.9990f	// Close to end, but not quite. Double that.
#define REACT_TO_GRENADE_HEIGHT		50.0f	// How low a grenade needs to be for AI to take notice.


namespace GrenadeRun
{



typedef Cure::ContextPath::SplinePath Spline;



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
	mStoppedFrame = -1;
	mActivePath = -1;
	SetMode(MODE_FIND_BEST_PATH);
	GetManager()->EnableTickCallback(this);
}



void VehicleAi::OnTick()
{
	if (!mGame->GetCutie() || !mGame->GetCutie()->IsLoaded() ||
		!mGame->GetLevel() || !mGame->GetLevel()->IsLoaded() ||
		mGame->GetFlybyMode() != Game::FLYBY_INACTIVE)
	{
		return;
	}

	const Cure::TimeManager* lTime = GetManager()->GetGameManager()->GetTimeManager();
	const int lModeRunDeltaFrameCount = lTime->GetCurrentPhysicsFrameDelta(mModeStartFrame);
	const float lModeRunTime = lTime->ConvertPhysicsFramesToSeconds(lModeRunDeltaFrameCount);

	const float lStrength = Math::Lerp(0.6f, 1.0f, mGame->GetComputerDifficulty());
	const Vector3DF lPosition = mGame->GetCutie()->GetPosition();
	const Vector3DF lVelocity = mGame->GetCutie()->GetVelocity();
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
			Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(mActivePath);
			const float lWantedDistance = SCALE_FACTOR * NORMAL_AIM_AHEAD;
			float lStep = lWantedDistance * lPath->GetDistanceNormal();
			if (lStep + lPath->GetCurrentInterpolationTime() > 1)
			{
				lStep = 1 - lPath->GetCurrentInterpolationTime();
			}
			lPath->StepInterpolation(lStep);
			SetMode(MODE_HEADING_BACK_ON_TRACK);
			mLog.Headlinef(_T("Picked path %i (%i pickable)."), mActivePath, lRelevantPaths.size());
		}
		break;
		case MODE_HEADING_BACK_ON_TRACK:
		{
			if (lModeRunDeltaFrameCount%5 == 2)
			{
				const float lVelocityScaleFactor = std::min(1.0f, lVelocity.GetLength() / 5);
				if (GetClosestPathDistance(lPosition) < SCALE_FACTOR * OFF_COURSE_DISTANCE * lVelocityScaleFactor)
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
		}
		// TRICKY: fall through.
		case MODE_NORMAL:
		{
			if (mMode == MODE_NORMAL && lModeRunDeltaFrameCount%20 == 19)
			{
				const float lVelocityScaleFactor = std::min(1.0f, lVelocity.GetLength() / 5);
				if (GetClosestPathDistance(lPosition) > SCALE_FACTOR * OFF_COURSE_DISTANCE * lVelocityScaleFactor)
				{
					SetMode(MODE_HEADING_BACK_ON_TRACK);
					return;
				}
			}

			Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(mActivePath);
			Vector3DF lTarget = lPath->GetValue();

			// Check if vehicle stopped. That would mean either crashed against something or too steep hill.
			if (lModeRunDeltaFrameCount%7 == 4 && mGame->GetCutie()->GetHealth() > 0)
			{
				const float lSlowSpeed = 0.2f * SCALE_FACTOR;
				if (lVelocity.GetLengthSquared() < lSlowSpeed*lSlowSpeed)
				{
					if (mStoppedFrame == -1)
					{
						mStoppedFrame = lTime->GetCurrentPhysicsFrame();
					}
					const int lStoppedDeltaFrameCount = lTime->GetCurrentPhysicsFrameDelta(mStoppedFrame);
					const float lStoppedTime = lTime->ConvertPhysicsFramesToSeconds(lStoppedDeltaFrameCount);
					if (lStoppedTime >= 1.0f)
					{
						const Vector3DF lDirection = mGame->GetCutie()->GetOrientation() * Vector3DF(0,1,0);
						const Vector3DF lWantedDirection = lTarget-lPosition;
						const float lForwardAngle = Vector2DF(lWantedDirection.x, lWantedDirection.y).GetAngle(Vector2DF(lDirection.x, lDirection.y));
						// Amplify angle to be either full left or full right.
						const float lAngle = (lForwardAngle < 0)? -1.0f : 1.0f;
						mGame->GetCutie()->SetEnginePower(1, -lAngle, 0);
						mStoppedFrame = -1;
						SetMode(MODE_BACKING_UP);
						return;
					}
				}
				else
				{
					mStoppedFrame = -1;
				}
			}

			// Check if incoming.
			if (lModeRunDeltaFrameCount%5 == 1)
			{
				if (AvoidGrenade(lPosition, lVelocity, 0.5f))
				{
					return;
				}
			}

			// Step target (aim) ahead.
			{
				const float lActualDistance2 = lTarget.GetDistanceSquared(lPosition);
				const float lWantedDistance = SCALE_FACTOR * NORMAL_AIM_AHEAD * std::min(1.0f, lVelocity.GetLength() / 10);
				if (lActualDistance2 < lWantedDistance*lWantedDistance)
				{
					const float lMoveAhead = lWantedDistance*1.1f - ::sqrt(lActualDistance2);
					const float lPreStepTime = lPath->GetCurrentInterpolationTime();
					lPath->StepInterpolation(lMoveAhead * lPath->GetDistanceNormal());
					// Did wrap around? That means target is close to the end of our path, so we
					// should try to stop.
					if (lPreStepTime > 0.9f && lPath->GetCurrentInterpolationTime() < 0.1f)
					{
						lPath->GotoAbsoluteTime(END_PATH_TIME);
						if (mGame->GetCutie()->GetForwardSpeed() > 2.0f*SCALE_FACTOR ||
							IsCloseToTarget(lPosition, GOAL_DISTANCE))
						{
							SetMode(MODE_AT_GOAL);
							return;
						}
					}
					lTarget = lPath->GetValue();
				}
			}

			// Move forward.
			mGame->GetCutie()->SetEnginePower(0, +lStrength, 0);
			mGame->GetCutie()->SetEnginePower(2, 0, 0);

			// Steer.
			const Vector3DF lDirection = mGame->GetCutie()->GetOrientation() * Vector3DF(0,1,0);
			const Vector3DF lWantedDirection = lTarget-lPosition;
			const float lAngle = Vector2DF(lWantedDirection.x, lWantedDirection.y).GetAngle(Vector2DF(lDirection.x, lDirection.y));
			mGame->GetCutie()->SetEnginePower(1, +lAngle, 0);

			// Check if we need to slow down.
			const float lHighSpeed = SCALE_FACTOR * 2.5f;
			const float lAbsAngle = ::fabs(lAngle);
			if (lVelocity.GetLengthSquared() > lHighSpeed*lHighSpeed)
			{
				if (lAbsAngle > 0.5f)
				{
					mGame->GetCutie()->SetEnginePower(2, lAbsAngle*0.001f + lVelocity.GetLength()*0.0001f, 0);
				}
				else if (lPath->GetCurrentInterpolationTime() >= DOUBLE_OFF_END_PATH_TIME &&
					IsCloseToTarget(lPosition, 3.5f*GOAL_DISTANCE))
				{
					mGame->GetCutie()->SetEnginePower(2, 0.3f, 0);
				}
			}
		}
		break;
		case MODE_BACKING_UP:
		{
			// Brake or move backward.
			const bool lIsMovingForward = (mGame->GetCutie()->GetForwardSpeed() > 0.1f*SCALE_FACTOR);
			mGame->GetCutie()->SetEnginePower(0, lIsMovingForward? 0.0f : -lStrength, 0);
			mGame->GetCutie()->SetEnginePower(2, lIsMovingForward? lStrength :  0.0f, 0);

			if (!lIsMovingForward && lModeRunTime > 1.7f)
			{
				SetMode(MODE_HEADING_BACK_ON_TRACK);
				return;
			}
		}
		break;
		case MODE_AT_GOAL:
		{
			if (lModeRunDeltaFrameCount%5 == 3 && mGame->GetCtf()->GetCaptureLevel() < 0.85f)
			{
				if (AvoidGrenade(lPosition, lVelocity, 1))
				{
					return;
				}
			}
			Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(mActivePath);
			if (!IsCloseToTarget(lPosition, GOAL_DISTANCE))
			{
				lPath->GotoAbsoluteTime(DOUBLE_OFF_END_PATH_TIME);	// Close to end, but not at end.
				SetMode(MODE_HEADING_BACK_ON_TRACK);
				return;
			}
			// Brake!
			mGame->GetCutie()->SetEnginePower(0, 0, 0);
			mGame->GetCutie()->SetEnginePower(2, -lStrength, 0);	// Negative = use full brakes, not only hand brake.
		}
		break;
	}
}

bool VehicleAi::AvoidGrenade(const Vector3DF& pPosition, const Vector3DF& pVelocity, float pCaution)
{
	if (mGame->GetComputerDifficulty() < 0.6f)
	{
		return false;
	}

	// Walk all objects, pick out grenades.
	const Cure::ContextManager::ContextObjectTable& lObjectTable = GetManager()->GetObjectTable();
	Cure::ContextManager::ContextObjectTable::const_iterator x = lObjectTable.begin();
	for (; x != lObjectTable.end(); ++x)
	{
		Grenade* lGrenade = dynamic_cast<Grenade*>(x->second);
		if (!lGrenade)
		{
			continue;
		}
		// Ignore those too high up, too far down or still heading upwards.
		const Vector3DF lGrenadePosition = lGrenade->GetPosition();
		const float h = lGrenadePosition.z - pPosition.z;
		const Vector3DF lGrenadeVelocity = lGrenade->GetVelocity();
		if (h > REACT_TO_GRENADE_HEIGHT * SCALE_FACTOR ||
			h < 4.0f * SCALE_FACTOR ||
			lGrenadeVelocity.z >= 0)
		{
			continue;
		}
		float t;
		{
			const float vup = lGrenadeVelocity.z;
			// g*t^2/2 - vup*t + h = 0
			//
			// Quaderatic formula:
			// ax^2 + bx + c = 0
			// =>
			//     -b +- sqrt(b^2 - 4ac)
			// x = ---------------------
			//             2a
			const float a = 9.82f/2;
			const float b = -vup;
			const float c = -h;
			const float b2 = b*b;
			const float _4ac = 4*a*c;
			if (b2 < _4ac)	// Will never rise high enough.
			{
				mLog.AHeadline("Ignoring grenade, ballistic says it can not hit.");
				continue;
			}
			t = (-b + sqrt(b2 - _4ac)) / (2*a);
			assert(t > 0);
		}
		const Vector2DF lGrenadeTarget(lGrenadePosition.x + lGrenadeVelocity.x * t,
			lGrenadePosition.y + lGrenadeVelocity.y * t);
		// Compare against some point in front of my vehicle, so that I won't break
		// if it's better to speed up.
		const float lDamageRange = 8.0f * SCALE_FACTOR * pCaution;
		const Vector2DF lMyTarget(pPosition.x + pVelocity.x * t,
			pPosition.y + pVelocity.y * t);
		Vector2DF lMyExtraStep(pVelocity.x, pVelocity.y);
		if (lMyExtraStep.GetLengthSquared() < SCALE_FACTOR*SCALE_FACTOR)
		{
			lMyExtraStep.Set(0, 0);
		}
		else
		{
			// Overlap the sweet spot some with my probable position.
			lMyExtraStep.Normalize(lDamageRange*0.6f);
		}
		if (lGrenadeTarget.GetDistanceSquared(lMyTarget+lMyExtraStep) < lDamageRange*lDamageRange)
		{
			mLog.Headlinef(_T("Grenade would hit: %f m (%f s)."), lGrenadeTarget.GetDistance(lMyTarget)/3, t);
			mGame->GetCutie()->SetEnginePower(1, 0, 0);	// Backup straight.
			SetMode(MODE_BACKING_UP);
			return true;
		}
		else
		{
			mLog.Headlinef(_T("Doging the grenade: %f m (%f s)."), lGrenadeTarget.GetDistance(lMyTarget)/3, t);
		}
	}
	return false;
}

void VehicleAi::SetMode(Mode pMode)
{
	mMode = pMode;
	mModeStartFrame = GetManager()->GetGameManager()->GetTimeManager()->GetCurrentPhysicsFrame();
	const tchar* lModeName = _T("???");
	switch (mMode)
	{
		case MODE_FIND_BEST_PATH:		lModeName = _T("FIND PATH");			break;
		case MODE_NORMAL:			lModeName = _T("NORMAL");			break;
		case MODE_HEADING_BACK_ON_TRACK:	lModeName = _T("HEADING BACK ON TRACK");	break;
		case MODE_BACKING_UP:			lModeName = _T("BACKING UP");			break;
		case MODE_AT_GOAL:			lModeName = _T("AT GOAL");			break;
	}
	mLog.Headlinef(_T("Switching mode to %s."), lModeName);
}

bool VehicleAi::IsCloseToTarget(const Vector3DF& pPosition, float pDistance) const
{
	Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(mActivePath);
	const Vector3DF lTarget = lPath->GetValue();
	const float lTargetDistance2 = lTarget.GetDistanceSquared(pPosition);
	const float lGoalDistance = pDistance*SCALE_FACTOR;
	return (lTargetDistance2 <= lGoalDistance*lGoalDistance);
}

float VehicleAi::GetClosestPathDistance(const Vector3DF& pPosition, int pPath) const
{
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
