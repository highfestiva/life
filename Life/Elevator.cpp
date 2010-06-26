
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "Elevator.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/GameManager.h"
#include "../Lepra/Include/HashUtil.h"
#include "../TBC/Include/PhysicsEngine.h"
#include "../TBC/Include/PhysicsTrigger.h"



namespace Life
{



Elevator::Elevator(Cure::ResourceManager* pResourceManager):
	Cure::CppContextObject(pResourceManager, _T("Elevator")),
	mActiveTrigger(0),
	mExitDelay(2.0),
	mAreEnginesActive(false),
	mEngineActivity(1)
{
}

Elevator::~Elevator()
{
}



void Elevator::OnTick(float pFrameTime)
{
	Parent::OnTick(pFrameTime);

	mTrigTime.UpdateTimer();
	if (mActiveTrigger && mActiveTrigger->GetType() != TBC::PhysicsTrigger::TRIGGER_ALWAYS
		&& mTrigTime.GetTimeDiff() > mExitDelay)
	{
		log_adebug("TRIGGER - exited trigger volume.");
		OnAlarm(0, 0);
	}

	mEngineActivity = Math::Lerp(mEngineActivity, GetActiveMaxSpeedSquare(), 0.1f);
	if (mAreEnginesActive && Math::IsEpsEqual(mEngineActivity, 0.0f, 0.1f))
	{
		log_adebug("TRIGGER - elevator has stopped.");
		HaltActiveEngines();

		const TBC::PhysicsTrigger* lTrigger = 0;
		if (!mActiveTrigger && GetTriggerCount((const void*&)lTrigger) == 1)
		{
			if (lTrigger->GetType() == TBC::PhysicsTrigger::TRIGGER_ALWAYS)
			{
				Trig(lTrigger);
			}
		}
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
		mAreEnginesActive = true;
		mEngineActivity = 100;
		// Run engine for some time before *forcing* deactivation.
		GetManager()->AddAlarmCallback(this, pAlarmId, 30, 0);
	}
	else
	{
		log_adebug("TRIGGER - no longer triggered.");
		HaltActiveEngines();
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

void Elevator::HaltActiveEngines()
{
	mAreEnginesActive = false;
	if (mActiveTrigger)
	{
		const int lEngineCount = mActiveTrigger->GetControlledEngineCount();
		for (int y = 0; y < lEngineCount; ++y)
		{
			const TBC::PhysicsTrigger::EngineTrigger& lEngineTrigger = mActiveTrigger->GetControlledEngine(y);
			lEngineTrigger.mEngine->SetValue(0, 0, 0);
		}
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
	pEngine->ForceSetValue(4, lTargetValue);	// Store shadow.
	pEngine->SetValue(0, lTargetValue, 0);	// Aspect is always 0 for triggered engines.
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Elevator);




}
