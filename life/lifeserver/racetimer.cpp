
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "racetimer.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/gamemanager.h"
#include "../../cure/include/timemanager.h"
#include "../../tbc/include/chunkyphysics.h"
#include "../../tbc/include/physicstrigger.h"
#include "../racescore.h"



namespace life {



RaceTimer::RaceTimer(cure::ContextManager* manager):
	cure::CppContextObject(manager->GetGameManager()->GetResourceManager(), "RaceTimer"),
	trigger_count_(0) {
	manager->AddLocalObject(this);
	attribute_name_ = strutil::Format("race_timer_%u", GetInstanceId());
	GetManager()->EnableTickCallback(this);
}

RaceTimer::~RaceTimer() {
}



void RaceTimer::FinalizeTrigger(const tbc::PhysicsTrigger* trigger) {
	std::vector<int> trigger_index_array;
	const tbc::ChunkyPhysics* physics = parent_->GetPhysics();
	trigger_count_ = trigger->GetTriggerGeometryCount();
	for (int x = 0; x < (int)trigger_count_; ++x) {
		const int bone_index = physics->GetIndex(trigger->GetTriggerGeometry(x));
		deb_assert(bone_index >= 0);
		trigger_index_array.push_back(bone_index);
	}
	const tbc::ChunkyClass::Tag* tag = ((CppContextObject*)parent_)->FindTag("race_trigger_data", 0, 0, &trigger_index_array);
	deb_assert(tag);
	if (tag) {
	}
}

void RaceTimer::OnTick() {
	const cure::TimeManager* time_manager = GetManager()->GetGameManager()->GetTimeManager();
	DoneMap::iterator x = done_map_.begin();
	while (x != done_map_.end()) {
		if (time_manager->GetCurrentPhysicsFrameDelta(x->second) >= 0) {
			cure::ContextObject* object = GetManager()->GetObject(x->first);
			if (object) {
				object->DeleteAttribute(attribute_name_);
			}
			done_map_.erase(x++);
		} else {
			++x;
		}
	}
}

void RaceTimer::OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal) {
	(void)body_id;
	(void)position;
	(void)normal;

	cure::ContextObject* object = other_object;
	if (object->GetPhysics()->GetEngineCount() <= 0) {	// Only self-movable stuff can let the games begin!
		return;
	}

	// Check if just finished this race.
	DoneMap::iterator i = done_map_.find(object->GetInstanceId());
	if (i != done_map_.end()) {
		return;
	}

	RaceScore* race_score = (RaceScore*)object->GetAttribute(attribute_name_);
	if (!race_score) {
		race_score = new RaceScore(object, attribute_name_, 1, trigger_id);
		log_.Headline("Race started!");
	}
	if (!race_score->IsTriggered(trigger_id)) {
		race_score->AddTriggered(trigger_id);
		log_.Infof("Hit %u/%u triggers in %f s.", race_score->GetTriggedCount(), trigger_count_, race_score->GetTime());
	} else if (race_score->GetTriggedCount() == trigger_count_ && trigger_id == race_score->GetStartTrigger()) {
		if (race_score->StartNewLap() <= 0) {
			log_.Headlinef("Congratulations - finished race in %f s!", race_score->GetTime());
			done_map_.insert(DoneMap::value_type(object->GetInstanceId(), GetManager()->GetGameManager()->GetTimeManager()->GetCurrentPhysicsFrameAddSeconds(5.0)));
		} else {
			log_.Headlinef("Lap completed; time is %f s!", race_score->GetTime());
		}
	}
}



loginstance(kGameContextCpp, RaceTimer);




}
