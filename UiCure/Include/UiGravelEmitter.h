
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiCppContextObject.h"
#include "../../Cure/Include/ContextForceListener.h"
#include "../../Lepra/Include/HiResTimer.h"
#include "../../UiTbc/Include/UiChunkyClass.h"



namespace UiCure
{



class GravelEmitter: public Cure::ContextForceListener
{
public:
	GravelEmitter(Cure::ResourceManager* pResourceManager, GameUiManager* pUiManager, float pSensitivity, float pScale, float pAmount, float pLifeTime);
	virtual ~GravelEmitter();

	virtual void OnForceApplied(Cure::ContextObject* pObject, Cure::ContextObject* pOtherObject,
		Tbc::PhysicsManager::BodyID pOwnBodyId, Tbc::PhysicsManager::BodyID pOtherBodyId,
		const vec3& pForce, const vec3& pTorque,
		const vec3& pPosition, const vec3& pRelativeVelocity);

	Cure::ResourceManager* mResourceManager;
	GameUiManager* mUiManager;
	HiResTimer mParticleTimer;
	float mSensitivityFactor;
	float mScale;
	float mDelay;
	float mLifeTime;

	logclass();
};



}
