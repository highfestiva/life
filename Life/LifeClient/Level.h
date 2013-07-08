
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
		TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque,
		const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);

	Cure::ContextForceListener* mGravelEmitter;
	MassObjectList mMassObjects;

	LOG_CLASS_DECLARE();
};



}
