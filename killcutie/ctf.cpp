
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "ctf.h"
#include "../cure/include/cppcontextobject.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/runtimevariable.h"
#include "../tbc/include/physicstrigger.h"
#include "../uicure/include/uicppcontextobject.h"



namespace grenaderun {



Ctf::Ctf(cure::ContextManager* manager):
	Parent(manager->GetGameManager()->GetResourceManager(), "Ctf"),
	trigger_(0),
	last_frame_triggered_(false),
	is_trigger_timer_started_(false),
	flag_mesh_(0),
	slide_down_(false),
	blink_time_(0) {
	manager->AddLocalObject(this);
	GetManager()->EnableTickCallback(this);
}

Ctf::~Ctf() {
}



vec3 Ctf::GetPosition() const {
	const tbc::ChunkyBoneGeometry* cutie_goal = trigger_->GetTriggerGeometry(0);
	return GetManager()->GetGameManager()->GetPhysicsManager()->GetBodyPosition(cutie_goal->GetBodyId());
}

float Ctf::GetCaptureLevel() const {
	return 1 - (flag_top_ - flag_mesh_->GetOffsetTransformation().GetPosition()).GetLength() / flag_offset_.GetLength();
}

void Ctf::StartSlideDown() {
	slide_down_ = true;
	catching_flag_velocity_ = start_flag_velocity_;
}


void Ctf::FinalizeTrigger(const tbc::PhysicsTrigger* trigger) {
	trigger_ = trigger;
	UiCure::CppContextObject* parent = (UiCure::CppContextObject*)parent_;
	const tbc::ChunkyClass::Tag* tag = parent->FindTag("stunt_trigger_data", 4, 0);
	deb_assert(tag && tag->mesh_index_list_.size() == 2);
	if (tag && tag->mesh_index_list_.size() == 2) {
		flag_offset_.x		= tag->float_value_list_[0];
		flag_offset_.y		= tag->float_value_list_[1];
		flag_offset_.z		= tag->float_value_list_[2];
		catching_flag_velocity_	= -flag_offset_ / tag->float_value_list_[3];
		start_flag_velocity_	= catching_flag_velocity_;
	}
}

void Ctf::OnTick() {
	if (!last_frame_triggered_) {
		is_trigger_timer_started_ = false;
	}

	UiCure::CppContextObject* parent = (UiCure::CppContextObject*)parent_;
	if (!flag_mesh_ || !blink_mesh_) {
		const tbc::ChunkyClass::Tag* tag = parent->FindTag("stunt_trigger_data", 4, 0);
		flag_mesh_ = (tbc::GeometryReference*)parent->GetMesh(tag->mesh_index_list_[0]);
		blink_mesh_ = (tbc::GeometryReference*)parent->GetMesh(tag->mesh_index_list_[1]);
		if (!flag_mesh_ || !blink_mesh_) {
			return;
		}
		flag_top_ = flag_mesh_->GetOffsetTransformation().GetPosition();
		flag_mesh_->AddOffset(flag_offset_);
		blink_start_color_ = blink_mesh_->GetBasicMaterialSettings().diffuse_;
	}
	//else
	//{
	//	parent->EnableMeshSlide(false);
	//}

	// Move flag up or down...
	Game* game = (Game*)GetManager()->GetGameManager();
	float factor = 1.0f / kFps;
	if (is_trigger_timer_started_ && !slide_down_) {
		// Move up or stop if reached top.
		if (game->GetComputerIndex() == 0) {
			const float t = game->GetComputerDifficulty();
			if (t >= 0 && t < 0.5f) {
				factor *= Math::Lerp(0.4f, 1.0f, t*2);
			}
		}
		float real_time_ratio;
		v_get(real_time_ratio, =(float), cure::GetSettings(), kRtvarPhysicsRtr, 1.0);
		factor *= real_time_ratio;
		flag_mesh_->AddOffset(catching_flag_velocity_ * factor);
		if ((flag_top_ - flag_mesh_->GetOffsetTransformation().GetPosition()).Dot(catching_flag_velocity_) <= 0) {
			catching_flag_velocity_.Set(0, 0, 0);
			game->OnCapture();
		}
		blink_time_ += real_time_ratio * 0.05f;
		const float r = -::cos(blink_time_*3)*0.5f + 0.5f;
		const float g = -::cos(blink_time_*4)*0.5f + 0.5f;
		const float b = -::cos(blink_time_*5)*0.5f + 0.5f;
		blink_mesh_->GetBasicMaterialSettings().diffuse_.Set(r, g, b);
	} else {
		blink_mesh_->GetBasicMaterialSettings().diffuse_ = blink_start_color_;
		blink_time_ = 0;
		// Move down if not at bottom.
		if (slide_down_ && (flag_offset_ - (flag_mesh_->GetOffsetTransformation().GetPosition() - flag_top_)).Dot(flag_offset_) > 0) {
			flag_mesh_->AddOffset(catching_flag_velocity_ * -factor);
		} else {
			slide_down_ = false;
		}
	}

	last_frame_triggered_ = false;
	trigger_timer_.UpdateTimer();
}

void Ctf::OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal) {
	(void)trigger_id;
	(void)other_object;
	(void)body_id;
	(void)normal;

	last_frame_triggered_ = true;
	if (!is_trigger_timer_started_) {
		trigger_timer_.PopTimeDiff();
		is_trigger_timer_started_ = true;
	}
}



loginstance(kGameContextCpp, Ctf);




}
