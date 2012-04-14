
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/Elevator.h"
#include "../Include/ContextManager.h"
#include "../Include/GameManager.h"
#include "../Include/TimeManager.h"
#include "../../Lepra/Include/HashUtil.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "../../TBC/Include/PhysicsTrigger.h"



namespace Cure
{



#define STARTS_MOVING_VELOCITY	0.6f
#define STOPS_MOVING_VELOCITY	0.3f
#define ELEVATOR_ACTIVITY_RESET	1000




Elevator::Elevator(ContextManager* pManager):
	CppContextObject(pManager->GetGameManager()->GetResourceManager(), _T("Elevator")),
	mActiveTrigger(0),
	mExitDelay(2.0),
	mStopDelay(5.0),
	mElevatorHasBeenMoving(true),	// Set to get this party started in some cases.
	mElevatorIsActive(true),	// Set to get this party started in some cases.
	mEngineActivity(1)
{
	pManager->AddLocalObject(this);
	GetManager()->EnableTickCallback(this);
}

Elevator::~Elevator()
{
}



Vector3DF Elevator::GetPosition() const
{
	return GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyPosition(GetFirstBody()->GetBodyId());
}

Vector3DF Elevator::GetVelocity() const
{
	Vector3DF lVelocity;
	GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyVelocity(GetFirstBody()->GetBodyId(), lVelocity);
	return lVelocity;
}

float Elevator::GetRadius() const
{
	const Vector3DF lSize = GetFirstBody()->GetShapeSize();
	return std::max(lSize.x, lSize.y) * 0.5f;
}


void Elevator::OnTick()
{
	Parent::OnTick();

	const float lSignedMaxSpeed2 = GetSignedMaxSpeedSquare();
	if (::fabs(lSignedMaxSpeed2) >= STARTS_MOVING_VELOCITY && mElevatorIsActive)
	{
		mElevatorHasBeenMoving = true;
	}

	const float lFrameTime = GetManager()->GetGameManager()->GetTimeManager()->GetNormalFrameTime();
	mEngineActivity = Math::Lerp(mEngineActivity, lSignedMaxSpeed2, Math::GetIterateLerpTime(0.4f, lFrameTime));
	if (::fabs(lSignedMaxSpeed2) < STARTS_MOVING_VELOCITY && ::fabs(mEngineActivity) < STOPS_MOVING_VELOCITY)
	{
		const TBC::PhysicsTrigger* lTrigger = 0;
		const int lTriggerCount = GetTriggerCount((const void*&)lTrigger);
		const bool lIsNonStop = (lTriggerCount == 1 && lTrigger->GetType() == TBC::PhysicsTrigger::TRIGGER_NON_STOP);
		bool lHasStopped = true;
		if (lIsNonStop)
		{
			mStopTimer.TryStart();
			lHasStopped = (mStopTimer.QueryTimeDiff() >= mStopDelay);
		}
		if (lHasStopped)
		{
			mStopTimer.ClearTimeDiff();
			const bool lStopEngines = !lIsNonStop;
			if (mElevatorHasBeenMoving)
			{
				log_adebug("TRIGGER - elevator has stopped.");
				HaltActiveEngines(lStopEngines);

				// Check if we need to restart "non_stop" or "always" trigger.
				if (lTriggerCount == 1 && lTrigger->GetType() <= TBC::PhysicsTrigger::TRIGGER_ALWAYS)
				{
					Trig(lTrigger);
				}
			}
			mEngineActivity = ELEVATOR_ACTIVITY_RESET;	// Don't try again in a long time.

			// Low engine activity and no longer actively triggered means we lost the previous trigger.
			if (mActiveTrigger && mActiveTrigger->GetType() >= TBC::PhysicsTrigger::TRIGGER_MOVEMENT &&
				mTrigTime.QueryTimeDiff() > mExitDelay)
			{
				log_adebug("TRIGGER - exited trigger volume.");
				// Stop the engines once again, to handle the following scenario:
				// 1. Physically trigged, engine started.
				// 2. Physical object came to end-point.
				// 3. Elevator stopped due to (2).
				// 4. Still physically triggered, engine re-started!
				HaltActiveEngines(lStopEngines);
				mActiveTrigger = 0;
			}
		}
	}
	else
	{
		mStopTimer.Stop();
	}
}

void Elevator::OnAlarm(int pAlarmId, void* pExtraData)
{
	Parent::OnAlarm(pAlarmId, pExtraData);

	typedef TBC::PhysicsTrigger::EngineTrigger EngineTrigger;
	const EngineTrigger* lEngineTrigger = (const EngineTrigger*)pExtraData;
	if (lEngineTrigger)
	{
		SetFunctionTarget(lEngineTrigger->mFunction, lEngineTrigger->mEngine);
		mEngineActivity = ELEVATOR_ACTIVITY_RESET;	// Don't try again in a long time.
		if (mActiveTrigger && mActiveTrigger->GetType() >= TBC::PhysicsTrigger::TRIGGER_MOVEMENT)
		{
			// Run engine for some time before *forcing* deactivation.
			GetManager()->AddAlarmCallback(this, pAlarmId, 60, 0);
		}
	}
	else
	{
		log_adebug("TRIGGER - no longer triggered.");
		HaltActiveEngines(true);
		mActiveTrigger = 0;
	}
}

void Elevator::OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, TBC::PhysicsManager::ForceFeedbackListener*)
{
	const TBC::PhysicsTrigger* lTrigger = (const TBC::PhysicsTrigger*)GetTrigger(pTriggerId);
	assert(lTrigger);
	if (lTrigger == mActiveTrigger)
	{
		mTrigTime.PopTimeDiff();
		return;
	}
	if (mActiveTrigger && lTrigger->GetPriority() > mActiveTrigger->GetPriority())
	{
		return;
	}
	Trig(lTrigger);
}

void Elevator::Trig(const TBC::PhysicsTrigger* pTrigger)
{
	GetManager()->CancelPendingAlarmCallbacksById(this, 0);
	mActiveTrigger = pTrigger;
	mTrigTime.PopTimeDiff();
	const int lEngineCount = pTrigger->GetControlledEngineCount();
	for (int y = 0; y < lEngineCount; ++y)
	{
		const TBC::PhysicsTrigger::EngineTrigger& lEngineTrigger = pTrigger->GetControlledEngine(y);
		log_volatile(mLog.Debugf(_T("TRIGGER - trigging function %s."), lEngineTrigger.mFunction.c_str()));
		GetManager()->AddAlarmCallback(this, 0, lEngineTrigger.mDelay, (void*)&lEngineTrigger);
	}
}

TBC::ChunkyBoneGeometry* Elevator::GetFirstBody() const
{
	const TBC::PhysicsTrigger* lTrigger = 0;
	GetTriggerCount((const void*&)lTrigger);
	assert(lTrigger && lTrigger->GetControlledEngineCount() > 0);
	const TBC::PhysicsTrigger::EngineTrigger& lEngineTrigger = lTrigger->GetControlledEngine(0);
	typedef TBC::PhysicsEngine::GeometryList BodyList;
	BodyList lBodyList = lEngineTrigger.mEngine->GetControlledGeometryList();
	assert(!lBodyList.empty());
	return lBodyList[0];
}



float Elevator::GetSignedMaxSpeedSquare() const
{
	float lMaxSpeed2 = 0;
	float lSignedMaxSpeed2 = 0;
	if (mActiveTrigger)
	{
		const TBC::PhysicsManager* lPhysicsManager = GetManager()->GetGameManager()->GetPhysicsManager();
		const int lEngineCount = mActiveTrigger->GetControlledEngineCount();
		for (int y = 0; y < lEngineCount; ++y)
		{
			const TBC::PhysicsTrigger::EngineTrigger& lEngineTrigger = mActiveTrigger->GetControlledEngine(y);
			Vector3DF lVelocity = lEngineTrigger.mEngine->GetCurrentMaxSpeed(lPhysicsManager);
			const float lSpeed2 = lVelocity.GetLengthSquared();
			if (lSpeed2 > lMaxSpeed2)
			{
				lMaxSpeed2 = lSpeed2;
			}
			lSignedMaxSpeed2 = (lVelocity.z < 0.5f)? -lMaxSpeed2 : lMaxSpeed2;
		}
	}
	return lSignedMaxSpeed2;
}

void Elevator::HaltActiveEngines(bool pStop)
{
	mElevatorHasBeenMoving = false;
	mElevatorIsActive = false;
	if (mActiveTrigger && pStop)
	{
		const int lEngineCount = mActiveTrigger->GetControlledEngineCount();
		for (int y = 0; y < lEngineCount; ++y)
		{
			const TBC::PhysicsTrigger::EngineTrigger& lEngineTrigger = mActiveTrigger->GetControlledEngine(y);
			lEngineTrigger.mEngine->SetValue(lEngineTrigger.mEngine->GetControllerIndex(), 0, 0);
		}
		mParent->ForceSend();	// Transmit our updated engine values.
	}
}



void Elevator::SetFunctionTarget(const str& pFunction, TBC::PhysicsEngine* pEngine)
{
	float lTargetValue = 0;	// Default it to stop.
	if (pFunction == _T("minimum"))
	{
		lTargetValue = -1;
	}
	else if (pFunction == _T("maximum"))
	{
		lTargetValue = 1;
	}
	else if (pFunction == _T("toggle"))
	{
		lTargetValue = -pEngine->GetValue();
		if (Math::IsEpsEqual(lTargetValue, 0.0f))
		{
			lTargetValue = -pEngine->GetValues()[TBC::PhysicsEngine::ASPECT_LOCAL_SHADOW];	// Invert shadow if stopped.
			if (Math::IsEpsEqual(lTargetValue, 0.0f))
			{
				lTargetValue = -1;
			}
		}
	}
	else
	{
		assert(false);
	}
	log_volatile(mLog.Debugf(_T("TRIGGER - activating engine for function %s."), pFunction.c_str()));
	mElevatorIsActive = true;
	pEngine->ForceSetValue(TBC::PhysicsEngine::ASPECT_LOCAL_SHADOW, lTargetValue);	// Store shadow.
	pEngine->SetValue(pEngine->GetControllerIndex(), lTargetValue, 0);
	mParent->ForceSend();	// Transmit our updated engine values.
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Elevator);




}
