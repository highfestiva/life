
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../UiCure/Include/UiMachine.h"
#include "../Life.h"



namespace Life
{



class Launcher;



class FastProjectile: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	FastProjectile(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Launcher* pLauncher);
	virtual ~FastProjectile();

protected:
	virtual void OnLoaded();
	void StartBullet(float pMuzzleVelocity);
	virtual void OnMicroTick(float pFrameTime);
	void OnTick();
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody, const Vector3DF& pNormal);
	void LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource);

	UiCure::UserSound3dResource* mShreekSound;
	Launcher* mLauncher;
	float mMaxVelocity;
	float mAcceleration;
	float mExplosiveEnergy;
	int mTickCount;
	bool mIsDetonated;

	LOG_CLASS_DECLARE();
};



}
