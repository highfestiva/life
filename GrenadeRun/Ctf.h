
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Cure/Include/CppContextObject.h"
#include "../Lepra/Include/HiResTimer.h"
#include "Game.h"



namespace GrenadeRun
{



class Ctf: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	Ctf(Cure::ContextManager* pManager);
	virtual ~Ctf();

	Vector3DF GetPosition() const;
	float GetCaptureLevel() const;

private:
	virtual void FinalizeTrigger(const TBC::PhysicsTrigger* pTrigger);
	virtual void OnTick();
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, TBC::PhysicsManager::ForceFeedbackListener* pListener);

	const TBC::PhysicsTrigger* mTrigger;
	Vector3DF mFlagOffset;
	Vector3DF mFlagTop;
	Vector3DF mCatchingFlagVelocity;
	bool mLastFrameTriggered;
	bool mIsTriggerTimerStarted;
	HiResTimer mTriggerTimer;
	TBC::GeometryReference* mFlagMesh;

	LOG_CLASS_DECLARE();
};



}
