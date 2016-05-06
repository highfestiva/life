
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "stunttrigger.h"
#include <algorithm>
#include "../cure/include/contextmanager.h"
#include "../cure/include/gamemanager.h"
#include "../tbc/include/chunkyphysics.h"
#include "../tbc/include/physicstrigger.h"



namespace life {



StuntTrigger::StuntTrigger(cure::ContextManager* manager, const str& class_id):
	cure::CppContextObject(manager->GetGameManager()->GetResourceManager(), class_id),
	allow_bullet_time_(true),
	last_frame_triggered_(false),
	min_speed_(0),
	max_speed_(-1),
	min_time_(-1),
	real_time_ratio_(-1),
	bullet_time_duration_(0) {
	manager->AddLocalObject(this);
	GetManager()->EnableTickCallback(this);
}

StuntTrigger::~StuntTrigger() {
}



void StuntTrigger::FinalizeTrigger(const tbc::PhysicsTrigger* trigger) {
	std::vector<int> trigger_index_array;
	const tbc::ChunkyPhysics* physics = parent_->GetPhysics();
	const int bone_count = trigger->GetTriggerGeometryCount();
	for (int x = 0; x < bone_count; ++x) {
		const int bone_index = physics->GetIndex(trigger->GetTriggerGeometry(x));
		deb_assert(bone_index >= 0);
		trigger_index_array.push_back(bone_index);
	}
	const tbc::ChunkyClass::Tag* tag = ((CppContextObject*)parent_)->FindTag("stunt_trigger_data", 5, 2, &trigger_index_array);
	deb_assert(tag);
	if (tag) {
		min_speed_		= tag->float_value_list_[0];
		max_speed_		= tag->float_value_list_[1];
		min_time_		= tag->float_value_list_[2];
		real_time_ratio_		= tag->float_value_list_[3];
		bullet_time_duration_	= tag->float_value_list_[4];
		client_start_command_	= tag->string_value_list_[0];
		client_stop_command_	= tag->string_value_list_[1];
	}
}

void StuntTrigger::OnTick() {
	if (!last_frame_triggered_) {
		trigger_timer_.Stop();
	}
	last_frame_triggered_ = false;
}

void StuntTrigger::OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal) {
	(void)trigger_id;
	(void)body_id;
	(void)position;
	(void)normal;

	if (!allow_bullet_time_) {
		return;
	}

	last_frame_triggered_ = true;

	ContextObject* object = other_object;
	const float speed = object->GetVelocity().GetLength();
	if (speed < min_speed_ || speed > max_speed_) {
		trigger_timer_.Stop();
		return;
	}
	trigger_timer_.TryStart();
	if (trigger_timer_.QueryTimeDiff() < min_time_) {
		return;
	}

	allow_bullet_time_ = false;
	DidTrigger(object);
}



loginstance(kGameContextCpp, StuntTrigger);




}
