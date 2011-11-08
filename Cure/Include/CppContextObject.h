
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

#pragma once

#include "../../Lepra/Include/Lepra.h"	// TRICKY: must be here due to a #define compile optimization.
#include "../../TBC/Include/ChunkyPhysics.h"
#include "ContextObject.h"
#include "ResourceManager.h"



namespace Cure
{



class CppContextObject: public ContextObject
{
public:
	CppContextObject(ResourceManager* pResourceManager, const str& pClassId);
	virtual ~CppContextObject();

	TBC::ChunkyPhysics::GuideMode GetGuideMode() const;
	void StabilizeTick();

	void SetAllowNetworkLogic(bool pAllow);

	TBC::ChunkyPhysics* GetPhysics() const;
	virtual const TBC::ChunkyClass* GetClass() const;
	const TBC::ChunkyClass::Tag* FindTag(const str& pTagType, int pFloatValueCount, int pStringValueCount, const std::vector<int>& pTriggerIndexArray) const;

protected:
	virtual void StartLoading();
	void StartLoadingPhysics(const str& pPhysicsName);
	virtual bool TryComplete();

	void OnMicroTick(float pFrameTime);
	void OnAlarm(int pAlarmId, void* pExtraData);
	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTriggerId, TBC::PhysicsManager::ForceFeedbackListener* pBody);
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
