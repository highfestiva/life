
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



#define	NORMAL_AIM_AHEAD		13.0f	// How far ahead to try and intersect the path.
#define ON_GOAL_DISTANCE		4.2f	// When at goal.
#define ELEVATOR_WAIT_DISTANCE		11.4f	// When close to elevator.
#define ELEVATOR_TOO_CLOSE_DISTANCE	7.5f	// When too close to elevator.
#define ON_ELEVATOR_DISTANCE		3.2f	// When on elevator.
#define SLOW_DOWN_DISTANCE		15.0f	// When to slow down before stopping at goal.
#define OFF_COURSE_DISTANCE		4.5f	// When to start heading back.
#define END_PATH_TIME			0.9999f	// Time is considered at end of path.
#define	OFF_END_PATH_TIME		0.9995f	// Close to end, but not quite.
#define DOUBLE_OFF_END_PATH_TIME	0.9990f	// Close to end, but not quite. Double that.
#define REACT_TO_GRENADE_HEIGHT		50.0f	// How low a grenade needs to be for AI to take notice.
#define SMOOTH_BRAKING_FACTOR		0.5f	// Factor to multiply with to ensure monster truck does not "rotate" too much.


namespace GrenadeRun
{



typedef Cure::ContextPath::SplinePath Spline;

struct PathIndexLikeliness
{
	int mPathIndex;
	float mLikeliness;
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

	float lStrength = Math::Lerp(0.6f, 1.0f, mGame->GetComputerDifficulty());
	const Vector3DF lPosition = mGame->GetCutie()->GetPosition();
	const Vector3DF lVelocity = mGame->GetCutie()->GetVelocity();
	switch (mMode)
	{
		case MODE_FIND_BEST_PATH:
		case MODE_FIND_PATH_OFF_ELEVATOR:
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
			std::vector<PathIndexLikeliness> lRelevantPaths;
			float lTotalLikeliness = 0;
			const int lPathCount = mGame->GetLevel()->QueryPath()->GetPathCount();
			for (int x = 0; x < lPathCount; ++x)
			{
				Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(x);
				lPath->GotoAbsoluteTime(lStartTime);
				float lLikeliness = 1;
				const int lRoundedNearestDistance = (int)(GetClosestPathDistance(lPosition, x, &lLikeliness)/SCALE_FACTOR/2);
				if (mMode == MODE_FIND_PATH_OFF_ELEVATOR && lPath->GetCurrentInterpolationTime() > 0.5f)
				{
					// This path is probably the one I used to get ON the elevator, we're not using that!
					continue;
				}
				PathIndexLikeliness pl;
				pl.mPathIndex = x;
				pl.mLikeliness = lLikeliness;
				if (lRoundedNearestDistance < lBestPathDistance)
				{
					lRelevantPaths.clear();
					lRelevantPaths.push_back(pl);
					lBestPathDistance = lRoundedNearestDistance;
					lTotalLikeliness += lLikeliness;
				}
				else if (lRoundedNearestDistance == lBestPathDistance)
				{
					lRelevantPaths.push_back(pl);
					lTotalLikeliness += lLikeliness;
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
						const float lAngle = Vector2DF(lWantedDirection.x, lWantedDirection.y).GetAngle(Vector2DF(lDirection.x, lDirection.y));
						mGame->GetCutie()->SetEnginePower(1, lAngle*0.5f, 0);
					}
					mLog.Headlinef(_T("On elevator: too long distance to path %i, or too many paths %u."), lBestPathDistance, lRelevantPaths.size());
					if (lBestPathDistance > 15)
					{
						const Cure::Elevator* lNearestElevator;
						const Vector3DF lNearestLiftPosition = GetClosestElevatorPosition(mElevatorStopPosition, lNearestElevator);
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
				mLog.Headlinef(_T("Getting off elevator: distance to path %i."), lBestPathDistance);
			}
			assert(!lRelevantPaths.empty());
			if (lRelevantPaths.empty())
			{
				return;
			}
			const float lPickedLikeliness = (float)Random::Uniform(0, lTotalLikeliness);
			lTotalLikeliness = 0;
			std::vector<PathIndexLikeliness>::iterator x;
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
			const float lWantedDistance = SCALE_FACTOR * NORMAL_AIM_AHEAD;
			float lStep = lWantedDistance * lPath->GetDistanceNormal();
			if (lStep + lPath->GetCurrentInterpolationTime() > 1)
			{
				lStep = 1 - lPath->GetCurrentInterpolationTime();
			}
			lPath->StepInterpolation(lStep);
			if (mMode == MODE_FIND_PATH_OFF_ELEVATOR)
			{
				SetMode(MODE_GET_OFF_ELEVATOR);
			}
			else
			{
				SetMode(MODE_HEADING_BACK_ON_TRACK);
			}
			mLog.Headlinef(_T("Picked path %i (%i pickable)."), mActivePath, lRelevantPaths.size());
		}
		break;
		case MODE_HEADING_BACK_ON_TRACK:
		{
			if (lModeRunDeltaFrameCount%5 == 2)
			{
				const float lVelocityScaleFactor = std::min(1.0f, lVelocity.GetLength() / 2.5f);
				const float lNearstPathDistance = GetClosestPathDistance(lPosition);
				if (lNearstPathDistance < SCALE_FACTOR * OFF_COURSE_DISTANCE * lVelocityScaleFactor)
				{
					// We were able to return to normal, keep on running.
					SetMode(MODE_NORMAL);
					return;
				}
				/*else if (lNearstPathDistance > SCALE_FACTOR * OFF_COURSE_DISTANCE * lVelocityScaleFactor * 5)
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
			if (mMode == MODE_GET_ON_ELEVATOR && lModeRunTime > 3.5)
			{
				mLog.AHeadline("Something hinders me getting on the elevator, back square one.");
				SetMode(MODE_FIND_BEST_PATH);
				return;
			}

			if (mMode == MODE_NORMAL && lModeRunDeltaFrameCount%20 == 19)
			{
				const float lVelocityScaleFactor = ((mMode == MODE_NORMAL)? 1.0f : 3.0f) * std::min(1.0f, lVelocity.GetLength() / 2.5f);
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
				const float lSlowSpeed = 0.15f * SCALE_FACTOR;
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

			// Are we heading towards an elevator?
			if (mMode == MODE_NORMAL && lPath->GetType() == _T("to_elevator"))
			{
				if (lPath->GetDistanceLeft() <= ELEVATOR_WAIT_DISTANCE)
				{
					mLog.AHeadline("Normal mode close to end of path to elevator, changing mode.");
					SetMode(MODE_WAITING_FOR_ELEVATOR);
					return;
				}
			}

			// Step target (aim) ahead.
			{
				const float lActualDistance2 = lTarget.GetDistanceSquared(lPosition);
				const float lWantedDistance = SCALE_FACTOR * NORMAL_AIM_AHEAD * std::min(1.0f, lVelocity.GetLength() / 5);
				if (lActualDistance2 < lWantedDistance*lWantedDistance)
				{
					const float lMoveAhead = lWantedDistance*1.1f - ::sqrt(lActualDistance2);
					const float lPreStepTime = lPath->GetCurrentInterpolationTime();
					lPath->StepInterpolation(lMoveAhead * lPath->GetDistanceNormal());
					// Did wrap around? That means target is close to the end of our path, so we
					// should try to stop.
					if (lPreStepTime > 0.6f && lPath->GetCurrentInterpolationTime() < 0.2f)
					{
						lPath->GotoAbsoluteTime(END_PATH_TIME);
						const bool lTowardsElevator = (lPath->GetType() == _T("to_elevator"));
						const float lTargetDistance = lTowardsElevator? ON_ELEVATOR_DISTANCE : ON_GOAL_DISTANCE;
						const bool lIsClose = IsCloseToTarget(lPosition, lTargetDistance);
						if (mGame->GetCutie()->GetForwardSpeed() > 4.0f*SCALE_FACTOR || lIsClose)
						{
							if (lTowardsElevator)
							{
								if (mMode == MODE_GET_ON_ELEVATOR)
								{
									// Need to check that we are actually on top of
									// the elevator, otherwise we wait until next frame.
									if (lIsClose)
									{
										SetMode(MODE_ON_ELEVATOR);
										return;
									}
								}
								else
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
					}
					lTarget = lPath->GetValue();
				}
			}

			if (mMode == MODE_GET_OFF_ELEVATOR && lModeRunTime < 0.5f && !IsVertical(lVelocity))
			{
				// Don't start driving yet, adjust the steering wheel before
				// getting off a horizontal elevator.
			}
			else
			{
				// Move forward.
				mGame->GetCutie()->SetEnginePower(0, +lStrength, 0);
				mGame->GetCutie()->SetEnginePower(2, 0, 0);
			}

			// Steer.
			const Vector3DF lDirection = mGame->GetCutie()->GetOrientation() * Vector3DF(0,1,0);
			const Vector3DF lWantedDirection = lTarget-lPosition;
			float lAngle = Vector2DF(lWantedDirection.x, lWantedDirection.y).GetAngle(Vector2DF(lDirection.x, lDirection.y));
			if (mMode == MODE_GET_OFF_ELEVATOR)
			{
				if (lAngle < 0.1f)
				{
					mMode = MODE_NORMAL;
				}
				else
				{
					// Extreme steering wheel motions, to get on and off the elevator smoothly. Usually sharp angles.
					lAngle = (lAngle < 0)? -1.0f : +1.0f;
				}
			}
			else if (mMode == MODE_GET_ON_ELEVATOR)
			{
				lAngle *= 2;	// Make steering more powerful while boarding.
			}
			mGame->GetCutie()->SetEnginePower(1, +lAngle, 0);
			mLastAverageAngle = Math::Lerp(mLastAverageAngle, lAngle, 0.5f);

			// Check if we need to slow down.
			const float lHighSpeed = SCALE_FACTOR * 3.5f;
			const float lAbsAngle = ::fabs(lAngle);
			if (lVelocity.GetLengthSquared() > lHighSpeed*lHighSpeed)
			{
				if (lAbsAngle > 0.5f)
				{
					mGame->GetCutie()->SetEnginePower(2, lAbsAngle*0.001f + lVelocity.GetLength()*0.0001f, 0);
				}
				else if (lPath->GetCurrentInterpolationTime() >= DOUBLE_OFF_END_PATH_TIME &&
					IsCloseToTarget(lPosition, SLOW_DOWN_DISTANCE))
				{
					mGame->GetCutie()->SetEnginePower(2, 0.2f, 0);
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
			mGame->GetCutie()->SetEnginePower(0, 0, 0);
			mGame->GetCutie()->SetEnginePower(2, -lStrength, 0);	// Negative = use full brakes, not only hand brake.
		}
		break;
		case MODE_WAITING_FOR_ELEVATOR:
		{
			/*if (lModeRunTime > 9.0f)
			{
				mLog.AHeadline("Backing up, I've waited for the elevator too long.");
				SetMode(MODE_BACKING_UP);
				return;
			}*/
			if (::fabs(mLastAverageAngle) > 0.1f)
			{
				lStrength *= SMOOTH_BRAKING_FACTOR;	// Smooth braking when turning, we can always back up if necessary.
			}
			if (mElevatorStopPosition.GetDistanceSquared(lPosition) < ELEVATOR_TOO_CLOSE_DISTANCE*ELEVATOR_TOO_CLOSE_DISTANCE)
			{
				mLog.AHeadline("Got too close to the elevator stop position, backing up.");
				mGame->GetCutie()->SetEnginePower(1, 0, 0);	// Back straight.
				const bool lIsMovingForward = (mGame->GetCutie()->GetForwardSpeed() > 0.1f*SCALE_FACTOR);
				mGame->GetCutie()->SetEnginePower(0, lIsMovingForward? 0.0f : -lStrength, 0);
				mGame->GetCutie()->SetEnginePower(2, lIsMovingForward? lStrength :  0.0f, 0);
				return;
			}

			// Check that we're headed towards the elevator center.
			if (lVelocity.GetLengthSquared() < 0.5f)
			{
				Vector3DF lUp(0, 0, 1);
				lUp = GetOrientation() * lUp;
				if (lUp.z > 0.7f)
				{
					const Vector3DF lDirection = mGame->GetCutie()->GetOrientation() * Vector3DF(0,1,0);
					const Vector3DF lWantedDirection = mElevatorStopPosition - lPosition;
					const float lAngle = Vector2DF(lWantedDirection.x, lWantedDirection.y).GetAngle(Vector2DF(lDirection.x, lDirection.y));
					if (::fabs(lAngle) > PIF/6)
					{
						mLastAverageAngle = -lAngle;
						SetMode(MODE_ROTATE_ON_THE_SPOT_WAITING);
						return;
					}
				}
			}

			const Cure::Elevator* lNearestElevator;
			const Vector3DF lNearestLiftPosition = GetClosestElevatorPosition(mElevatorStopPosition, lNearestElevator);
			const float lElevatorDistance2ToElevatorStop = mElevatorStopPosition.GetDistanceSquared(lNearestLiftPosition);
			if (lNearestLiftPosition.z < lPosition.z+0.5f &&
				lElevatorDistance2ToElevatorStop < 2*2)
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
					if (lElevatorDistance2ToElevatorStop+0.1f >= mElevatorStopPosition.GetDistanceSquared(lNearestLiftPosition+lVelocityXY))
					{
						lTryGetOn = true;
					}
				}
				if (lTryGetOn)
				{
					mLog.Infof(_T("Elevator Z=%f, my Z=%f. Getting on!"), lNearestLiftPosition.z, lPosition.z);
					SetMode(MODE_GET_ON_ELEVATOR);
					return;
				}
				else
				{
					mLog.AInfo("Waiting for elevator: not getting on, since elevator is departing!");
				}
			}

			const float lAngle= mLastAverageAngle * 3;
			mGame->GetCutie()->SetEnginePower(1, lAngle, 0);	// Set initial steering in a good direction.
			// Brake!
			mGame->GetCutie()->SetEnginePower(0, 0, 0);
			mGame->GetCutie()->SetEnginePower(2, -lStrength, 0);	// Negative = use full brakes, not only hand brake.
		}
		break;
		case MODE_ON_ELEVATOR:
		{
			lStrength *= SMOOTH_BRAKING_FACTOR;	// Smooth braking, we can always back up if necessary.

			// Check if elevator departed.
			const float lMinimumVelocity2 = 0.7f*0.7f;
			if (lModeRunTime > 1.0f && lVelocity.GetLengthSquared() > lMinimumVelocity2)
			{
				const Cure::Elevator* lNearestElevator;
				const Vector3DF lNearestLiftPosition = GetClosestElevatorPosition(mElevatorStopPosition, lNearestElevator);
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
					if (!IsVertical(lElevatorVelocity))
					{
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
			// Brake!
			mGame->GetCutie()->SetEnginePower(0, 0, 0);
			mGame->GetCutie()->SetEnginePower(2, -lStrength, 0);	// Negative = use full brakes, not only hand brake.
		}
		break;
		case MODE_ROTATE_ON_THE_SPOT_DURING:
		case MODE_ROTATE_ON_THE_SPOT_WAITING:
		{
			const float lAngle = (mLastAverageAngle < 0)? -1.0f : 1.0f;
			float lSteerEndTime = 0.4f;
			float lForwardEndTime = lSteerEndTime + 0.9f;
			float lOtherSteerEndTime = lForwardEndTime + lSteerEndTime;
			float lPeriod = lOtherSteerEndTime + 0.8f;
			if (GetVehicleId() == 1)
			{
				lSteerEndTime = 0.7f;
				lForwardEndTime = lSteerEndTime + 0.7f;
				lOtherSteerEndTime = lForwardEndTime + lSteerEndTime;
				lPeriod = lOtherSteerEndTime + 1.0f;
				lStrength *= SMOOTH_BRAKING_FACTOR;
			}
			const int lIterations = 2;
			if (lModeRunTime > lIterations*lPeriod+lSteerEndTime)
			{
				mGame->GetCutie()->SetEnginePower(0, 0, 0);
				mGame->GetCutie()->SetEnginePower(1, -lAngle, 0);
				mGame->GetCutie()->SetEnginePower(2, -1, 0);
				if (mMode == MODE_ROTATE_ON_THE_SPOT_DURING)
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
					mGame->GetCutie()->SetEnginePower(0, 0, 0);
					mGame->GetCutie()->SetEnginePower(1, -lAngle, 0);
					mGame->GetCutie()->SetEnginePower(2, -lStrength, 0);
					break;
				}
				else if (lModeRunTime >= lBase+lSteerEndTime && lModeRunTime < lBase+lForwardEndTime)
				{
					// Drive forward.
					mGame->GetCutie()->SetEnginePower(0, +lStrength, 0);
					mGame->GetCutie()->SetEnginePower(2, 0, 0);
					break;
				}
				else if (lModeRunTime >= lBase+lForwardEndTime && lModeRunTime < lBase+lOtherSteerEndTime)
				{
					// Brake and turn in "backward direction".
					mGame->GetCutie()->SetEnginePower(0, 0, 0);
					mGame->GetCutie()->SetEnginePower(1, +lAngle, 0);
					mGame->GetCutie()->SetEnginePower(2, -lStrength, 0);
					break;
				}
				else if (lModeRunTime >= lBase+lOtherSteerEndTime && lModeRunTime < lBase+lPeriod)
				{
					// Drive backward.
					mGame->GetCutie()->SetEnginePower(0, -0.7f*lStrength, 0);
					mGame->GetCutie()->SetEnginePower(2, 0, 0);
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
	if (pMode == MODE_BACKING_UP || pMode == MODE_HEADING_BACK_ON_TRACK)
	{
		++mStuckCount;
	}
	else
	{
		mStuckCount = 0;
	}
	if (mStuckCount >= 5)
	{
		pMode = MODE_FIND_BEST_PATH;
		mStuckCount = 0;
	}
	if (pMode == MODE_WAITING_FOR_ELEVATOR && mActivePath != -1)
	{
		Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath(mActivePath);
		lPath->GotoAbsoluteTime(END_PATH_TIME);
		mElevatorStopPosition = lPath->GetValue();
	}
	else if (pMode == MODE_FIND_PATH_OFF_ELEVATOR)
	{
		mActivePath = -1;	// Treat all paths as equals.
	}
	else if (pMode == MODE_HEADING_BACK_ON_TRACK || pMode == MODE_NORMAL)
	{
		mLastAverageAngle = 0;
	}
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
		case MODE_STOPPING_AT_GOAL:		lModeName = _T("STOPPING AT GOAL");		break;
		case MODE_AT_GOAL:			lModeName = _T("AT GOAL");			break;
		case MODE_WAITING_FOR_ELEVATOR:		lModeName = _T("WAITING FOR ELEVATOR");		break;
		case MODE_GET_ON_ELEVATOR:		lModeName = _T("GET ON ELEVATOR");		break;
		case MODE_GET_OFF_ELEVATOR:		lModeName = _T("GET OFF ELEVATOR");		break;
		case MODE_ON_ELEVATOR:			lModeName = _T("ON ELEVATOR");			break;
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

float VehicleAi::GetClosestPathDistance(const Vector3DF& pPosition, int pPath, float* pLikeliness) const
{
	Spline* lPath = mGame->GetLevel()->QueryPath()->GetPath((pPath >= 0)? pPath : mActivePath);
	if (pLikeliness)
	{
		*pLikeliness = lPath->GetLikeliness();
	}
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
	const float lSearchStepLength = (pPath >= 0)? -0.1f : 0.0125f;
	const int lSearchSteps = (pPath >= 0)? 10 : 3;
	lPath->FindNearestTime(lSearchStepLength, pPosition, lNearestDistance, lClosestPoint, lSearchSteps);
	// Steep check.
	if (lNearestDistance < ::fabs(pPosition.z - lClosestPoint.z)*3)
	{
		lNearestDistance *= 5;
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

bool VehicleAi::IsVertical(const Vector3DF& pVector)
{
	return (::fabs(pVector.z) > 2 * (::fabs(pVector.x) + ::fabs(pVector.y)));
}

int VehicleAi::GetVehicleId() const
{
	if (mGame->GetCutie()->GetClassId() == _T("cutie"))
	{
		return 0;
	}
	if (mGame->GetCutie()->GetClassId() == _T("monster"))
	{
		return 1;
	}
	if (mGame->GetCutie()->GetClassId() == _T("corvette"))
	{
		return 2;
	}
	if (mGame->GetCutie()->GetClassId() == _T("road_roller"))
	{
		return 3;
	}
	assert(false);
	return -1;
}

LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, VehicleAi);



}
