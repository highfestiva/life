
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games

#pragma once

#include "ContextObject.h"
#include "ResourceManager.h"



namespace Cure
{



class CppContextObject: public ContextObject
{
public:
	CppContextObject(const Lepra::String& pClassId);
	virtual ~CppContextObject();

	void StartLoading();

protected:
	void __StartLoadingFuckedUpPhysicsRemoveMe(Cure::UserResource* pClassResource, const TBC::ChunkyClass* pClass);
	void StartLoadingPhysics(const Lepra::String& pPhysicsName);

	virtual void TryComplete();

private:
	void OnTick(float pFrameTime);
	void OnAlarm(int pAlarmId);
	void OnTrigger(TBC::PhysicsManager::BodyID pBody1, TBC::PhysicsManager::BodyID pBody2);
	void OnForceApplied(TBC::PhysicsManager::ForceFeedbackListener* pOtherObject,
		const Lepra::Vector3DF& pForce, const Lepra::Vector3DF& pTorque);

	void OnLoadClass(UserClassResource* pClassResource);
	void OnLoadPhysics(UserPhysicsResource* pPhysicsResource);

	UserClassResource* mClassResource;
	UserPhysicsResource* mPhysicsResource;
	LOG_CLASS_DECLARE();
};



}
