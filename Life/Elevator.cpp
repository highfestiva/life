
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "Elevator.h"
#include "../Cure/Include/ContextManager.h"
#include "../Lepra/Include/HashUtil.h"
#include "../TBC/Include/PhysicsEngine.h"
#include "../TBC/Include/PhysicsTrigger.h"



namespace Life
{



Elevator::Elevator(Cure::ResourceManager* pResourceManager):
	Cure::CppContextObject(pResourceManager, _T("Elevator")),
	mActiveTrigger(0),
	mExitDelay(2.0)
{
}

Elevator::~Elevator()
{
}



void Elevator::OnTick(float pFrameTime)
{
	Parent::OnTick(pFrameTime);

	mTrigTime.UpdateTimer();
	if (mActiveTrigger && mTrigTime.GetTimeDiff() > mExitDelay)
	{
		log_adebug("TRIGGER - exited trigger volume.");
		OnAlarm(0, 0);
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
		// Run engine for some time before *forcing* deactivation.
		GetManager()->AddAlarmCallback(this, pAlarmId, 30, 0);
	}
	else
	{
		log_adebug("TRIGGER - no longer triggered.");
		if (mActiveTrigger)
		{
			const int lEngineCount = mActiveTrigger->GetControlledEngineCount();
			for (int y = 0; y < lEngineCount; ++y)
			{
				const EngineTrigger& lEngineTrigger = mActiveTrigger->GetControlledEngine(y);
				lEngineTrigger.mEngine->SetValue(0, 0, 0);
			}
		}
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
	GetManager()->CancelPendingAlarmCallbacksById(this, 0);
	mActiveTrigger = lTrigger;
	mTrigTime.PopTimeDiff();
	const int lEngineCount = lTrigger->GetControlledEngineCount();
	for (int y = 0; y < lEngineCount; ++y)
	{
		const TBC::PhysicsTrigger::EngineTrigger& lEngineTrigger = lTrigger->GetControlledEngine(y);
		log_volatile(mLog.Debugf(_T("TRIGGER - trigging function %s."), lEngineTrigger.mFunction.c_str()));
		GetManager()->AddAlarmCallback(this, 0, lEngineTrigger.mDelay, (void*)&lEngineTrigger);
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
			lTargetValue = -1;
		}
	}
	else if (pFunction == _T("stop"))
	{
	}
	else
	{
		assert(false);
	}
	log_volatile(mLog.Debugf(_T("TRIGGER - activating engine for function %s."), pFunction.c_str()));
	pEngine->SetValue(0, lTargetValue, 0);	// Aspect is always 0 for triggered engines.
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Elevator);




}
