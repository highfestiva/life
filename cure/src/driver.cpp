
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/driver.h"
#include "../include/contextmanager.h"
#include "../include/gamemanager.h"
#include "../include/timemanager.h"
#include "../../lepra/include/random.h"
#include "../../tbc/include/physicsengine.h"



namespace cure {



Driver::Driver(ContextManager* manager):
	CppContextObject(manager->GetGameManager()->GetResourceManager(), "Driver") {
	manager->AddLocalObject(this);
	GetManager()->EnableTickCallback(this);
}

Driver::~Driver() {
}



void Driver::SetTagIndex(int index) {
	tag_ = ((CppContextObject*)parent_)->GetClass()->GetTag(index);
	type_ = strutil::Split(tag_.tag_name_, ":")[1];
}

void Driver::OnTick() {
	Parent::OnTick();
	if (!parent_->GetPhysics()) {
		return;
	}

	for (size_t x = 0; x < tag_.engine_index_list_.size(); ++x) {
		const int engine_index = tag_.engine_index_list_[x];
		if (engine_index >= parent_->GetPhysics()->GetEngineCount()) {
			break;
		}
		tbc::PhysicsEngine* engine = parent_->GetPhysics()->GetEngine(engine_index);

		float f = 1;
		if (tag_.float_value_list_.size() > 0) {
			f = tag_.float_value_list_[0];
		}
		if (type_ == "cos") {
			deb_assert(tag_.float_value_list_.size() == 1);
			engine->SetValue(engine->GetControllerIndex(), (float)::cos(time_.QueryTimeDiff()*f));
			const double period = 2*PI/f;
			if (time_.GetTimeDiff() > period) {
				time_.ReduceTimeDiff(period);
			}
		} else if (type_ == "random_jerker") {
			deb_assert(tag_.float_value_list_.size() == 2);
			const float g = tag_.float_value_list_[1];
			const float t = (float)time_.QueryTimeDiff();
			if (t < f) {
				engine->SetValue(engine->GetControllerIndex()+0, 0);
				engine->SetValue(engine->GetControllerIndex()+1, 0);
				engine->SetValue(engine->GetControllerIndex()+3, 0);	// 3 for Z... Yup, I know!
			} else {
				vec3 push = RNDVEC(1.0f);
				push.x = ::pow(std::abs(push.x), 0.1f) * ((push.x <= 0)? -1 : +1);
				push.y = ::pow(std::abs(push.y), 0.1f) * ((push.y <= 0)? -1 : +1);
				push.z = ::pow(std::abs(push.z), 0.1f) * ((push.z <= 0)? -1 : +1);
				engine->SetValue(engine->GetControllerIndex()+0, push.x);
				engine->SetValue(engine->GetControllerIndex()+1, push.y);
				engine->SetValue(engine->GetControllerIndex()+3, push.z);	// 3 for Z... Yup, I know!
				if (t > f+g) {
					time_.ReduceTimeDiff(f+g);
				}
			}
		}
	}
}



loginstance(kGameContextCpp, Driver);




}
