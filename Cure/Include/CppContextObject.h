
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

#pragma once

#include "ContextObject.h"
#include "ResourceManager.h"



namespace Cure
{



class CppContextObject: public ContextObject
{
public:
	CppContextObject(const str& pClassId);
	virtual ~CppContextObject();

protected:
	void __StartLoadingFuckedUpPhysicsRemoveMe(Cure::UserResource* pClassResource, const TBC::ChunkyClass* pClass);

	virtual void StartLoading();
	void StartLoadingPhysics(const str& pPhysicsName);
	virtual bool TryComplete();

	TBC::ChunkyPhysics* GetPhysics() const;

	void OnTick(float pFrameTime);
	void OnAlarm(int pAlarmId);
	void OnTrigger(TBC::PhysicsManager::TriggerID pTrigger, TBC::PhysicsManager::ForceFeedbackListener* pBody);
	void OnForceApplied(TBC::PhysicsManager::ForceFeedbackListener* pOtherObject,
		const Vector3DF& pForce, const Vector3DF& pTorque);

	void OnLoadClass(UserClassResource* pClassResource);
	void OnLoadPhysics(UserPhysicsResource* pPhysicsResource);

private:
	UserClassResource* mClassResource;
	UserPhysicsResource* mPhysicsResource;

	LOG_CLASS_DECLARE();
};



}
