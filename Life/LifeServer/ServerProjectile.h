
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Cure/Include/CppContextObject.h"
#include "../Life.h"



namespace Life
{



class Launcher;



class ServerProjectile: public Cure::CppContextObject
{
public:
	typedef Cure::CppContextObject Parent;

	ServerProjectile(Cure::ResourceManager* pResourceManager, const str& pClassId, float pMuzzleVelocity, Launcher* pLauncher);
	virtual ~ServerProjectile();

private:
	virtual void OnLoaded();
	virtual void OnForceApplied(Cure::ContextObject* pOtherObject,
		Tbc::PhysicsManager::BodyID pOwnBodyId, Tbc::PhysicsManager::BodyID pOtherBodyId,
		const vec3& pForce, const vec3& pTorque,
		const vec3& pPosition, const vec3& pRelativeVelocity);

	float mMuzzleVelocity;
	Launcher* mLauncher;
	bool mIsDetonated;
	float mExplosiveEnergy;

	logclass();
};



}
