
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../Cure/Include/CppContextObject.h"
#include "../Push.h"



namespace Push
{



class ServerLauncher;



class ServerGrenade: public Cure::CppContextObject
{
public:
	typedef Cure::CppContextObject Parent;

	ServerGrenade(Cure::ResourceManager* pResourceManager, float pMuzzleVelocity, ServerLauncher* pLauncher);
	virtual ~ServerGrenade();

	Cure::GameObjectId GetOwnerId() const;
	void SetOwnerId(Cure::GameObjectId pOwnerId);

private:
	virtual void OnLoaded();
	void OnTick();
	virtual void OnForceApplied(Cure::ContextObject* pOtherObject,
		TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque,
		const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);

	float mMuzzleVelocity;
	ServerLauncher* mLauncher;
	bool mIsLaunched;
	bool mIsExploded;
	Cure::GameObjectId mOwnerId;

	LOG_CLASS_DECLARE();
};



}
