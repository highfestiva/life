
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "Launcher.h"
#include "Push.h"



namespace Push
{



class Grenade: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	Grenade(Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager, float pMuzzleVelocity, Launcher* pLauncher);
	virtual ~Grenade();

private:
	void Launch();
	void OnTick();
	virtual void OnForceApplied(Cure::ContextObject* pOtherObject,
		TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque,
		const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);
	void LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource);

	UiCure::UserSound3dResource* mShreekSound;
	UiCure::UserSound3dResource* mLaunchSound;
	int mTimeFrameCreated;
	float mMuzzleVelocity;
	Launcher* mLauncher;
	bool mIsLaunched;
	bool mIsExploded;

	LOG_CLASS_DECLARE();
};



}
