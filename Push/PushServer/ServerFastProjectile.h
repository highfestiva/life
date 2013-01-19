
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

	ServerFastProjectile(Cure::ResourceManager* pResourceManager, const str& pClassId, float pMuzzleVelocity, Life::Launcher* pLauncher);
	virtual ~ServerFastProjectile();

private:
	virtual void OnLoaded();
	virtual void OnMicroTick(float pFrameTime);
	virtual void OnTick();
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody);

	float mMuzzleVelocity;
	Life::Launcher* mLauncher;
	bool mIsDetonated;

	LOG_CLASS_DECLARE();
};



}
