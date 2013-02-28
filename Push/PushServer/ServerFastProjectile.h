
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../Cure/Include/CppContextObject.h"
#include "../Push.h"



namespace Life
{
class Launcher;
}



namespace Push
{



class ServerFastProjectile: public Cure::CppContextObject
{
public:
	typedef Cure::CppContextObject Parent;

	ServerFastProjectile(Cure::ResourceManager* pResourceManager, const str& pClassId, Life::Launcher* pLauncher);
	virtual ~ServerFastProjectile();

private:
	virtual void OnLoaded();
	virtual void OnMicroTick(float pFrameTime);
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody, const Vector3DF& pNormal);

	Life::Launcher* mLauncher;
	float mMaxVelocity;
	float mAcceleration;
	float mExplosiveEnergy;
	bool mIsDetonated;

	LOG_CLASS_DECLARE();
};



}
