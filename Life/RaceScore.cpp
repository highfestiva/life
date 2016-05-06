
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "racescore.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/contextobject.h"
#include "../cure/include/gamemanager.h"
#include "../cure/include/timemanager.h"
#include "../lepra/include/lepraassert.h"
#include "../lepra/include/packer.h"



namespace life {



RaceScore::RaceScore(cure::ContextObject* context_object, const str& name, int lap_count, tbc::PhysicsManager::BodyID start_trigger):
	Parent(context_object, name),
	is_updated_(true),
	lap_count_left_(lap_count),
	start_trigger_(start_trigger),
	trigged_count_(1),
	trigger_count_(2) {
	physics_frame_start_ = context_object_->GetManager()->GetGameManager()->GetTimeManager()->GetCurrentPhysicsFrame();
	if (start_trigger_ != tbc::INVALID_BODY) {
		AddTriggered(start_trigger_);
	}

	SetNetworkType(kTypeServer);
}

RaceScore::~RaceScore() {
}



RaceScore::TriggerId RaceScore::GetStartTrigger() const {
	return start_trigger_;
}

bool RaceScore::IsTriggered(TriggerId trigger_id) const {
	TriggerSet::const_iterator x = trigger_set_.find(trigger_id);
	return (x != trigger_set_.end());
}

void RaceScore::AddTriggered(TriggerId trigger_id) {
	trigger_set_.insert(trigger_id);
	is_updated_ = true;
	context_object_->OnAttributeUpdated(this);
}

unsigned RaceScore::GetTriggedCount() const {
	return trigger_set_.size();
}

int RaceScore::StartNewLap() {
	if (--lap_count_left_ > 0) {
		trigger_set_.clear();
		trigger_set_.insert(start_trigger_);
		is_updated_ = true;
		context_object_->OnAttributeUpdated(this);
	}
	return lap_count_left_;
}

double RaceScore::GetTime() const {
	const int diff = context_object_->GetManager()->GetGameManager()->GetTimeManager()->GetCurrentPhysicsFrameDelta(physics_frame_start_);
	const float seconds = context_object_->GetManager()->GetGameManager()->GetTimeManager()->ConvertPhysicsFramesToSeconds(diff);
	if (seconds > 5*60) {
		context_object_->DeleteAttribute(name_);
	}
	return seconds;
}



int RaceScore::QuerySend() const {
	return is_updated_? Parent::QuerySend()+1+sizeof(uint16)+1+1 : 0;
}

int RaceScore::Pack(uint8* destination) {
	const int parent_size = Parent::Pack(destination);
	destination += parent_size;
	destination[0] = (uint8)lap_count_left_;
	PackerInt32::Pack(destination+1, physics_frame_start_);
	destination[5] = (uint8)trigger_set_.size();
	destination[6] = (uint8)trigger_count_;

	is_updated_ = false;

	return parent_size + 1+sizeof(int32)+1+1;
}

int RaceScore::Unpack(const uint8* source, int max_size) {
	if (max_size < 1+sizeof(uint16)+1+1) {
		return -1;
	}
	lap_count_left_ = source[0];
	PackerInt32::Unpack(physics_frame_start_, source+1, max_size);
	trigged_count_ = source[5];
	trigger_count_ = source[6];
	is_updated_ = true;
	return 1+sizeof(int32)+1+1;
}



void RaceScore::operator=(const RaceScore&) {
	deb_assert(false);
}



}
