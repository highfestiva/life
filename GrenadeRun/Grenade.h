
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../UiCure/Include/UiMachine.h"



namespace GrenadeRun
{



using namespace Lepra;



class Grenade: public UiCure::Machine
{
public:
	typedef UiCure::Machine Parent;

	Grenade(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Grenade();

	void Start();

private:
	void OnTick();
	void OnAlarm(int pAlarmId, void*);
	virtual void OnForceApplied(TBC::PhysicsManager::ForceFeedbackListener* pOtherObject,
		TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque,
		const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);
	void LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource);

	UiCure::UserSound3dResource* mShreekSound;
	UiCure::UserSound3dResource* mLaunchSound;
	bool mExploded;

	LOG_CLASS_DECLARE();
};



}
