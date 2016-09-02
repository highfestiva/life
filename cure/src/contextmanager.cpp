
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/contextmanager.h"
#include <list>
#include "../include/contextobjectattribute.h"
#include "../include/contextobject.h"
#include "../include/gamemanager.h"
#include "../include/timemanager.h"



namespace cure {



#define CHECK_OBJ_ALARM_ERASED(obj)	LEPRA_DEBUG_CODE( for (AlarmSet::iterator _x = alarm_callback_object_set_.begin(); _x != alarm_callback_object_set_.end(); ++_x) { deb_assert(_x->object_ != obj); } )



ContextManager::ContextManager(GameManager* game_manager):
	game_manager_(game_manager),
	is_object_owner_(true),
	local_object_id_manager_(0x40000000, 0x7FFFFFFF-1, 0xFFFFFFFF),
	remote_object_id_manager_(1, 0x40000000-1, 0xFFFFFFFF),
	max_post_kill_processing_time_(0.01) {
}

ContextManager::~ContextManager() {
	ClearObjects();
	game_manager_ = 0;
}



GameManager* ContextManager::GetGameManager() const {
	return (game_manager_);
}

void ContextManager::SetLocalRange(unsigned index, unsigned count) {
	deb_assert(object_table_.empty());
	const GameObjectId start_id = 0x40000000;
	const GameObjectId block_size = (0x7FFFFFFF-start_id-count)/count;
	const GameObjectId offset = start_id + block_size*index;
	local_object_id_manager_ = ObjectIdManager(offset, offset+block_size-1, 0xFFFFFFFF);
}


void ContextManager::SetIsObjectOwner(bool is_object_owner) {
	is_object_owner_ = is_object_owner;
}

void ContextManager::AddLocalObject(ContextObject* object) {
	deb_assert(object->GetInstanceId() == 0);
	object->SetInstanceId(AllocateGameObjectId(kNetworkObjectLocalOnly));
	deb_assert(object->GetManager() == 0);
	object->SetManager(this);
	AddObject(object);
}

void ContextManager::AddObject(ContextObject* object) {
	deb_assert(object->GetInstanceId() != 0);
	deb_assert(object_table_.find(object->GetInstanceId()) == object_table_.end());
	deb_assert(object->GetManager() == this);
	object_table_.insert(ContextObjectTable::value_type(object->GetInstanceId(), object));
}

void ContextManager::RemoveObject(ContextObject* object) {
	deb_assert(Thread::GetCurrentThread()->GetThreadName() == "MainThread");
	CancelPendingAlarmCallbacks(object);
	DisableMicroTickCallback(object);
	DisableTickCallback(object);
	physics_sender_object_table_.erase(object->GetInstanceId());
	attribute_sender_object_table_.erase(object->GetInstanceId());
	object_table_.erase(object->GetInstanceId());
}

bool ContextManager::DeleteObject(GameObjectId instance_id) {
	bool ok = false;
	ContextObject* _object = GetObject(instance_id, true);
	if (_object) {
		log_volatile(log_.Tracef("Deleting context object %i.", instance_id));
		delete (_object);
		ok = true;
	} else {
		log_volatile(log_.Debugf("Could not delete context object %i, since not found.", instance_id));
	}
	return (ok);
}

void ContextManager::SetPostKillTimeout(double timeout) {
	max_post_kill_processing_time_ = timeout;
}

void ContextManager::PostKillObject(GameObjectId instance_id) {
	post_kill_set_.insert(instance_id);
}

void ContextManager::DelayKillObject(ContextObject* object, float seconds) {
	AddAlarmExternalCallback(object, AlarmExternalCallback(this, &ContextManager::OnDelayedDelete), 1, seconds, 0);
}

ContextObject* ContextManager::GetObject(GameObjectId instance_id, bool force) const {
	ContextObjectTable::const_iterator x = object_table_.find(instance_id);
	ContextObject* _object = 0;
	if (x != object_table_.end()) {
		_object = x->second;
		if (!force && !_object->IsLoaded()) {
			_object = 0;
		}
	}
	return (_object);
}

const ContextManager::ContextObjectTable& ContextManager::GetObjectTable() const {
	return (object_table_);
}

void ContextManager::ClearObjects() {
	physics_sender_object_table_.clear();
	attribute_sender_object_table_.clear();
	while (object_table_.size() > 0) {
		ContextObject* _object = object_table_.begin()->second;
		if (is_object_owner_) {
			delete (_object);
		} else {
			_object->SetManager(0);
			object_table_.erase(object_table_.begin());
		}
	}
}

void ContextManager::AddPhysicsSenderObject(ContextObject* object) {
	deb_assert(object->GetInstanceId() != 0);
	deb_assert(object_table_.find(object->GetInstanceId()) != object_table_.end());
	deb_assert(object->GetManager() == this);
	deb_assert(object->GetManager()->GetGameManager()->GetTickLock()->IsOwner());
	physics_sender_object_table_.insert(ContextObjectTable::value_type(object->GetInstanceId(), object));
}

void ContextManager::AddPhysicsBody(ContextObject* object, tbc::PhysicsManager::BodyID body_id) {
	body_table_.insert(BodyPair(body_id, object));
}

void ContextManager::RemovePhysicsBody(tbc::PhysicsManager::BodyID body_id) {
	if (body_id != tbc::INVALID_BODY) {
		body_table_.erase(body_id);
	}
}

void ContextManager::AddAttributeSenderObject(ContextObject* object) {
	deb_assert(object->GetInstanceId() != 0);
	deb_assert(object_table_.find(object->GetInstanceId()) != object_table_.end());
	deb_assert(object->GetManager() == this);
	attribute_sender_object_table_.insert(ContextObjectTable::value_type(object->GetInstanceId(), object));
}

void ContextManager::UnpackObjectAttribute(GameObjectId object_id, const uint8* data, unsigned _size) {
	ContextObject* _object = GetObject(object_id, true);
	if (_object) {
		ContextObjectAttribute::Unpack(_object, data, _size);
	} else {
		log_volatile(log_.Debugf("Trying to unpack attribute for non-existent object %u.", object_id));
	}
}



GameObjectId ContextManager::AllocateGameObjectId(NetworkObjectType network_type) {
	ScopeLock lock(&id_lock_);
	GameObjectId _instance_id;
	if (network_type == kNetworkObjectLocalOnly) {
		_instance_id = local_object_id_manager_.GetFreeId();
	} else {
		_instance_id = remote_object_id_manager_.GetFreeId();
	}
	return (_instance_id);
}

void ContextManager::FreeGameObjectId(NetworkObjectType network_type, GameObjectId instance_id) {
	recycled_id_queue_.push_back(GameObjectIdRecycleInfo(instance_id, network_type));
}

bool ContextManager::IsLocalGameObjectId(GameObjectId instance_id) const {
	ScopeLock lock(&id_lock_);
	return instance_id >= local_object_id_manager_.GetMinId();
}



void ContextManager::EnableTickCallback(ContextObject* object) {
	deb_assert(object->GetInstanceId());
	tick_callback_object_table_.insert(ContextObjectTable::value_type(object->GetInstanceId(), object));
}

void ContextManager::DisableTickCallback(ContextObject* object) {
	tick_callback_object_table_.erase(object->GetInstanceId());
}

void ContextManager::EnableMicroTickCallback(ContextObject* object) {
	if (object->GetNetworkObjectType() == kNetworkObjectLocalOnly || GetGameManager()->IsPrimaryManager()) {
		micro_tick_callback_object_table_.insert(ContextObjectTable::value_type(object->GetInstanceId(), object));
	}
}

void ContextManager::DisableMicroTickCallback(ContextObject* object) {
	micro_tick_callback_object_table_.erase(object->GetInstanceId());
}

void ContextManager::AddAlarmCallback(ContextObject* object, int alarm_id, float seconds, void* extra_data) {
	Alarm _alarm(object, 0, alarm_id, extra_data);
	DoAddAlarmCallback(_alarm, seconds);
}

void ContextManager::AddGameAlarmCallback(ContextObject* object, int alarm_id, float seconds, void* extra_data) {
	seconds /= ((const GameManager*)game_manager_)->GetTimeManager()->GetRealTimeRatio();
	AddAlarmCallback(object, alarm_id, seconds, extra_data);
}

void ContextManager::AddAlarmExternalCallback(ContextObject* object, const AlarmExternalCallback& callback, int alarm_id, float seconds, void* extra_data) {
	Alarm _alarm(object, callback, 0, alarm_id, extra_data);
	DoAddAlarmCallback(_alarm, seconds);
}

void ContextManager::CancelPendingAlarmCallbacksById(ContextObject* object, int alarm_id) {
	ScopeLock lock(&alarm_mutex_);
	AlarmSet::iterator x = alarm_callback_object_set_.begin();
	while (x != alarm_callback_object_set_.end()) {
		if (x->object_ == object && x->alarm_id_ == alarm_id) {
			alarm_callback_object_set_.erase(x);
			x = alarm_callback_object_set_.begin();	// Shouldn't be necessary, but is!
		} else {
			++x;
		}
	}
}

void ContextManager::CancelPendingAlarmCallbacks(ContextObject* object) {
	deb_assert(Thread::GetCurrentThread()->GetThreadName() == "MainThread");

	ScopeLock lock(&alarm_mutex_);
	AlarmSet::iterator x = alarm_callback_object_set_.begin();
	while (x != alarm_callback_object_set_.end()) {
		if (x->object_ == object) {
			alarm_callback_object_set_.erase(x);
			x = alarm_callback_object_set_.begin();	// Shouldn't be necessary, but is!
		} else {
			++x;
		}
	}

	CHECK_OBJ_ALARM_ERASED(object);
}



void ContextManager::MicroTick(float time_delta) {
	DispatchMicroTickCallbacks(time_delta);
}

void ContextManager::TickPhysics() {
	DispatchTickCallbacks();
	DispatchAlarmCallbacks();
}

void ContextManager::HandleIdledBodies() {
	typedef tbc::PhysicsManager::BodySet BodySet;
	const BodySet& body_set = game_manager_->GetPhysicsManager()->GetIdledBodies();
	BodySet::const_iterator x = body_set.begin();
	for (; x != body_set.end(); ++x) {
		BodyTable::iterator y = body_table_.find(*x);
		if (y != body_table_.end()) {
			game_manager_->OnStopped(y->second, y->first);
		}
		/*else {
			log_.Error("Body not present in body table!");
		}*/
	}
}

void ContextManager::HandlePhysicsSend() {
	ContextObjectTable::iterator x = physics_sender_object_table_.begin();
	while (x != physics_sender_object_table_.end()) {
		if (game_manager_->OnPhysicsSend(x->second)) {
			physics_sender_object_table_.erase(x++);
		} else {
			++x;
		}
	}
}

void ContextManager::HandleAttributeSend() {
	ContextObjectTable::iterator x = attribute_sender_object_table_.begin();
	while (x != attribute_sender_object_table_.end()) {
		if (game_manager_->OnAttributeSend(x->second)) {
			attribute_sender_object_table_.erase(x++);
		} else {
			++x;
		}
	}
}

void ContextManager::HandlePostKill() {
	HiResTimer timer(false);
	while (!post_kill_set_.empty()) {
		game_manager_->DeleteContextObject(*post_kill_set_.begin());
		post_kill_set_.erase(post_kill_set_.begin());
		double delta = timer.QueryTimeDiff();
		if (delta > max_post_kill_processing_time_) {	// Time's up, have a go later.
			break;
		}
	}

	ScopeLock lock(&id_lock_);
	RecycledIdQueue::iterator y = recycled_id_queue_.begin();
	while (y != recycled_id_queue_.end()) {
		if (y->timer_.QueryTimeDiff() < 10.0) {
			break;
		}
		if (y->network_type_ == kNetworkObjectLocalOnly) {
			local_object_id_manager_.RecycleId(y->instance_id_);
		} else {
			remote_object_id_manager_.RecycleId(y->instance_id_);
		}
		y = recycled_id_queue_.erase(y);
	}
}



void ContextManager::DoAddAlarmCallback(Alarm& alarm, float seconds) {
	deb_assert(alarm.object_->GetInstanceId() != 0);
	deb_assert(alarm.object_->GetManager() == this);
	deb_assert(GetObject(alarm.object_->GetInstanceId(), true) == alarm.object_);

	const TimeManager* time = ((const GameManager*)game_manager_)->GetTimeManager();
	alarm.frame_time_ = time->GetCurrentPhysicsFrameAddSeconds(seconds);
	ScopeLock lock(&alarm_mutex_);
	alarm_callback_object_set_.insert(alarm);
}

void ContextManager::OnDelayedDelete(int, ContextObject* object, void*) {
	PostKillObject(object->GetInstanceId());
}

void ContextManager::DispatchTickCallbacks() {
	ContextObjectTable::iterator x = tick_callback_object_table_.begin();
	for (; x != tick_callback_object_table_.end(); ++x) {
		ContextObject* _object = x->second;
		_object->OnTick();
	}
}

void ContextManager::DispatchMicroTickCallbacks(float time_delta) {
	ContextObjectTable::iterator x = micro_tick_callback_object_table_.begin();
	for (; x != micro_tick_callback_object_table_.end(); ++x) {
		x->second->OnMicroTick(time_delta);
	}
}

void ContextManager::DispatchAlarmCallbacks() {
	// Divide dispatch into two parts to avoid callbacks messing up the skiplist:
	// 1. Extract due alarms into list.
	// 2. Callback alarms.

	ScopeLock lock(&alarm_mutex_);

	std::list<Alarm> callback_list;
	AlarmSet::iterator x = alarm_callback_object_set_.begin();
	while (x != alarm_callback_object_set_.end()) {
		if (game_manager_->GetTimeManager()->GetCurrentPhysicsFrameDelta(x->frame_time_) >= 0) {
			deb_assert(!x->object_->GetClassId().empty());
			callback_list.push_back(*x);
			alarm_callback_object_set_.erase(x);
			x = alarm_callback_object_set_.begin();	// Shouldn't be necessary, but is!
		} else {
			++x;
		}
	}

	// Callback alarms.
	for (std::list<Alarm>::iterator y = callback_list.begin(); y != callback_list.end(); ++y) {
		const Alarm& _alarm = *y;
		if (_alarm.callback_.empty()) {
			_alarm.object_->OnAlarm(_alarm.alarm_id_, _alarm.extra_data_);
		} else {
			_alarm.callback_(_alarm.alarm_id_, _alarm.object_, _alarm.extra_data_);
		}
	}
}



loginstance(kGameContext, ContextManager);



}
