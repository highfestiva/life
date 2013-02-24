
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "VehicleAi.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/ContextPath.h"
#include "../Cure/Include/Elevator.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/Random.h"
#include "Ctf.h"
#include "Cutie.h"
#include "Game.h"
#include "Grenade.h"
#include "Launcher.h"
#include "Level.h"



#define	NORMAL_AIM_AHEAD		5.0f	// How far ahead to try and intersect the path.
#define ON_GOAL_DISTANCE		3.2f	// When at goal.
#define ELEVATOR_WAIT_DISTANCE		8.8f	// When close to elevator.
#define ELEVATOR_TOO_CLOSE_DISTANCE	8.2f	// When too close to elevator.
#define ELEVATOR_GOT_OFF_EXTRA_DISTANCE	1.7f	// Distance to get off elevator.
#define ON_ELEVATOR_DISTANCE		2.3f	// When on elevator.
#define ELEVATOR_FAR_DISTANCE		15.0f	// When far to elevator.
#define SLOW_DOWN_DISTANCE		6.0f	// When to slow down before stopping at goal.
#define OFF_COURSE_DISTANCE		4.5f	// When to start heading back.
#define TOTALLY_OFF_COURSE_DISTANCE	16.0f	// When fallen off some cliff or similar.
#define END_PATH_TIME			0.9999f	// Time is considered at end of path.
#define	OFF_END_PATH_TIME		0.9995f	// Close to end, but not quite.
#define DOUBLE_OFF_END_PATH_TIME	0.9990f	// Close to end, but not quite. Double that.
#define REACT_TO_GRENADE_HEIGHT		50.0f	// How low a grenade needs to be for AI to take notice.
#define SMOOTH_BRAKING_FACTOR		0.7f	// Factor to multiply with to ensure monster truck does not "rotate" too much.
#define AIM_DISTANCE()			(((GetVehicleIndex() == 1)? 1.2f : 1.0f) * SCALE_FACTOR * NORMAL_AIM_AHEAD)


namespace GrenadeRun
{



typedef Cure::ContextPath::SplinePath Spline;

struct PathIndexLikeliness
{
	int mPathIndex;
	float mLikeliness;
	float mDistance;
};



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
	mMode = MODE_AT_GOAL;	// Will end up in previous mode.
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
	const float lAimDistance = AIM_DISTANCE();

	float lStrength = Math::Lerp(0.6f, 1.0f, mGame->GetComputerDifficulty());
	const Vector3DF lPosition = mGame->GetCutie()->GetPosition();
	const Vector3DF lVelocity = mGame->GetCutie()->GetVelocity();
	switch (mMode)
	{
		case MODE_FIND_BEST_PATH:
		case MODE_FIND_PATH_OFF_ELEVATOR:
		{
			float lStartTime = 0.5f;
			if (mActivePath != -1)
			{
				// Synchronize all paths.
				Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(mActivePath);
				lStartTime = lPath->GetCurrentInterpolationTime();
				mActivePath = -1;
			}
			mLog.Headlinef(_T("Trying to find new path... starting iterating from  %.2f."), lStartTime);
			Vector3DF lElevatorDirection;
			if (mMode == MODE_FIND_PATH_OFF_ELEVATOR)
			{
				if (lModeRunDeltaFrameCount%5 == 3)
				{
					if (AvoidGrenade(lPosition, lVelocity, 1))
					{
						return;
					}
				}
				mGame->GetCutie()->SetEnginePower(0, 0);
				mGame->GetCutie()->SetEnginePower(2, -lStrength);	// Negative = use full brakes, not only hand brake.
				const Cure::Elevator* lNearestElevator;
				const Vector3DF lElevatorPosition = GetClosestElevatorPosition(lPosition, lNearestElevator);
				if (lElevatorPosition.GetDistanceSquared(lPosition) > ELEVATOR_TOO_CLOSE_DISTANCE*ELEVATOR_TOO_CLOSE_DISTANCE)
				{
					mLog.AHeadline("Fell off elevator while looking for get-off route. Looking for somewhere else to go.");
					SetMode(MODE_FIND_BEST_PATH);
					return;
				}
				lElevatorDirection = lNearestElevator->GetVelocity().GetNormalized(0.5f);
			}
			float lBestPathDistance = 1000000;
			std::vector<PathIndexLikeliness> lRelevantPaths;
			bool lLiftingTowardsGoal = false;
			const int lPathCount = mGame->GetLevel()->QueryPath()->GetPathCount();
			for (int x = 0; x < lPathCount; ++x)
			{
				bool lCurrentLiftingTowardsGoal = false;
				Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(x);
				lPath->GotoAbsoluteTime(lStartTime);
				float lLikeliness = 1;
				const float lNearestDistance = GetClosestPathDistance(lPosition, x, &lLikeliness)/SCALE_FACTOR/2;
				mLog.Infof(_T(" - Path %2i is %2.2f units away."), x, lNearestDistance);
				if (mMode == MODE_FIND_PATH_OFF_ELEVATOR)
				{
					if (lPath->GetCurrentInterpolationTime() > 0.7f)
					{
						// This path is probably the one I used to get ON the elevator (or one
						// just like it from another direction), we're not using that!
						mLog.AInfo("   (Not relevant, too close to path end.)");
						continue;
					}
					else
					{
						const float lTowardsDistance = GetClosestPathDistance(lPosition+lElevatorDirection, x)/SCALE_FACTOR/2;
						if (lTowardsDistance < lNearestDistance)
						{
							lCurrentLiftingTowardsGoal = true;
							if (!lLiftingTowardsGoal)
							{
								lLiftingTowardsGoal = true;
								lBestPathDistance = 1000000;
							}
						}
					}
				}
				if (!lCurrentLiftingTowardsGoal && lLiftingTowardsGoal)
				{
					// This elevator isn't heading in the right direction, but at least one other is.
					continue;
				}
				PathIndexLikeliness pl;
				pl.mPathIndex = x;
				pl.mLikeliness = lLikeliness;
				pl.mDistance = lNearestDistance;
				lRelevantPaths.push_back(pl);
				if (lNearestDistance < lBestPathDistance)
				{
					lBestPathDistance = lNearestDistance;
				}
			}
			// Sort out those that are too far away.
			float lTotalLikeliness = 0;
			std::vector<PathIndexLikeliness>::iterator x;
			for (x = lRelevantPaths.begin(); x != lRelevantPaths.end();)
			{
				if (x->mDistance < lBestPathDistance+2.0f)
				{
					lTotalLikeliness += x->mLikeliness;
					++x;
				}
				else
				{
					x = lRelevantPaths.erase(x);
				}
			}
			if (mMode == MODE_FIND_PATH_OFF_ELEVATOR)
			{
				if (lBestPathDistance > 5 || lRelevantPaths.size() != 1)
				{
					if (lRelevantPaths.size() == 1)
					{
						// Point wheels in the right direction for us to get off safely.
						Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(lRelevantPaths[0].mPathIndex);
						const Vector3DF lDirection = mGame->GetCutie()->GetOrientation() * Vector3DF(0,1,0);
						const Vector3DF lWantedDirection = lPath->GetValue() - lPosition;
						const float lAngle = LEPRA_XY_ANGLE(lWantedDirection, lDirection);
						mGame->GetCutie()->SetEnginePower(1, lAngle*0.5f);
					}
					mLog.Headlinef(_T("On elevator: too long distance to path %.1f, or too many paths %u."), lBestPathDistance, lRelevantPaths.size());
					if (lBestPathDistance > 15)
					{
						const Cure::Elevator* lNearestElevator;
						const Vector3DF lNearestLiftPosition = GetClosestElevatorPosition(lPosition, lNearestElevator);
						if (lNearestLiftPosition.GetDistanceSquared(lPosition) > ELEVATOR_TOO_CLOSE_DISTANCE*ELEVATOR_TOO_CLOSE_DISTANCE)
						{
							// DUCK!!! We fell off!
							mLog.AHeadline("Was on elevator: I'm far from the elevator, so must've fallen off!");
							SetMode(MODE_FIND_BEST_PATH);
						}
					}
					if (lModeRunTime >= 20)
					{
						mLog.AHeadline("On elevator: been here too long, getting off!");
						SetMode(MODE_FIND_BEST_PATH);
					}
					return;
				}
				mLog.Headlinef(_T("Getting off elevator: distance to path %.1f."), lBestPathDistance);
			}
			assert(!lRelevantPaths.empty());
			if (lRelevantPaths.empty())
			{
				return;
			}
			const float lPickedLikeliness = Random::Uniform(0.0f, lTotalLikeliness);
			lTotalLikeliness = 0;
			for (x = lRelevantPaths.begin(); x != lRelevantPaths.end(); ++x)
			{
				const float lNextLikeliness = lTotalLikeliness + x->mLikeliness;
				if (lPickedLikeliness >= lTotalLikeliness && lPickedLikeliness <= lNextLikeliness)
				{
					mActivePath = x->mPathIndex;
					break;
				}
				lTotalLikeliness = lNextLikeliness;
			}
			if (mActivePath < 0)
			{
				mActivePath = lRelevantPaths[Random::GetRandomNumber() % lRelevantPaths.size()].mPathIndex;
			}
			Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(mActivePath);
			const float lWantedDistance = lAimDistance;
			float lStep = lWantedDistance * lPath->GetDistanceNormal();
			if (lStep + lPath->GetCurrentInterpolationTime() > 1)
			{
				lStep = 1 - lPath->GetCurrentInterpolationTime();
			}
			lPath->StepInterpolation(lStep);
			// Fetch ending position.
			const float t = lPath->GetCurrentInterpolationTime();
			lPath->GotoAbsoluteTime(END_PATH_TIME);
			mElevatorGetOnPosition = lPath->GetValue();
			lPath->GotoAbsoluteTime(t);

			mLog.Headlinef(_T("Picked path %i (%i pickable)."), mActivePath, lRelevantPaths.size());
			if (mMode == MODE_FIND_PATH_OFF_ELEVATOR)
			{
				SetMode(MODE_GET_OFF_ELEVATOR);
			}
			else
			{
				SetMode(MODE_HEADING_BACK_ON_TRACK);
			}
		}
		break;
		case MODE_HEADING_BACK_ON_TRACK:
		{
			if (lModeRunDeltaFrameCount%5 == 2)
			{
				const float lVelocityScaleFactor = std::min(1.0f, lVelocity.GetLength() / 2.5f);
				Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(mActivePath);
				const float lCurrentTime = lPath->GetCurrentInterpolationTime();
				const float lNearestPathDistance = GetClosestPathDistance(lPosition, mActivePath, 0, 1);
				if (lNearestPathDistance > 3.0f)
				{
					// First verify that we haven't ended up under path somehow. We do that by checking
					// steepness, since pure Z-distance may be big when going over ditches.
					const Vector3DF lPathPosition = lPath->GetValue();
					const float lSteepness = (lPathPosition.z - lPosition.z) / lNearestPathDistance;
					//mLog.Infof(_T("Checking steepness, nearest path distance is %.3f, steepness is %.3f."), lNearestPathDistance, lSteepness);
					if (lSteepness > 0.6f)
					{
						mLog.Infof(_T("Searching for new, better path, we seem to have ended up under the path. Beneath a bridge perhaps? Nearest path is %.2f, steepness is %.2f."), lNearestPathDistance, lSteepness);
						SetMode(MODE_FIND_BEST_PATH);
						return;
					}
				}
				lPath->GotoAbsoluteTime(lCurrentTime);
				if (lNearestPathDistance < SCALE_FACTOR * OFF_COURSE_DISTANCE * lVelocityScaleFactor)
				{
					// We were able to return to normal, keep on running.
					SetMode(MODE_NORMAL);
					return;
				}
				/*else if (lNearestPathDistance > SCALE_FACTOR * OFF_COURSE_DISTANCE * lVelocityScaleFactor * 5)
				{
					// We're far off, perhaps we fell down from a plateu.
					mActivePath = -1;
					SetMode(MODE_FIND_BEST_PATH);
					return;
				}*/
				else if (lModeRunTime > 7.0f)
				{
					SetMode(MODE_FIND_BEST_PATH);
					return;
				}
			}
		}
		// TRICKY: fall through.
		case MODE_NORMAL:
		case MODE_GET_ON_ELEVATOR:
		case MODE_GET_OFF_ELEVATOR:
		{
			if (mMode == MODE_GET_ON_ELEVATOR)
			{
				if (lModeRunTime > 4.5)
				{
					mLog.Headlinef(_T("Something presumably hinders me getting on the elevator, back square one. (mode run time=%f)"), lModeRunTime);
					SetMode(MODE_FIND_BEST_PATH);
					return;
				}
				const Cure::Elevator* lNearestElevator;
				const Vector3DF lNearestLiftPosition = GetClosestElevatorPosition(mElevatorGetOnPosition, lNearestElevator);
				if (lNearestLiftPosition.z > lPosition.z+0.5f)
				{
					mLog.AHeadline("Couldn't get on in time, going back to waiting.");
					SetMode(MODE_WAITING_FOR_ELEVATOR);
					return;
				}
			}

			if (mMode != MODE_HEADING_BACK_ON_TRACK && mMode != MODE_GET_ON_ELEVATOR && lModeRunDeltaFrameCount%20 == 19)
			{
				const float lDistance = GetClosestPathDistance(lPosition);
				if (lDistance > SCALE_FACTOR * TOTALLY_OFF_COURSE_DISTANCE)
				{
					mLog.AHeadline("Fell off something. Trying some new path.");
					SetMode(MODE_FIND_BEST_PATH);
					return;
				}
				const float lVelocityScaleFactor = ((mMode == MODE_NORMAL)? 1.0f : 3.0f) * Math::Clamp(lVelocity.GetLength() / 2.5f, 0.3f, 1.0f);
				if (lDistance > SCALE_FACTOR * OFF_COURSE_DISTANCE * lVelocityScaleFactor)
				{
					mLog.AHeadline("Going about my way, but got offside somehow. Heading back.");
					SetMode(MODE_HEADING_BACK_ON_TRACK);
					return;
				}
			}

			Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(mActivePath);
			Vector3DF lTarget = lPath->GetValue();

			// Check if vehicle stopped. That would mean either crashed against something or too steep hill.
			if (lModeRunDeltaFrameCount%7 == 4 && mGame->GetCutie()->GetHealth() > 0)
			{
				if (QueryCutieHindered(lTime, lVelocity))
				{
					const Vector3DF lDirection = mGame->GetCutie()->GetOrientation() * Vector3DF(0,1,0);
					const Vector3DF lWantedDirection = lTarget-lPosition;
					const float lForwardAngle = LEPRA_XY_ANGLE(lWantedDirection, lDirection);
					// Amplify angle to be either full left or full right.
					const float lAngle = (lForwardAngle < 0)? -1.0f : 1.0f;
					mGame->GetCutie()->SetEnginePower(1, -lAngle);
					SetMode(MODE_BACKING_UP);
					return;
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

			// Are we heading towards an elevator?
			if (mMode != MODE_GET_ON_ELEVATOR && mMode != MODE_GET_OFF_ELEVATOR && lPath->GetType() == _T("to_elevator"))
			{
				if (lPath->GetDistanceLeft() <= ELEVATOR_WAIT_DISTANCE)
				{
					if (mElevatorGetOnPosition.GetDistanceSquared(lPosition) <= ELEVATOR_WAIT_DISTANCE*ELEVATOR_WAIT_DISTANCE)
					{
						mLog.AHeadline("Normal mode close to end of path to elevator, changing mode.");
						SetMode(MODE_WAITING_FOR_ELEVATOR);
						return;
					}
				}
			}

			// Did we just pass (fly by?) the goal?
			if (lModeRunDeltaFrameCount%3 == 0)
			{
				const Vector3DF lGoalDirection = mGame->GetCtf()->GetPosition() - lPosition;
				if (::fabs(lGoalDirection.z) < 2 &&
					lGoalDirection.GetLengthSquared() < ELEVATOR_FAR_DISTANCE*ELEVATOR_FAR_DISTANCE &&
					lVelocity.GetLengthSquared() < 6*6)
				{
					const Vector3DF lCutieDirection = mGame->GetCutie()->GetOrientation() * Vector3DF(0,1,0);
					const float lDeltaAngle = ::fabs(LEPRA_XY_ANGLE(lGoalDirection, lCutieDirection));
					if (lDeltaAngle >= PIF-PIF/4 && lDeltaAngle <= PIF+PIF/4)
					{
						mLog.AHeadline("Passed goal, it's right behind me!");
						SetMode(MODE_BACKING_UP_TO_GOAL);
						return;
					}
				}
			}

			// Step target (aim) ahead.
			{
				const float lActualDistance2 = lTarget.GetDistanceSquared(lPosition);
				const float lMaxAimFactor = (mMode == MODE_GET_OFF_ELEVATOR)? 1.0f : 1.5f;
				const float lWantedDistance = lAimDistance * Math::Clamp(lVelocity.GetLength() / 2.5f, 0.5f, lMaxAimFactor);
				if (lActualDistance2 < lWantedDistance*lWantedDistance)
				{
					const float lMoveAhead = lWantedDistance*1.1f - ::sqrt(lActualDistance2);
					lPath->StepInterpolation(lMoveAhead * lPath->GetDistanceNormal());
					log_volatile(mLog.Debugf(_T("Stepping %f (=%f m) from %f."), lMoveAhead*lPath->GetDistanceNormal(), lMoveAhead, lPath->GetCurrentInterpolationTime()));
				}

				// Check if we're there yet.
				const float t = lPath->GetCurrentInterpolationTime();
				lPath->GotoAbsoluteTime(1.0f);
				const float lTargetDistance = (mMode == MODE_GET_ON_ELEVATOR)? ON_ELEVATOR_DISTANCE : ON_GOAL_DISTANCE + mGame->GetCutie()->GetForwardSpeed()/4;
				if (IsCloseToTarget(lPosition, lTargetDistance))
				{
					const bool lTowardsElevator = (lPath->GetType() == _T("to_elevator"));
					if (lTowardsElevator)
					{
						if (mMode == MODE_GET_ON_ELEVATOR)
						{
							SetMode(MODE_ON_ELEVATOR);
							return;
						}
						else if (mMode != MODE_GET_OFF_ELEVATOR)
						{
							// We got off track somewhere, try to shape up!
							mLog.AHeadline("Normal mode target wrapped on our way to an elevator, changing mode.");
							SetMode(MODE_WAITING_FOR_ELEVATOR);
							return;
						}
					}
					else
					{
						SetMode(MODE_STOPPING_AT_GOAL);
						return;
					}
				}
				lPath->GotoAbsoluteTime(t);

				lTarget = lPath->GetValue();
			}

			const float lGetOffDelayTime = (GetVehicleIndex() == 3)? mGame->GetComputerDifficulty() / 3.0f - 0.15f: 0.4f;
			if (!(mMode == MODE_GET_OFF_ELEVATOR && lModeRunTime < lGetOffDelayTime))
			{
				// Move forward.
				mGame->GetCutie()->SetEnginePower(0, +lStrength);
				mGame->GetCutie()->SetEnginePower(2, 0);
			}

			// Steer.
			const Vector3DF lDirection = mGame->GetCutie()->GetOrientation() * Vector3DF(0,1,0);
			const Vector3DF lWantedDirection = lTarget-lPosition;
			float lAngle = LEPRA_XY_ANGLE(lWantedDirection, lDirection);
			if (mMode == MODE_GET_OFF_ELEVATOR)
			{
				// Aborting too early might cause us to stop, waiting for the next ride in mid-air.
				const float lGetOffDistance = GetClosestElevatorRadius() + ELEVATOR_GOT_OFF_EXTRA_DISTANCE;
				Vector2DF lElevatorGetOff2d(mElevatorGetOffPosition.x, mElevatorGetOffPosition.y);
				Vector2DF lPosition2d(lPosition.x, lPosition.y);
				mLog.Infof(_T("ElevatorGetOff (%f;%f), pos (%f;%f)"), lElevatorGetOff2d.x, lElevatorGetOff2d.y, lPosition2d.x, lPosition2d.y);
				if (lElevatorGetOff2d.GetDistanceSquared(lPosition2d) > lGetOffDistance*lGetOffDistance)
				{
					SetMode(MODE_NORMAL);
				}
				lAngle *= 2;	// Make steering more powerful while getting off.
			}
			if (GetVehicleIndex() == 3)
			{
				lAngle *= 2;
			}
			mGame->GetCutie()->SetEnginePower(1, +lAngle);
			mLastAverageAngle = Math::Lerp(mLastAverageAngle, lAngle, 0.5f);

			// Check if we need to slow down.
			const float lHighSpeed = SCALE_FACTOR * 2.7f;
			const float lAbsAngle = ::fabs(lAngle);
			if (lVelocity.GetLengthSquared() > lHighSpeed*lHighSpeed)
			{
				if (lAbsAngle > 0.2f)
				{
					float lFactor = 0;
					switch (GetVehicleIndex())
					{
						case 0: lFactor = 0.02f;	break;
						case 1:	lFactor = 0.10f;	break;
						case 2:	lFactor = 0.02f;	break;
					}
					mGame->GetCutie()->SetEnginePower(2, lAbsAngle*lFactor + lVelocity.GetLength()*lFactor*0.1f);
				}
				else if (lPath->GetCurrentInterpolationTime() >= DOUBLE_OFF_END_PATH_TIME &&
					IsCloseToTarget(lPosition, SLOW_DOWN_DISTANCE))
				{
					mGame->GetCutie()->SetEnginePower(2, 0.2f);
				}
			}
		}
		break;
		case MODE_BACKING_UP:
		{
			// Brake or move backward.
			const bool lIsMovingForward = (mGame->GetCutie()->GetForwardSpeed() > 0.1f*SCALE_FACTOR);
			mGame->GetCutie()->SetEnginePower(0, lIsMovingForward? 0.0f : -lStrength);
			mGame->GetCutie()->SetEnginePower(2, lIsMovingForward? lStrength :  0.0f);

			const float lBackTime = (GetVehicleIndex() == 3)? 2.5f : 1.7f;
			if (!lIsMovingForward && lModeRunTime > lBackTime)
			{
				SetMode(MODE_HEADING_BACK_ON_TRACK);
				return;
			}
		}
		break;
		case MODE_BACKING_UP_TO_GOAL:
		{
			Vector3DF lWantedDirection = mGame->GetCtf()->GetPosition() - lPosition;
			const float lDistance2 = lWantedDirection.GetLengthSquared();
			if (lDistance2 <= ON_GOAL_DISTANCE*ON_GOAL_DISTANCE)
			{
				Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(mActivePath);
				lPath->GotoAbsoluteTime(END_PATH_TIME);
				SetMode(MODE_STOPPING_AT_GOAL);
				return;
			}
			else if (lDistance2 >= TOTALLY_OFF_COURSE_DISTANCE*TOTALLY_OFF_COURSE_DISTANCE)
			{
				SetMode(MODE_FIND_BEST_PATH);
				return;
			}

			// Brake or move backward.
			const bool lIsMovingForward = (mGame->GetCutie()->GetForwardSpeed() > 0.1f*SCALE_FACTOR);
			mGame->GetCutie()->SetEnginePower(0, lIsMovingForward? 0.0f : -lStrength);
			mGame->GetCutie()->SetEnginePower(2, lIsMovingForward? lStrength :  0.0f);

			// Turn steering wheel.
			const Vector3DF lDirection = mGame->GetCutie()->GetOrientation() * Vector3DF(0,1,0);
			float lAngle = LEPRA_XY_ANGLE(lWantedDirection, lDirection);
			lAngle += (lAngle < 0)? +PIF : -PIF;
			lAngle *= 3;
			mGame->GetCutie()->SetEnginePower(1, -lAngle);

			if (lModeRunTime > 15)
			{
				mLog.AHeadline("Not getting back to goal. Fuck it.");
				SetMode(MODE_ROTATE_ON_THE_SPOT);
				return;
			}
		}
		break;
		case MODE_FLEE:
		{
			// Pedal to the metal.
			mGame->GetCutie()->SetEnginePower(0, +lStrength);
			mGame->GetCutie()->SetEnginePower(1, 0);
			mGame->GetCutie()->SetEnginePower(2, 0);
			if (lModeRunTime > 3.0f)
			{
				SetMode(MODE_FIND_BEST_PATH);
				return;
			}
		}
		break;
		case MODE_STOPPING_AT_GOAL:
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
			if (!IsCloseToTarget(lPosition, ON_GOAL_DISTANCE))
			{
				// If either already stopped at goal, OR stopped but at the wrong spot.
				if (mMode != MODE_STOPPING_AT_GOAL || mGame->GetCutie()->GetForwardSpeed() < 0.5f*SCALE_FACTOR)
				{
					lPath->GotoAbsoluteTime(DOUBLE_OFF_END_PATH_TIME);	// Close to end, but not at end.
					SetMode(MODE_HEADING_BACK_ON_TRACK);
					return;
				}
			}
			if (mMode != MODE_AT_GOAL)
			{
				SetMode(MODE_AT_GOAL);
			}
			// Brake!
			mGame->GetCutie()->SetEnginePower(0, 0);
			mGame->GetCutie()->SetEnginePower(2, -lStrength);	// Negative = use full brakes, not only hand brake.
		}
		break;
		case MODE_WAITING_FOR_ELEVATOR:
		{
			if (lModeRunDeltaFrameCount%5 == 3)
			{
				if (AvoidGrenade(lPosition, lVelocity, 1))
				{
					return;
				}
			}
			if (lModeRunTime > 25.0f)
			{
				mLog.AHeadline("Movin' on, I've waited for the elevator too long.");
				SetMode(MODE_FLEE);
				return;
			}
			if (::fabs(mLastAverageAngle) > 0.1f && GetVehicleIndex() == 1)
			{
				lStrength *= SMOOTH_BRAKING_FACTOR;	// Smooth braking when turning, we can always back up if necessary.
			}
			const float lElevatorDistance2 = mElevatorGetOnPosition.GetDistanceSquared(lPosition);
			if (lElevatorDistance2 < ELEVATOR_TOO_CLOSE_DISTANCE*ELEVATOR_TOO_CLOSE_DISTANCE)
			{
				mLog.AHeadline("Got too close to the elevator stop position, backing up.");
				// Back up parallel to the spline direction.
				const Vector3DF lDirection = mGame->GetCutie()->GetOrientation() * Vector3DF(0,1,0);
				Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(mActivePath);
				const Vector3DF lWantedDirection = lPath->GetSlope();
				const float lAngle = LEPRA_XY_ANGLE(lWantedDirection, lDirection);
				mGame->GetCutie()->SetEnginePower(1, +lAngle);
				const bool lIsMovingForward = (mGame->GetCutie()->GetForwardSpeed() > 0.1f*SCALE_FACTOR);
				mGame->GetCutie()->SetEnginePower(0, lIsMovingForward? 0.0f : -lStrength);
				mGame->GetCutie()->SetEnginePower(2, lIsMovingForward? lStrength :  0.0f);

				const Cure::Elevator* lNearestElevator;
				Vector3DF lNearestLiftPosition2d;
				float lElevatorXyDistance2ToElevatorStop;
				const bool lIsElevatorHere = HasElevatorArrived(lNearestElevator, lPosition.z, lNearestLiftPosition2d, lElevatorXyDistance2ToElevatorStop);
				if (lIsElevatorHere)
				{
					SetMode(MODE_GET_ON_ELEVATOR);
				}
				else if (QueryCutieHindered(lTime, lVelocity))
				{
					mLastAverageAngle = (Random::Uniform(0.0f, 1.0f) > 0.5f)? +2.0f : -2.0f;
					SetMode(MODE_ROTATE_ON_THE_SPOT);
				}
				return;
			}
			if (::fabs(mElevatorGetOnPosition.z-lPosition.z) >= 3 ||
				lElevatorDistance2 > ELEVATOR_FAR_DISTANCE*ELEVATOR_FAR_DISTANCE)
			{
				mLog.AHeadline("Somehow got away from the elevator wait position, doing something else.");
				SetMode(MODE_FIND_BEST_PATH);
				return;
			}

			// Check that we're headed towards the elevator center.
			if (lVelocity.GetLengthSquared() < 0.5f)
			{
				Vector3DF lUp(0, 0, 1);
				lUp = mGame->GetCutie()->GetOrientation() * lUp;
				if (lUp.z > 0.7f)
				{
					const Vector3DF lDirection = mGame->GetCutie()->GetOrientation() * Vector3DF(0,1,0);
					const Vector3DF lWantedDirection = mElevatorGetOnPosition - lPosition;
					const float lAngle = LEPRA_XY_ANGLE(lWantedDirection, lDirection);
					if (::fabs(lAngle) > PIF/12)
					{
						mRotateAngle = -lAngle;
						SetMode(MODE_ROTATE_ON_THE_SPOT_WAITING);
						return;
					}
				}
			}

			const Cure::Elevator* lNearestElevator;
			Vector3DF lNearestLiftPosition2d;
			float lElevatorXyDistance2ToElevatorStop;
			if (HasElevatorArrived(lNearestElevator, lPosition.z, lNearestLiftPosition2d, lElevatorXyDistance2ToElevatorStop))
			{
				Vector3DF lVelocityXY = lNearestElevator->GetVelocity();
				bool lTryGetOn = false;
				// Check if elevator is on it's way out.
				if (IsVertical(lVelocityXY))
				{
					lTryGetOn = true;
				}
				else
				{
					lVelocityXY.x *= 0.1f;
					lVelocityXY.y *= 0.1f;
					lVelocityXY.z  = 0;
					if (lElevatorXyDistance2ToElevatorStop+0.1f >= mElevatorGetOnPosition.GetDistanceSquared(lNearestLiftPosition2d+lVelocityXY))
					{
						lTryGetOn = true;
					}
				}
				if (lTryGetOn)
				{
					mLog.AInfo("Elevator here - getting on!");
					SetMode(MODE_GET_ON_ELEVATOR);
					return;
				}
				else
				{
					mLog.AInfo("Waiting for elevator: not getting on, since elevator is departing!");
				}
			}

			mGame->GetCutie()->SetEnginePower(1, 0);
			// Brake!
			mGame->GetCutie()->SetEnginePower(0, 0);
			mGame->GetCutie()->SetEnginePower(2, -lStrength);	// Negative = use full brakes, not only hand brake.
		}
		break;
		case MODE_ON_ELEVATOR:
		{
			lStrength *= SMOOTH_BRAKING_FACTOR;	// Smooth braking, we can always back up if necessary.

			// Brake!
			mGame->GetCutie()->SetEnginePower(0, 0);
			mGame->GetCutie()->SetEnginePower(1, 0);
			mGame->GetCutie()->SetEnginePower(2, -lStrength);	// Negative = use full brakes, not only hand brake.

			// Check if elevator departed.
			const float lMinimumVelocity2 = 0.5f*0.5f;
			if (lModeRunTime > 0.7f && lVelocity.GetLengthSquared() > lMinimumVelocity2)
			{
				const Cure::Elevator* lNearestElevator;
				const Vector3DF lNearestLiftPosition = GetClosestElevatorPosition(mElevatorGetOnPosition, lNearestElevator);
				if (lNearestLiftPosition.z > lPosition.z+0.2f)
				{
					// Crap, we missed it!
					mLog.AHeadline("Must have missed the elevator (it's not close!), waiting for it again!");
					SetMode(MODE_WAITING_FOR_ELEVATOR);
					return;
				}
				// Vehicle speed check not enouch (bouncy wheels), so check elevator speed too.
				Vector3DF lElevatorVelocity = lNearestElevator->GetVelocity();
				if (lElevatorVelocity.GetLengthSquared() > lMinimumVelocity2)
				{
					const bool lIsHorizontal = !IsVertical(lElevatorVelocity);
					const Vector3DF lDirection = lIsHorizontal? lElevatorVelocity : mGame->GetCutie()->GetOrientation() * Vector3DF(0,1,0);
					mRotateAngle = -GetRelativeDriveOnAngle(lDirection);
					if (::fabs(mRotateAngle) > PIF/6 || lIsHorizontal)
					{
						if (lIsHorizontal)
						{
							mRotateAngle = (mRotateAngle < 0)? -1.3f : +1.3f;
						}
						SetMode(MODE_ROTATE_ON_THE_SPOT_DURING);
						return;
					}
					SetMode(MODE_FIND_PATH_OFF_ELEVATOR);
					return;
				}
			}
			else if (lModeRunTime > 4.5f)
			{
				// Crap, we missed it!
				mLog.AHeadline("Must have missed the elevator (I'm still here!), waiting for it again!");
				SetMode(MODE_WAITING_FOR_ELEVATOR);
				return;
			}

			if (lModeRunTime > 0.8f)
			{
				// Check if we should adjust pos.
				const Vector3DF lForward = mGame->GetCutie()->GetOrientation() * Vector3DF(0,1,0);
				const float lDist = mElevatorGetOnPosition.GetDistanceSquared(lPosition);
				if (lDist > mElevatorGetOnPosition.GetDistanceSquared(lPosition+lForward))
				{
					mGame->GetCutie()->SetEnginePower(0, +lStrength);
					mGame->GetCutie()->SetEnginePower(2, 0);
				}
				else if (lDist > mElevatorGetOnPosition.GetDistanceSquared(lPosition-lForward))
				{
					mGame->GetCutie()->SetEnginePower(0, -lStrength);
					mGame->GetCutie()->SetEnginePower(2, 0);
				}
			}
		}
		break;
		case MODE_ROTATE_ON_THE_SPOT:
		case MODE_ROTATE_ON_THE_SPOT_DURING:
		case MODE_ROTATE_ON_THE_SPOT_WAITING:
		{
			float lAngle = mRotateAngle;
			const float lMinAngle = 0.3f;
			if (::fabs(lAngle) < lMinAngle)
			{
				lAngle = (lAngle < 0)? -lMinAngle : +lMinAngle;
			}
			float lSteerEndTime = 0.4f;
			float lForwardEndTime = lSteerEndTime + 0.9f;
			float lOtherSteerEndTime = lForwardEndTime + lSteerEndTime;
			float lPeriod = lOtherSteerEndTime + 0.8f;
			switch (GetVehicleIndex())
			{
				case 0:
				{
					// Cutie less forward acceleration, because it has good grip and accelerates quite well.
					lAngle *= 1.2f;
					lForwardEndTime = lSteerEndTime + 0.7f;
					lOtherSteerEndTime = lForwardEndTime + lSteerEndTime;
					lPeriod = lOtherSteerEndTime + 0.8f;
				}
				break;
				case 1:
				{
					// Hardie's steering impared.
					lAngle *= 2;
					lSteerEndTime = 0.7f;
					lForwardEndTime = lSteerEndTime + 0.7f;
					lOtherSteerEndTime = lForwardEndTime + lSteerEndTime;
					lPeriod = lOtherSteerEndTime + 1.0f;
					lStrength *= SMOOTH_BRAKING_FACTOR;
				}
				break;
				case 2:
				{
					// Speedie turns really effectively.
					lAngle *= 0.5f;
				}
				break;
				case 3:
				{
					// Sleepie needs longer running time, since so slow.
					lAngle *= 2;
					lForwardEndTime = lSteerEndTime + 1.0f;
					lOtherSteerEndTime = lForwardEndTime + lSteerEndTime;
					lPeriod = lOtherSteerEndTime + 1.5f;
				}
				break;
			}
			if (mMode == MODE_ROTATE_ON_THE_SPOT_WAITING)
			{
				const Cure::Elevator* lNearestElevator;
				Vector3DF lNearestLiftPosition2d;
				float lElevatorXyDistance2ToElevatorStop;
				if (HasElevatorArrived(lNearestElevator, lPosition.z, lNearestLiftPosition2d, lElevatorXyDistance2ToElevatorStop))
				{
					mLog.AHeadline("Elevator arrived while rotating on the spot, getting on instead!");
					SetMode(MODE_GET_ON_ELEVATOR);
					return;
				}
			}
			// Finish this rotation show if we're getting there.
			const int lIterations = (mMode == MODE_ROTATE_ON_THE_SPOT_WAITING)? 1 : 2;
			if (lModeRunTime > lIterations*lPeriod+lSteerEndTime)
			{
				mGame->GetCutie()->SetEnginePower(0, 0);
				mGame->GetCutie()->SetEnginePower(1, -lAngle);
				mGame->GetCutie()->SetEnginePower(2, -1);
				if (mMode == MODE_ROTATE_ON_THE_SPOT)
				{
					SetMode(MODE_HEADING_BACK_ON_TRACK);
				}
				else if (mMode == MODE_ROTATE_ON_THE_SPOT_DURING)
				{
					SetMode(MODE_FIND_PATH_OFF_ELEVATOR);
				}
				else
				{
					SetMode(MODE_WAITING_FOR_ELEVATOR);
				}
				return;
			}
			for (int x = 0; x < lIterations+1; ++x)
			{
				const float lBase = x*lPeriod;
				if (lModeRunTime >= lBase && lModeRunTime < lBase+lSteerEndTime)
				{
					// Brake and turn in "forward direction".
					mGame->GetCutie()->SetEnginePower(0, 0);
					mGame->GetCutie()->SetEnginePower(1, -lAngle);
					mGame->GetCutie()->SetEnginePower(2, -lStrength);
					break;
				}
				else if (lModeRunTime >= lBase+lSteerEndTime && lModeRunTime < lBase+lForwardEndTime)
				{
					// Drive forward.
					mGame->GetCutie()->SetEnginePower(0, +lStrength);
					mGame->GetCutie()->SetEnginePower(2, 0);
					break;
				}
				else if (lModeRunTime >= lBase+lForwardEndTime && lModeRunTime < lBase+lOtherSteerEndTime)
				{
					// Brake and turn in "backward direction".
					mGame->GetCutie()->SetEnginePower(0, 0);
					mGame->GetCutie()->SetEnginePower(1, +lAngle);
					mGame->GetCutie()->SetEnginePower(2, -lStrength);
					break;
				}
				else if (lModeRunTime >= lBase+lOtherSteerEndTime && lModeRunTime < lBase+lPeriod)
				{
					// Drive backward.
					mGame->GetCutie()->SetEnginePower(0, -0.7f*lStrength);
					mGame->GetCutie()->SetEnginePower(2, 0);
					break;
				}
			}
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
			h < 4.0f * SCALE_FACTOR ||	// Still gonna blow up in our face, so ignore it.
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
			mGame->GetCutie()->SetEnginePower(1, 0);	// Backup straight.
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
	if (pMode == mPreviousMode && pMode != MODE_NORMAL)
	{
		++mStuckCount;
	}
	else
	{
		mStuckCount = 0;
	}
	if (mStuckCount >= 2)
	{
		Vector3DF lVelocity = mGame->GetCutie()->GetVelocity();
		lVelocity.z = 0;
		if (lVelocity.GetLengthSquared() < 2*2)
		{
			mLog.AHeadline("Stuck in a vehicle AI loop, trying to break out!");
			if (pMode != MODE_FIND_BEST_PATH && mMode != MODE_FIND_BEST_PATH)
			{
				pMode = MODE_FIND_BEST_PATH;
			}
			else
			{
				mRotateAngle = (Random::Uniform(0.0f, 1.0f) > 0.5f)? +2.0f : -2.0f;
				pMode = MODE_ROTATE_ON_THE_SPOT;
			}
		}
		mStuckCount = 0;
	}
	else if (pMode == MODE_FIND_PATH_OFF_ELEVATOR)
	{
		mActivePath = -1;	// Treat all paths as equals.
	}
	else if (pMode == MODE_HEADING_BACK_ON_TRACK || pMode == MODE_NORMAL)
	{
		mLastAverageAngle = 0;
	}
	else if (pMode == MODE_GET_OFF_ELEVATOR)
	{
		mElevatorGetOffPosition = mGame->GetCutie()->GetPosition();
	}
	mStoppedFrame = -1;
	mPreviousMode = mMode;
	mMode = pMode;
	mModeStartFrame = GetManager()->GetGameManager()->GetTimeManager()->GetCurrentPhysicsFrame();
	const tchar* lModeName = _T("???");
	switch (mMode)
	{
		case MODE_FIND_BEST_PATH:		lModeName = _T("FIND BEST PATH");		break;
		case MODE_FIND_PATH_OFF_ELEVATOR:	lModeName = _T("FIND PATH OFF ELEVATOR");	break;
		case MODE_NORMAL:			lModeName = _T("NORMAL");			break;
		case MODE_HEADING_BACK_ON_TRACK:	lModeName = _T("HEADING BACK ON TRACK");	break;
		case MODE_BACKING_UP:			lModeName = _T("BACKING UP");			break;
		case MODE_BACKING_UP_TO_GOAL:		lModeName = _T("BACKING UP TO GOAL");		break;
		case MODE_FLEE:				lModeName = _T("FLEE");				break;
		case MODE_STOPPING_AT_GOAL:		lModeName = _T("STOPPING AT GOAL");		break;
		case MODE_AT_GOAL:			lModeName = _T("AT GOAL");			break;
		case MODE_WAITING_FOR_ELEVATOR:		lModeName = _T("WAITING FOR ELEVATOR");		break;
		case MODE_GET_ON_ELEVATOR:		lModeName = _T("GET ON ELEVATOR");		break;
		case MODE_GET_OFF_ELEVATOR:		lModeName = _T("GET OFF ELEVATOR");		break;
		case MODE_ON_ELEVATOR:			lModeName = _T("ON ELEVATOR");			break;
		case MODE_ROTATE_ON_THE_SPOT:		lModeName = _T("ROTATE ON THE SPOT");		break;
		case MODE_ROTATE_ON_THE_SPOT_DURING:	lModeName = _T("ROTATE ON THE SPOT DURING");	break;
		case MODE_ROTATE_ON_THE_SPOT_WAITING:	lModeName = _T("ROTATE ON THE SPOT WAITING");	break;
	}
	mLog.Headlinef(_T("Switching mode to %s."), lModeName);
}

bool VehicleAi::IsCloseToTarget(const Vector3DF& pPosition, float pDistance) const
{
	Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(mActivePath);
	const Vector3DF lTarget = lPath->GetValue();
	const float lTargetDistance2 = lTarget.GetDistanceSquared(pPosition);
	//mLog.Headlinef(_T("IsCloseToTarget ^2: %f."), lTargetDistance2);
	const float lGoalDistance = pDistance*SCALE_FACTOR;
	return (lTargetDistance2 <= lGoalDistance*lGoalDistance);
}

float VehicleAi::GetClosestPathDistance(const Vector3DF& pPosition, const int pPath, float* pLikeliness, float pSteepFactor) const
{
	Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath((pPath >= 0)? pPath : mActivePath);
	if (pLikeliness)
	{
		*pLikeliness = lPath->GetLikeliness();
	}
	const float lCurrentTime = lPath->GetCurrentInterpolationTime();

	if (pPath < 0)
	{
		// We can assume the path "current" pointer is a bit ahead, so step back some to get a closer
		// approximation of where to start looking for our closest point on the spline.
		const float lWantedDistance = AIM_DISTANCE();
		float lDeltaTime = -lWantedDistance * lPath->GetDistanceNormal();
		if (lCurrentTime+lDeltaTime < 0)
		{
			lDeltaTime = -lCurrentTime;
		}
		lPath->StepInterpolation(lDeltaTime);
	}
	else
	{
		// Coarse check first to find a reasonable sample in the whole path.
		int lBestSample = -1;
		float lBestDistance2 = 1e8f;
		const int lSteps = 5;
		for (int x = 0; x < 5; ++x)
		{
			const float lSampleTime = 1.0f/(lSteps+1) * (x+1);
			lPath->GotoAbsoluteTime(lSampleTime);
			const float lDistance2 = lPath->GetValue().GetDistanceSquared(pPosition);
			if (lDistance2 < lBestDistance2)
			{
				lBestSample = x;
				lBestDistance2 = lDistance2;
			}
		}
		const float lBestTime = 1.0f/(lSteps+1) * (lBestSample+1);
		lPath->GotoAbsoluteTime(lBestTime);
	}

	float lNearestDistance;
	Vector3DF lClosestPoint;
	const float lSearchStepLength = (pPath >= 0)? -0.1f : 0.0125f;
	const int lSearchSteps = (pPath >= 0)? 10 : 3;
	lPath->FindNearestTime(lSearchStepLength, pPosition, lNearestDistance, lClosestPoint, lSearchSteps);
	// Steep check.
	if (lNearestDistance < ::fabs(pPosition.z - lClosestPoint.z)*3)
	{
		lNearestDistance *= pSteepFactor;
	}

	if (pPath < 0)
	{
		// Step back to target point.
		lPath->GotoAbsoluteTime(lCurrentTime);
	}

	return lNearestDistance;
}

Vector3DF VehicleAi::GetClosestElevatorPosition(const Vector3DF& pPosition, const Cure::Elevator*& pNearestElevator) const
{
	pNearestElevator = 0;
	typedef Cure::ContextManager::ContextObjectTable ContextTable;
	const ContextTable& lObjectTable = GetManager()->GetObjectTable();
	ContextTable::const_iterator x = lObjectTable.begin();
	const str lElevatorClassId = _T("Elevator");
	float lDistance2 = -1;
	Vector3DF lNearestPosition;
	for (; x != lObjectTable.end(); ++x)
	{
		const Cure::ContextObject* lObject = x->second;
		if (lObject->GetClassId() != lElevatorClassId)
		{
			continue;
		}
		const Cure::Elevator* lElevator = (const Cure::Elevator*)lObject;
		const Vector3DF lElevatorPosition = lElevator->GetPosition();
		const float lThisDistance2 = lElevatorPosition.GetDistanceSquared(pPosition);
		if (lDistance2 < 0)
		{
			pNearestElevator = lElevator;
			lDistance2 = lThisDistance2;
			lNearestPosition = lElevatorPosition;
		}
		else if (lThisDistance2 < lDistance2)
		{
			pNearestElevator = lElevator;
			lDistance2 = lThisDistance2;
			lNearestPosition = lElevatorPosition;
		}
	}
	return lNearestPosition;
}

bool VehicleAi::HasElevatorArrived(const Cure::Elevator*& pNearestElevator, const float pPositionZ, Vector3DF& pNearestLiftPosition2d, float& pElevatorXyDistance2ToElevatorStop)
{
	const Vector3DF lNearestLiftPosition = GetClosestElevatorPosition(mElevatorGetOnPosition, pNearestElevator);
	pNearestLiftPosition2d = lNearestLiftPosition;
	pNearestLiftPosition2d.z = mElevatorGetOnPosition.z;
	pElevatorXyDistance2ToElevatorStop = mElevatorGetOnPosition.GetDistanceSquared(pNearestLiftPosition2d);
	return (lNearestLiftPosition.z < pPositionZ+0.5f && pElevatorXyDistance2ToElevatorStop < 2*2);
}

float VehicleAi::GetClosestElevatorRadius() const
{
	const Cure::Elevator* lNearestElevator;
	GetClosestElevatorPosition(mElevatorGetOnPosition, lNearestElevator);
	return lNearestElevator->GetRadius();
}

bool VehicleAi::IsVertical(const Vector3DF& pVector)
{
	return (::fabs(pVector.z) > 2 * (::fabs(pVector.x) + ::fabs(pVector.y)));
}

int VehicleAi::GetVehicleIndex() const
{
	const str& lClassId = mGame->GetCutie()->GetClassId();
	if (lClassId == _T("cutie"))
	{
		return 0;
	}
	if (lClassId == _T("monster"))
	{
		return 1;
	}
	if (lClassId == _T("corvette"))
	{
		return 2;
	}
	if (lClassId == _T("road_roller"))
	{
		return 3;
	}
	assert(false);
	return -1;
}

float VehicleAi::GetRelativeDriveOnAngle(const Vector3DF& pDirection) const
{
	assert(mActivePath >= 0);
	if (mActivePath < 0)
	{
		return 0;
	}
	Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(mActivePath);
	lPath->GotoAbsoluteTime(0.95f);
	const Vector3DF p1 = lPath->GetValue();
	lPath->StepInterpolation(0.04f);
	const Vector3DF p2 = lPath->GetValue();
	const Vector3DF lWantedDirection = p2-p1;
	const float lAngle = LEPRA_XY_ANGLE(lWantedDirection, pDirection);
	return lAngle;
}

bool VehicleAi::QueryCutieHindered(const Cure::TimeManager* pTime, const Vector3DF& pVelocity)
{
	const float lSlowSpeed = 0.35f * SCALE_FACTOR;
	if (pVelocity.GetLengthSquared() < lSlowSpeed*lSlowSpeed)
	{
		if (mStoppedFrame == -1)
		{
			mStoppedFrame = pTime->GetCurrentPhysicsFrame();
		}
		const int lStoppedDeltaFrameCount = pTime->GetCurrentPhysicsFrameDelta(mStoppedFrame);
		const float lStoppedTime = pTime->ConvertPhysicsFramesToSeconds(lStoppedDeltaFrameCount);
		if (lStoppedTime >= 1.0f)
		{
			return true;
		}
	}
	else
	{
		mStoppedFrame = -1;
	}
	return false;
}




LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, VehicleAi);



}
