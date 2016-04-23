
// Author: Jonas Byström
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
	virtual void OnTrigger(Tbc::PhysicsManager::BodyID pTriggerId, ContextObject* pBody, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pPosition, const vec3& pNormal);

	Launcher* mLauncher;
	float mMaxVelocity;
	float mAcceleration;
	float mExplosiveEnergy;
	bool mIsDetonated;

	logclass();
};



}
