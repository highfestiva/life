
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/spawner.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/gamemanager.h"
#include "../../lepra/include/random.h"
#include "../../tbc/include/physicsspawner.h"
#include "../include/runtimevariable.h"



namespace cure {



Spawner::Spawner(ContextManager* manager):
	CppContextObject(manager->GetGameManager()->GetResourceManager(), "Spawner"),
	spawn_point_index_(0) {
	manager->AddLocalObject(this);
	manager->EnableTickCallback(this);
	manager_->AddGameAlarmCallback(this, 0, 0.5f, 0);	// Create.
	manager_->AddGameAlarmCallback(this, 1, 0.6f, 0);	// Destroy.
	manager_->AddGameAlarmCallback(this, 2, 0.7f, 0);	// Recreate.
}

Spawner::~Spawner() {
	while (!child_array_.empty()) {
		const ContextObject* _object = child_array_.front();
		child_array_.erase(child_array_.begin());
		GetManager()->DeleteObject(_object->GetInstanceId());
	}
}



void Spawner::PlaceObject(ContextObject* object, int spawn_point_index) {
	const vec3 scale_point = RNDPOSVEC();
	if (spawn_point_index < 0) {
		spawn_point_index = Random::GetRandomNumber() % GetSpawner()->GetSpawnPointCount();
	}
	vec3 initial_velocity;
	object->SetInitialTransform(GetSpawner()->GetSpawnPoint(parent_->GetPhysics(), scale_point, spawn_point_index, initial_velocity));
	object->SetRootVelocity(initial_velocity);
}

xform Spawner::GetSpawnPoint() const {
	vec3 initial_velocity;
	return GetSpawner()->GetSpawnPoint(parent_->GetPhysics(), vec3(), 0, initial_velocity);
}

void Spawner::EaseDown(tbc::PhysicsManager* physics_manager, ContextObject* object, const vec3* start_position) {
	const cure::ObjectPositionalData* positional_data = 0;
	object->UpdateFullPosition(positional_data);
	cure::ObjectPositionalData* new_positional_data = (cure::ObjectPositionalData*)positional_data->Clone();
	if (start_position) {
		new_positional_data->position_.transformation_.SetPosition(*start_position);
	}
	/*new_positional_data->position_.acceleration_ = vec3();
	new_positional_data->position_.velocity_ = vec3();
	new_positional_data->position_.angular_acceleration_ = vec3();
	new_positional_data->position_.angular_velocity_ = vec3();*/
	bool has_touched_ground = false;
	float step = 1.0f;
	for (int x = 0; x < 20; ++x) {
		object->SetFullPosition(*new_positional_data, 0);
		const bool is_colliding = physics_manager->IsColliding(object->GetInstanceId());
		//log_.Infof("Spawned object %s %scolliding at step %i with step size %g.", object->GetClassId().c_str(), is_colliding? "" : "not ", x, step);
		if (step < 0.0001f && is_colliding) {
			break;
		}
		new_positional_data->position_.transformation_.GetPosition().z += is_colliding? +step : -step;
		has_touched_ground |= is_colliding;
		if (has_touched_ground) {
			step /= 2;
		}
	}
	object->SetFullPosition(*new_positional_data, 0);
	object->SetPositionFinalized();
	delete new_positional_data;
}



void Spawner::OnTick() {
	Parent::OnTick();

	for (GameObjectIdArray::iterator x = ease_down_objects_.begin(); x != ease_down_objects_.end(); ++x) {
		ContextObject* _object = GetManager()->GetObject(*x, true);
		if (!_object) {
			ease_down_objects_.erase(x);
			break;
		}
		if (_object->IsLoaded()) {
			EaseDown(GetManager()->GetGameManager()->GetPhysicsManager(), _object, 0);
			ease_down_objects_.erase(x);
			break;
		}
	}
}

void Spawner::OnAlarm(int alarm_id, void* extra_data) {
	Parent::OnAlarm(alarm_id, extra_data);

	const tbc::PhysicsSpawner::IntervalArray& intervals = GetSpawner()->GetIntervals();
	const size_t interval_count = intervals.size();
	if (interval_count < 2 || interval_count > 3) {
		log_.Error("Error: spawner has badly configured intervals!");
		deb_assert(false);
		return;
	}

	if (alarm_id == 0) {
		const bool _has_recreate = (interval_count >= 3 && intervals[2]);
		OnCreate(intervals[0], _has_recreate);
	} else if (alarm_id == 1) {
		OnDestroy(intervals[1]);
	} else if (interval_count >= 3) {
		OnRecreate(intervals[2]);
	}
}

void Spawner::OnCreate(float create_interval, bool has_recreate) {
	if (!create_interval) {
		return;
	}

	if ((int)child_array_.size() < GetSpawnCount()) {
		Create();
	}

	if (!has_recreate) {
		recreate_timer_.Stop();	// Make sure re-create starts over when we're done.
		GetManager()->AddGameAlarmCallback(this, 0, create_interval, 0);
	} else if (create_interval < 0 && (int)child_array_.size() < GetSpawnCount()) {
		recreate_timer_.Stop();	// Make sure re-create starts over when we're done.
		GetManager()->AddGameAlarmCallback(this, 0, -create_interval, 0);
	}
}

void Spawner::OnDestroy(float destroy_interval) {
	if (!destroy_interval) {
		return;
	}

	if (!child_array_.empty()) {
		if (destroy_interval > 0 || (int)child_array_.size() == GetSpawnCount()) {
			const ContextObject* _object = child_array_.front();
			child_array_.erase(child_array_.begin());
			GetManager()->DeleteObject(_object->GetInstanceId());
		}
	}
	GetManager()->AddGameAlarmCallback(this, 1, destroy_interval, 0);
}

void Spawner::OnRecreate(float recreate_interval) {
	if ((int)child_array_.size() < GetSpawnCount()) {
		recreate_timer_.TryStart();
		if (recreate_timer_.QueryTimeDiff() >= recreate_interval) {
			recreate_timer_.Stop();
			Create();
		}
	}
	GetManager()->AddGameAlarmCallback(this, 2, 0.5f, 0);
}

void Spawner::Create() {
	bool is_physics_halted;
	v_get(is_physics_halted, =, GetSettings(), kRtvarPhysicsHalt, false);
	if (is_physics_halted) {
		return;
	}
	const str spawn_object = GetSpawner()->GetSpawnObject(Random::Uniform(0.0f, 1.0f));
	if (!spawn_object.empty()) {
		ContextObject* _object = GetManager()->GetGameManager()->CreateContextObject(spawn_object, kNetworkObjectLocallyControlled);
		AddChild(_object);
		if (spawner_->IsEaseDown()) {
			ease_down_objects_.push_back(_object->GetInstanceId());
		}
		PlaceObject(_object, spawn_point_index_);
		if (++spawn_point_index_ >= GetSpawner()->GetSpawnPointCount()) {
			if (GetSpawner()->GetFunction() == "spawner_init") {
				deb_assert(GetSpawner()->GetSpawnPointCount() >= 2);
				spawn_point_index_ = 1;
			} else {
				spawn_point_index_ = 0;
			}
		}
		_object->StartLoading();
	}
}

int Spawner::GetSpawnCount() const {
	return (int)GetSpawner()->GetNumber();
}



loginstance(kGameContextCpp, Spawner);




}
