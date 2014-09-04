
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Cure/Include/CppContextObject.h"
#include "../Life.h"



namespace Life
{



class Launcher;



class ServerMine: public Cure::CppContextObject
{
public:
	typedef Cure::CppContextObject Parent;

	ServerMine(Cure::ResourceManager* pResourceManager, const str& pClassId, Launcher* pLauncher);
	virtual ~ServerMine();

private:
	virtual void OnTick();
	virtual void OnForceApplied(Cure::ContextObject* pOtherObject,
		Tbc::PhysicsManager::BodyID pOwnBodyId, Tbc::PhysicsManager::BodyID pOtherBodyId,
		const vec3& pForce, const vec3& pTorque,
		const vec3& pPosition, const vec3& pRelativeVelocity);

	Launcher* mLauncher;
	int mTicksTilFullyActivated;
	int mTicksTilDetonation;
	bool mIsDetonated;

	logclass();
};



}
