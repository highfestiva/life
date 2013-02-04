
// Author: Jonas Bystr�m
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



class ServerProjectile: public Cure::CppContextObject
{
public:
	typedef Cure::CppContextObject Parent;

	ServerProjectile(Cure::ResourceManager* pResourceManager, const str& pClassId, float pMuzzleVelocity, Life::Launcher* pLauncher);
	virtual ~ServerProjectile();

private:
	virtual void OnLoaded();
	virtual void OnForceApplied(Cure::ContextObject* pOtherObject,
		TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque,
		const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);

	float mMuzzleVelocity;
	Life::Launcher* mLauncher;
	bool mIsDetonated;
	float mExplosiveEnergy;

	LOG_CLASS_DECLARE();
};



}
