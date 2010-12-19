
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "Elevator.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Lepra/Include/HashUtil.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "../../TBC/Include/PhysicsTrigger.h"



namespace Life
{



Elevator::Elevator(Cure::ContextManager* pManager):
	Cure::CppContextObject(pManager->GetGameManager()->GetResourceManager(), _T("Elevator")),
	mActiveTrigger(0),
	mStoppedTime(0),
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



void Elevator::OnTick(float pFrameTime)
{
	Parent::OnTick(pFrameTime);

	const float lSpeedStillSquare = 0.1f;
	const float lMaxSpeedSquare = GetActiveMaxSpeedSquare();
	if (lMaxSpeedSquare >= lSpeedStillSquare && mElevatorIsActive)
	{
		mElevatorHasBeenMoving = true;
	}

	mEngineActivity = Math::Lerp(mEngineActivity, lMaxSpeedSquare, Math::GetIterateLerpTime(0.4f, pFrameTime));
	if (mEngineActivity < lSpeedStillSquare)
	{
		if (mStopTimer.IsStarted())
		{
			mStoppedTime += mStopTimer.PopTimeDiff();
		}
		else
		{
			mStopTimer.Start();
		}
		const TBC::PhysicsTrigger* lTrigger = 0;
		const int lTriggerCount = GetTriggerCount((const void*&)lTrigger);
		const bool lIsNonStop = (lTriggerCount == 1 || lTrigger->GetType() == TBC::PhysicsTrigger::TRIGGER_NON_STOP);
		if (!lIsNonStop || mStoppedTime >= mStopDelay)
		{
			mStoppedTime = 0;
			if (mElevatorHasBeenMoving)
			{
				log_adebug("TRIGGER - elevator has stopped.");
				bool lStopEngines = !lIsNonStop;
				HaltActiveEngines(lStopEngines);

				// Check if we need to restart "non_stop" or "always" trigger.
				if (lTriggerCount == 1 && lTrigger->GetType() <= TBC::PhysicsTrigger::TRIGGER_ALWAYS)
				{
					Trig(lTrigger);
				}
			}
			mEngineActivity = 100;	// Don't try again in a long time.

			// Low engine activity and no longer actively triggered means we lost the previous trigger.
			if (mActiveTrigger && mActiveTrigger->GetType() > TBC::PhysicsTrigger::TRIGGER_ALWAYS &&
				mTrigTime.QueryTimeDiff() > mExitDelay)
			{
				log_adebug("TRIGGER - exited trigger volume.");
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
		mEngineActivity = 100;
		if (mActiveTrigger && mActiveTrigger->GetType() > TBC::PhysicsTrigger::TRIGGER_ALWAYS)
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



float Elevator::GetActiveMaxSpeedSquare() const
{
	float lMaxSpeed = 0;
	if (mActiveTrigger)
	{
		const TBC::PhysicsManager* lPhysicsManager = GetManager()->GetGameManager()->GetPhysicsManager();
		const int lEngineCount = mActiveTrigger->GetControlledEngineCount();
		for (int y = 0; y < lEngineCount; ++y)
		{
			const TBC::PhysicsTrigger::EngineTrigger& lEngineTrigger = mActiveTrigger->GetControlledEngine(y);
			lMaxSpeed = std::max(lMaxSpeed, lEngineTrigger.mEngine->GetCurrentMaxSpeedSquare(lPhysicsManager));
		}
	}
	return (lMaxSpeed);
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
			lTargetValue = -pEngine->GetValues()[4];	// Invert shadow if stopped.
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
	pEngine->ForceSetValue(4, lTargetValue);	// Store shadow.
	pEngine->SetValue(pEngine->GetControllerIndex(), lTargetValue, 0);
	mParent->ForceSend();	// Transmit our updated engine values.
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Elevator);




}
