
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../UiCure/Include/UiMachine.h"
#include "Push.h"



namespace Life
{
class Launcher;
}



namespace Push
{



class FastProjectile: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	FastProjectile(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager, Life::Launcher* pLauncher);
	virtual ~FastProjectile();

private:
	void Detonate(const Vector3DF& pPosition);
	virtual void OnLoaded();
	virtual void OnMicroTick(float pFrameTime);
	void OnTick();
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, ContextObject* pBody);
	void LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource);

	UiCure::UserSound3dResource* mShreekSound;
	Life::Launcher* mLauncher;
	bool mIsDetonated;

	LOG_CLASS_DECLARE();
};



}
