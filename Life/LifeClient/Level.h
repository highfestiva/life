
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../UiCure/Include/UiMachine.h"
#include "../../Cure/Include/ContextForceListener.h"
#include "../Life.h"



namespace Life
{



class Level: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	struct MassObjectInfo
	{
		str mClassId;
		int mGroundBodyIndex;
		int mCount;
	};
	typedef std::list<MassObjectInfo> MassObjectList;

	Level(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Cure::ContextForceListener* pGravelEmitter);
	virtual ~Level();
	virtual void OnLoaded();

	MassObjectList GetMassObjects() const;

private:
	virtual void OnForceApplied(Cure::ContextObject* pOtherObject,
		Tbc::PhysicsManager::BodyID pOwnBodyId, Tbc::PhysicsManager::BodyID pOtherBodyId,
		const vec3& pForce, const vec3& pTorque,
		const vec3& pPosition, const vec3& pRelativeVelocity);

	Cure::ContextForceListener* mGravelEmitter;
	MassObjectList mMassObjects;

	logclass();
};



}
