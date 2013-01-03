
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "Push.h"



namespace Push
{



class Launcher;



class Grenade: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	Grenade(Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager, Launcher* pLauncher);
	virtual ~Grenade();

private:
	void Detonate(const Vector3DF& pPosition);
	virtual void OnLoaded();
	void OnTick();
	virtual void Grenade::OnForceApplied(ContextObject* pOtherObject,
		TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque,
		const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);
	void LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource);

	UiCure::UserSound3dResource* mShreekSound;
	UiCure::UserSound3dResource* mLaunchSound;
	Launcher* mLauncher;
	bool mDetonated;

	LOG_CLASS_DECLARE();
};



}
