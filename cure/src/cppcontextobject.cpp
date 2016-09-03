
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/cppcontextobject.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/diskfile.h"
#include "../../lepra/include/hashutil.h"
#include "../../tbc/include/physicsengine.h"
#include "../../tbc/include/physicstrigger.h"
#include "../include/contextmanager.h"
#include "../include/gamemanager.h"
#include "../include/gameticker.h"
#include "../include/runtimevariable.h"
#include "../include/timemanager.h"



namespace cure {



CppContextObject::CppContextObject(ResourceManager* resource_manager, const str& class_id):
	ContextObject(resource_manager, class_id),
	class_resource_(0),
	physics_resource_(0),
	allow_network_logic_(true),
	force_load_unique_(false) {
}

CppContextObject::~CppContextObject() {
	ClearPhysics();
	delete (physics_resource_);
	physics_resource_ = 0;
	delete (class_resource_);
	class_resource_ = 0;
}



tbc::ChunkyPhysics::GuideMode CppContextObject::GetGuideMode() const {
	if (GetPhysics()) {
		return GetPhysics()->GetGuideMode();
	}
	return tbc::ChunkyPhysics::kGuideExternal;
}

void CppContextObject::StabilizeTick() {
	const tbc::ChunkyPhysics* _physics = GetPhysics();
	const tbc::ChunkyClass* clazz = GetClass();
	if (!_physics || !clazz) {
		return;
	}
	bool is_physics_stopped;
	v_get(is_physics_stopped, =, GetSettings(), kRtvarPhysicsHalt, false);
	if (is_physics_stopped) {
		return;
	}
	if (_physics->GetPhysicsType() == tbc::ChunkyPhysics::kDynamic && _physics->GetGuideMode() >= tbc::ChunkyPhysics::kGuideExternal) {
		float stability_factor = 1;
		int body_index = 0;
		const float micro_step_factor = manager_->GetGameManager()->GetTimeManager()->GetDesiredMicroSteps() / 18.0f;
		for (size_t x = 0; x < clazz->GetTagCount(); ++x) {
			const tbc::ChunkyClass::Tag& tag = clazz->GetTag(x);
			if (tag.tag_name_ == "upright_stabilizer") {
				if (tag.float_value_list_.size() != 1 ||
					tag.string_value_list_.size() != 0 ||
					tag.body_index_list_.size() != 1 ||
					tag.engine_index_list_.size() != 0 ||
					tag.mesh_index_list_.size() != 0) {
					log_.Errorf("The upright_stabilizer tag '%s' has the wrong # of parameters.", tag.tag_name_.c_str());
					deb_assert(false);
					return;
				}
				stability_factor = tag.float_value_list_[0];
				body_index = tag.body_index_list_[0];
				tbc::PhysicsEngine::UprightStabilize(manager_->GetGameManager()->GetPhysicsManager(),
					_physics, _physics->GetBoneGeometry(body_index), GetMass()*stability_factor*micro_step_factor, 1);
			} else if (tag.tag_name_ == "forward_stabilizer") {
				if (tag.float_value_list_.size() != 1 ||
					tag.string_value_list_.size() != 0 ||
					tag.body_index_list_.size() != 1 ||
					tag.engine_index_list_.size() != 0 ||
					tag.mesh_index_list_.size() != 0) {
					log_.Errorf("The forward_stabilizer tag '%s' has the wrong # of parameters.", tag.tag_name_.c_str());
					deb_assert(false);
					return;
				}
				stability_factor = tag.float_value_list_[0];
				body_index = tag.body_index_list_[0];
				tbc::PhysicsEngine::ForwardStabilize(manager_->GetGameManager()->GetPhysicsManager(),
					_physics, _physics->GetBoneGeometry(body_index), GetMass()*stability_factor*micro_step_factor, 1);
			}
		}
	}
}



void CppContextObject::StartLoading() {
	deb_assert(class_resource_ == 0);
	class_resource_ = new UserClassResource();
	const str asset_name = GetClassId()+".class";
	class_resource_->Load(GetResourceManager(), asset_name,
		UserClassResource::TypeLoadCallback(this, &CppContextObject::OnLoadClass));
}



void CppContextObject::SetAllowNetworkLogic(bool allow) {
	allow_network_logic_ = allow;
}



tbc::ChunkyPhysics* CppContextObject::GetPhysics() const {
	if (physics_resource_ && physics_resource_->GetLoadState() == cure::kResourceLoadComplete) {
		return (physics_resource_->GetData());
	}
	return (0);
}

UserPhysicsReferenceResource* CppContextObject::GetPhysicsResource() const {
	return physics_resource_;
}

void CppContextObject::CreatePhysicsRef(const str& name) {
	StartLoadingPhysics(name);
}

const tbc::ChunkyClass* CppContextObject::GetClass() const {
	if (class_resource_->GetLoadState() == cure::kResourceLoadComplete) {
		return (class_resource_->GetRamData());
	}
	return (0);
}

const tbc::ChunkyClass::Tag* CppContextObject::FindTag(const str& tag_type, int float_value_count, int string_value_count, const std::vector<int>* trigger_index_array) const {
	const tbc::ChunkyClass* clazz = GetClass();
	for (size_t x = 0; x < clazz->GetTagCount(); ++x) {
		const tbc::ChunkyClass::Tag& tag = clazz->GetTag(x);
		if (tag.tag_name_ == tag_type &&
			(float_value_count < 0 || tag.float_value_list_.size() == (size_t)float_value_count) &&
			(string_value_count < 0 || tag.string_value_list_.size() == (size_t)string_value_count) &&
			(!trigger_index_array || (tag.body_index_list_.size() == trigger_index_array->size() &&
				std::equal(tag.body_index_list_.begin(), tag.body_index_list_.end(), trigger_index_array->begin())))) {
			return &tag;
		}
	}
	return 0;
}

void CppContextObject::SetTagIndex(int index) {
	(void)index;
}



void CppContextObject::SetForceLoadUnique(bool load_unique) {
	force_load_unique_ = load_unique;
}

void CppContextObject::StartLoadingPhysics(const str& physics_name) {
	deb_assert(physics_resource_ == 0);
	const str instance_id = strutil::IntToString(GetInstanceId(), 10);
	const str asset_name = physics_name + ".phys;" + instance_id.c_str();
	PhysicsSharedInitData init_data(position_.position_.transformation_, position_.position_.velocity_, physics_override_,
		((cure::GameTicker*)manager_->GetGameManager()->GetTicker())->GetPhysicsLock(), manager_->GetGameManager()->GetPhysicsManager(),
		manager_->GetGameManager()->GetTimeManager()->GetDesiredMicroSteps(), GetInstanceId());
	physics_resource_ = new UserPhysicsReferenceResource(init_data);
	if (!force_load_unique_) {
		physics_resource_->Load(GetResourceManager(), asset_name,
			UserPhysicsReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadPhysics), false);
	} else {
		physics_resource_->LoadUnique(GetResourceManager(), asset_name,
			UserPhysicsReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadPhysics));
	}
}

bool CppContextObject::TryComplete() {
	if (!physics_resource_ || IsLoaded()) {
		return (false);
	}

	// This is the way to post-process physics (create physical bodies) only after the rest have been created, so
	// that they are not created on different frames.
	if (physics_resource_->GetLoadState() == kResourceLoadComplete) {
		PhysicsSharedResource* _physics_resource = (PhysicsSharedResource*)physics_resource_->GetConstResource();
		if (_physics_resource->InjectPostProcess() == kResourceLoadComplete) {
			SetPhysics(physics_resource_->GetData());
			if (GetAllowNetworkLogic()) {
				SetupChildHandlers();
			}
		}
	}

	if (physics_resource_->GetLoadState() == kResourceLoadComplete) {
		for (Array::iterator x = child_array_.begin(); x != child_array_.end(); ++x) {
			CppContextObject* child = (CppContextObject*)*x;
			child->TryComplete();
		}
		if (GetPhysics() && GetPhysics()->GetEngineCount() > 0 && GetManager()) {
			GetManager()->EnableMicroTickCallback(this);	// Used for engine force applications each micro frame.
		}
		SetLoadResult(true);
		return (true);
	} else if (physics_resource_->GetLoadState() != kResourceLoadInProgress) {
		SetLoadResult(false);
		return (true);
	}
	return (false);
}

void CppContextObject::SetupChildHandlers() {
	Parent::SetupChildHandlers();

	if (!GetClass()) {
		return;
	}
	const int tag_count = GetClass()->GetTagCount();
	for (int x = 0; x < tag_count; ++x) {
		const tbc::ChunkyClass::Tag& tag = GetClass()->GetTag(x);
		CppContextObject* handler_child = (CppContextObject*)GetManager()->GetGameManager()->CreateLogicHandler(tag.tag_name_);
		if (!handler_child) {
			continue;
		}
		AddChild(handler_child);
		handler_child->SetTagIndex(x);
	}
}



void CppContextObject::OnMicroTick(float frame_time) {
	if (physics_ && GetManager()) {
		const bool needs_steering_help = (GetAttributeFloatValue("float_is_child") > 0.75f);
		int acc_index = GetPhysics()->GetEngineIndexFromControllerIndex(GetPhysics()->GetEngineCount()-1, -1, 0);
		const int turn_index = GetPhysics()->GetEngineIndexFromControllerIndex(0, 1, 1);
		if (needs_steering_help && acc_index >= 0 && turn_index >= 0) {
			// Young children have the possibility of just pressing left/right which will cause
			// a forward motion in the currently used vehicle.
			tbc::PhysicsEngine* acc = GetPhysics()->GetEngine(acc_index);
			const tbc::PhysicsEngine* turn = GetPhysics()->GetEngine(turn_index);
			const float power_fwd_rev = acc->GetValue();
			const float power_lr = turn->GetValue();
			float auto_turn_acc_value = 0;
			if (Math::IsEpsEqual(power_fwd_rev, 0.0f, 0.05f) && !Math::IsEpsEqual(power_lr, 0.0f, 0.05f)) {
				const float intensity = acc->GetIntensity();
				auto_turn_acc_value = Math::Clamp(10.0f*(0.2f-intensity), 0.0f, 1.0f);
			}
			// Throttle up all relevant acc engines.
			for (;;) {
				acc->ForceSetValue(tbc::PhysicsEngine::kAspectLocalPrimary, auto_turn_acc_value);
				acc_index = GetPhysics()->GetEngineIndexFromControllerIndex(acc_index-1, -1, 0);
				if (acc_index < 0) {
					break;
				}
				acc = GetPhysics()->GetEngine(acc_index);
			}
		}
		physics_->OnMicroTick(GetManager()->GetGameManager()->GetPhysicsManager(), frame_time);
	}
}

void CppContextObject::OnAlarm(int /*alarm_id*/, void* /*extra_data*/) {
}

void CppContextObject::OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal) {
	if (!GetAllowNetworkLogic()) {
		return;
	}

	ContextObject* child = (ContextObject*)GetTrigger(trigger_id);
	if (child) {
		child->OnTrigger(trigger_id, other_object, body_id, position, normal);
	} else {
		//log_.Errorf("Physical trigger not configured for logical trigging on %s.", GetClassId().c_str());
	}

	/*
	TODO: put this back when attaching objects to each other is working.
	ContextObject* object2 = manager_->GetGameManager()->GetPhysicsManager()->GetForceFeedbackListenerId(body2);
	if (manager_->GetGameManager()->IsServer() && object2) {
		AttachToObject(body1, object2, body2);
	}*/
}



void CppContextObject::OnForceApplied(ContextObject* other_object,
	tbc::PhysicsManager::BodyID own_body_id, tbc::PhysicsManager::BodyID other_body_id,
	const vec3& force, const vec3& torque,
	const vec3& position, const vec3& relative_velocity) {
	(void)position;
	(void)relative_velocity;

	if (!IsAttachedTo((ContextObject*)other_object)) {
		// TODO: replace by sensible values. Like dividing by mass, for instance.
		//if (force.GetLengthSquared() > 100 || torque.GetLengthSquared() > 10)
		{
			manager_->GetGameManager()->OnCollision(force, torque, position, this, (ContextObject*)other_object,
				own_body_id, other_body_id);
		}
	}
}



void CppContextObject::OnLoadClass(UserClassResource* class_resource) {
	tbc::ChunkyClass* clazz = class_resource->GetData();
	if (class_resource->GetLoadState() != cure::kResourceLoadComplete) {
		log_.Errorf("Could not load class '%s'.", class_resource->GetName().c_str());
		deb_assert(false);
		GetManager()->PostKillObject(GetInstanceId());
		return;
	} else {
		StartLoadingPhysics(clazz->GetPhysicsBaseName());
	}
}

void CppContextObject::OnLoadPhysics(UserPhysicsReferenceResource* physics_resource) {
	if (physics_resource->GetLoadState() != kResourceLoadComplete) {
		log_.Errorf("Could not load physics class '%s'.", physics_resource->GetName().c_str());
		deb_assert(false);
		GetManager()->PostKillObject(GetInstanceId());
		return;
	}

	TryComplete();
}



bool CppContextObject::GetAllowNetworkLogic() const {
	return allow_network_logic_;
}



loginstance(kGameContextCpp, CppContextObject);



}
