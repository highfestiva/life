
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
	CppContextObject(ResourceManager* pResourceManager, const str& pClassId);
	virtual ~CppContextObject();

	void SetAllowNetworkLogic(bool pAllow);

protected:
	virtual void StartLoading();
	void StartLoadingPhysics(const str& pPhysicsName);
	virtual bool TryComplete();

	TBC::ChunkyPhysics* GetPhysics() const;

	void OnTick(float pFrameTime);
	void OnAlarm(int pAlarmId, void* pExtraData);
	void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, TBC::PhysicsManager::ForceFeedbackListener* pBody);
	void OnForceApplied(TBC::PhysicsManager::ForceFeedbackListener* pOtherObject,
		 TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		 const Vector3DF& pForce, const Vector3DF& pTorque,
		 const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);

	void OnLoadClass(UserClassResource* pClassResource);
	void OnLoadPhysics(UserPhysicsResource* pPhysicsResource);

	bool GetAllowNetworkLogic() const;

private:
	typedef std::hash_map<int, const TBC::PhysicsTrigger*> ActiveTriggerGroupMap;

	UserClassResource* mClassResource;
	UserPhysicsResource* mPhysicsResource;
	ActiveTriggerGroupMap mActiveTriggerGroupMap;
	bool mAllowNetworkLogic;

	LOG_CLASS_DECLARE();
};



}
