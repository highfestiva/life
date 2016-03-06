
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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

	vec3 GetPosition() const;
	float GetCaptureLevel() const;
	void StartSlideDown();

private:
	virtual void FinalizeTrigger(const Tbc::PhysicsTrigger* pTrigger);
	virtual void OnTick();
	virtual void OnTrigger(Tbc::PhysicsManager::BodyID pTriggerId, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pPosition, const vec3& pNormal);

	const Tbc::PhysicsTrigger* mTrigger;
	vec3 mFlagOffset;
	vec3 mFlagTop;
	vec3 mCatchingFlagVelocity;
	vec3 mStartFlagVelocity;
	bool mLastFrameTriggered;
	bool mIsTriggerTimerStarted;
	HiResTimer mTriggerTimer;
	Tbc::GeometryReference* mFlagMesh;
	Tbc::GeometryReference* mBlinkMesh;
	bool mSlideDown;
	vec3 mBlinkStartColor;
	float mBlinkTime;

	logclass();
};



}
