
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "UiCppContextObject.h"
#include "../../Cure/Include/ContextForceListener.h"
#include "../../Lepra/Include/HiResTimer.h"
#include "../../UiTBC/Include/UiChunkyClass.h"



namespace UiCure
{



class GravelEmitter: public Cure::ContextForceListener
{
public:
	GravelEmitter(Cure::ResourceManager* pResourceManager, GameUiManager* pUiManager, const str& pParticleClass, float pScale, float pAmount, float pLifeTime);
	virtual ~GravelEmitter();

	virtual void OnForceApplied(Cure::ContextObject* pObject, Cure::ContextObject* pOtherObject,
		TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque,
		const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);

	Cure::ResourceManager* mResourceManager;
	GameUiManager* mUiManager;
	str mParticleClass;
	HiResTimer mParticleTimer;
	Cure::GameObjectId mLastCollidedId;
	float mScale;
	float mDelay;
	float mLifeTime;

	LOG_CLASS_DECLARE();
};



}
