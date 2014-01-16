
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Cure/Include/CppContextObject.h"
#include "../Life.h"



namespace Life
{



class Launcher;



class ServerFastProjectile: public Cure::CppContextObject
{
public:
	typedef Cure::CppContextObject Parent;

	ServerFastProjectile(Cure::ResourceManager* pResourceManager, const str& pClassId, Launcher* pLauncher);
	virtual ~ServerFastProjectile();

private:
	virtual void OnLoaded();
	virtual void OnMicroTick(float pFrameTime);
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody, TBC::PhysicsManager::BodyID pBodyId, const Vector3DF& pNormal);

	Launcher* mLauncher;
	float mMaxVelocity;
	float mAcceleration;
	float mExplosiveEnergy;
	bool mIsDetonated;

	LOG_CLASS_DECLARE();
};



}
