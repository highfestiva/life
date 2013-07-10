
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
		TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque,
		const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);

	Launcher* mLauncher;
	int mTicksTilFullyActivated;
	int mTicksTilDetonation;
	bool mIsDetonated;

	LOG_CLASS_DECLARE();
};



}
