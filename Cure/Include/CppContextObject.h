
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#pragma once

#include "../../lepra/include/lepra.h"	// TRICKY: must be here due to a #define compile optimization.
#include "../../tbc/include/chunkyclass.h"
#include "../../tbc/include/chunkyphysics.h"
#include "contextobject.h"
#include "physicssharedresource.h"



namespace cure {



class CppContextObject: public ContextObject {
	typedef ContextObject Parent;
public:
	CppContextObject(ResourceManager* resource_manager, const str& class_id);
	virtual ~CppContextObject();

	tbc::ChunkyPhysics::GuideMode GetGuideMode() const;
	void StabilizeTick();

	virtual void StartLoading();

	void SetAllowNetworkLogic(bool allow);

	tbc::ChunkyPhysics* GetPhysics() const;
	UserPhysicsReferenceResource* GetPhysicsResource() const;
	void CreatePhysics(tbc::ChunkyPhysics* physics);
	void CreatePhysicsRef(const str& name);
	virtual const tbc::ChunkyClass* GetClass() const;
	const tbc::ChunkyClass::Tag* FindTag(const str& tag_type, int float_value_count, int string_value_count, const std::vector<int>* trigger_index_array = 0) const;
	virtual void SetTagIndex(int index);

protected:
	void SetForceLoadUnique(bool load_unique);
	void StartLoadingPhysics(const str& physics_name);
	virtual bool TryComplete();
	virtual void SetupChildHandlers();

	void OnMicroTick(float frame_time);
	void OnAlarm(int alarm_id, void* extra_data);
	virtual void OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal);
	virtual void OnForceApplied(ContextObject* other_object,
		 tbc::PhysicsManager::BodyID own_body_id, tbc::PhysicsManager::BodyID other_body_id,
		 const vec3& force, const vec3& torque,
		 const vec3& position, const vec3& relative_velocity);

	void OnLoadClass(UserClassResource* class_resource);
	void OnLoadPhysics(UserPhysicsReferenceResource* physics_resource);

	bool GetAllowNetworkLogic() const;

private:
	typedef std::unordered_map<int, const tbc::PhysicsTrigger*> ActiveTriggerGroupMap;

	UserClassResource* class_resource_;
	UserPhysicsReferenceResource* physics_resource_;
	ActiveTriggerGroupMap active_trigger_group_map_;
	bool allow_network_logic_;
	bool force_load_unique_;

	logclass();
};



}
