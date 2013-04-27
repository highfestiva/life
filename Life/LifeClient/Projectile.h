
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../UiCure/Include/UiMachine.h"
#include "../Life.h"



namespace Life
{



class Launcher;



class Projectile: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	Projectile(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Launcher* pLauncher);
	virtual ~Projectile();

private:
	virtual void OnLoaded();
	void OnTick();
	virtual void OnForceApplied(ContextObject* pOtherObject,
		TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque,
		const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);
	void LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource);

	UiCure::UserSound3dResource* mShreekSound;
	Launcher* mLauncher;
	int mTickCounter;
	bool mIsDetonated;
	float mExplosiveEnergy;

	LOG_CLASS_DECLARE();
};



}
