
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



class ServerMine: public Cure::CppContextObject
{
public:
	typedef Cure::CppContextObject Parent;

	ServerMine(Cure::ResourceManager* pResourceManager, const str& pClassId, Life::Launcher* pLauncher);
	virtual ~ServerMine();

private:
	virtual void OnTick();
	virtual void OnForceApplied(Cure::ContextObject* pOtherObject,
		TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque,
		const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);

	Life::Launcher* mLauncher;
	int mTicksTilFullyActivated;
	int mTicksTilDetonation;
	bool mIsDetonated;

	LOG_CLASS_DECLARE();
};



}
