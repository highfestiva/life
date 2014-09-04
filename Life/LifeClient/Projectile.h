
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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
		Tbc::PhysicsManager::BodyID pOwnBodyId, Tbc::PhysicsManager::BodyID pOtherBodyId,
		const vec3& pForce, const vec3& pTorque,
		const vec3& pPosition, const vec3& pRelativeVelocity);
	void LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource);

	UiCure::UserSound3dResource* mShreekSound;
	Launcher* mLauncher;
	int mTickCounter;
	bool mIsDetonated;
	float mExplosiveEnergy;

	logclass();
};



}
