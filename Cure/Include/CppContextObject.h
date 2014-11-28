
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#pragma once

#include "../../Lepra/Include/Lepra.h"	// TRICKY: must be here due to a #define compile optimization.
#include "../../Tbc/Include/ChunkyClass.h"
#include "../../Tbc/Include/ChunkyPhysics.h"
#include "ContextObject.h"
#include "PhysicsSharedResource.h"



namespace Cure
{



class CppContextObject: public ContextObject
{
	typedef ContextObject Parent;
public:
	CppContextObject(ResourceManager* pResourceManager, const str& pClassId);
	virtual ~CppContextObject();

	Tbc::ChunkyPhysics::GuideMode GetGuideMode() const;
	void StabilizeTick();

	virtual void StartLoading();

	void SetAllowNetworkLogic(bool pAllow);

	Tbc::ChunkyPhysics* GetPhysics() const;
	void CreatePhysics(Tbc::ChunkyPhysics* pPhysics);
	virtual const Tbc::ChunkyClass* GetClass() const;
	const Tbc::ChunkyClass::Tag* FindTag(const str& pTagType, int pFloatValueCount, int pStringValueCount, const std::vector<int>* pTriggerIndexArray = 0) const;
	virtual void SetTagIndex(int pIndex);

protected:
	void SetForceLoadUnique(bool pLoadUnique);
	void StartLoadingPhysics(const str& pPhysicsName);
	virtual bool TryComplete();
	virtual void SetupChildHandlers();

	void OnMicroTick(float pFrameTime);
	void OnAlarm(int pAlarmId, void* pExtraData);
	virtual void OnTrigger(Tbc::PhysicsManager::TriggerID pTriggerId, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pNormal);
	virtual void OnForceApplied(ContextObject* pOtherObject,
		 Tbc::PhysicsManager::BodyID pOwnBodyId, Tbc::PhysicsManager::BodyID pOtherBodyId,
		 const vec3& pForce, const vec3& pTorque,
		 const vec3& pPosition, const vec3& pRelativeVelocity);

	void OnLoadClass(UserClassResource* pClassResource);
	void OnLoadPhysics(UserPhysicsReferenceResource* pPhysicsResource);

	bool GetAllowNetworkLogic() const;

private:
	typedef std::unordered_map<int, const Tbc::PhysicsTrigger*> ActiveTriggerGroupMap;

	UserClassResource* mClassResource;
	UserPhysicsReferenceResource* mPhysicsResource;
	ActiveTriggerGroupMap mActiveTriggerGroupMap;
	bool mAllowNetworkLogic;
	bool mForceLoadUnique;

	logclass();
};



}
