
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../thirdparty/FastDelegate/FastDelegate.h"
#include "../../lepra/include/unordered.h"
#include "../../lepra/include/hirestimer.h"
#include "../../lepra/include/idmanager.h"
#include "../../lepra/include/thread.h"
#include "../../tbc/include/physicsmanager.h"
#include "cure.h"



namespace cure {



class ContextObject;
class GameManager;
class TimeManager;



class ContextManager {
public:
	typedef std::unordered_map<GameObjectId, ContextObject*> ContextObjectTable;
	typedef fastdelegate::FastDelegate3<int,ContextObject*,void*,void> AlarmExternalCallback;

	ContextManager(GameManager* game_manager);
	virtual ~ContextManager();

	GameManager* GetGameManager() const;
	void SetLocalRange(unsigned index, unsigned count);	// Sets the range that will be used for local ID's.

	void SetIsObjectOwner(bool is_object_owner);
	void AddLocalObject(ContextObject* object);
	void AddObject(ContextObject* object);
	virtual void RemoveObject(ContextObject* object);
	bool DeleteObject(GameObjectId instance_id);
	void SetPostKillTimeout(double timeout);
	void PostKillObject(GameObjectId instance_id);
	void DelayKillObject(ContextObject* object, float seconds);
	ContextObject* GetObject(GameObjectId instance_id, bool force = false) const;
	const ContextObjectTable& GetObjectTable() const;
	void ClearObjects();
	void AddPhysicsSenderObject(ContextObject* object);
	void AddPhysicsBody(ContextObject* object, tbc::PhysicsManager::BodyID body_id);
	void RemovePhysicsBody(tbc::PhysicsManager::BodyID body_id);

	void AddAttributeSenderObject(ContextObject* object);
	void UnpackObjectAttribute(GameObjectId object_id, const uint8* data, unsigned size);

	GameObjectId AllocateGameObjectId(NetworkObjectType network_type);
	void FreeGameObjectId(NetworkObjectType network_type, GameObjectId instance_id);
	bool IsLocalGameObjectId(GameObjectId instance_id) const;

	void EnableTickCallback(ContextObject* object);
	void DisableTickCallback(ContextObject* object);
	void EnableMicroTickCallback(ContextObject* object);
	void DisableMicroTickCallback(ContextObject* object);
	void AddAlarmCallback(ContextObject* object, int alarm_id, float seconds, void* extra_data);
	void AddGameAlarmCallback(ContextObject* object, int alarm_id, float seconds, void* extra_data);	// Scale time by RTR.
	void AddAlarmExternalCallback(ContextObject* object, const AlarmExternalCallback& callback, int alarm_id, float seconds, void* extra_data);
	void CancelPendingAlarmCallbacksById(ContextObject* object, int alarm_id);
	void CancelPendingAlarmCallbacks(ContextObject* object);

	void MicroTick(float time_delta);
	void TickPhysics();
	void HandleIdledBodies();
	void HandlePhysicsSend();
	void HandleAttributeSend();
	void HandlePostKill();

private:
	typedef IdManager<GameObjectId> ObjectIdManager;
	struct GameObjectIdRecycleInfo {
		GameObjectIdRecycleInfo(GameObjectId instance_id, NetworkObjectType network_type):
			instance_id_(instance_id),
			network_type_(network_type) {
		}
		HiResTimer timer_;
		GameObjectId instance_id_;
		NetworkObjectType network_type_;
	};
	typedef std::vector<GameObjectIdRecycleInfo> RecycledIdQueue;
	struct Alarm {
		ContextObject* object_;
		AlarmExternalCallback callback_;
		int frame_time_;
		int alarm_id_;
		void* extra_data_;
		inline Alarm() {}
		inline Alarm(ContextObject* object, int frame_time, int alarm_id, void* extra_data):
			object_(object),
			frame_time_(frame_time),
			alarm_id_(alarm_id),
			extra_data_(extra_data) {
		}
		inline Alarm(ContextObject* object, const AlarmExternalCallback& callback, int frame_time, int alarm_id, void* extra_data):
			object_(object),
			callback_(callback),
			frame_time_(frame_time),
			alarm_id_(alarm_id),
			extra_data_(extra_data) {
		}
		inline bool operator<(const Alarm& other) const {
			return (object_ < other.object_ &&
				callback_ < other.callback_ &&
				frame_time_ < other.frame_time_ &&
				alarm_id_ < other.alarm_id_ &&
				extra_data_ < other.extra_data_);
		}
		inline bool operator==(const Alarm& other) const {
			return (object_ == other.object_ &&
				callback_ == other.callback_ &&
				frame_time_ == other.frame_time_ &&
				alarm_id_ == other.alarm_id_ &&
				extra_data_ == other.extra_data_);
		}
	};
	struct AlarmHasher {
		inline size_t operator()(const Alarm& alarm) const {
			const int f = alarm.callback_.empty()? 5527 : 1;
			return (alarm.frame_time_ + alarm.alarm_id_ + (size_t)alarm.object_) * f;
		}
	};
	typedef std::unordered_set<Alarm, AlarmHasher> AlarmSet;
	typedef std::unordered_map<tbc::PhysicsManager::BodyID, ContextObject*> BodyTable;
	typedef std::unordered_set<GameObjectId> IdSet;
	typedef BodyTable::value_type BodyPair;

	void OnDelayedDelete(int, ContextObject* object, void*);
	void DoAddAlarmCallback(Alarm& alarm, float seconds);
	void DispatchTickCallbacks();
	void DispatchMicroTickCallbacks(float frame_time_delta);
	void DispatchAlarmCallbacks();

	GameManager* game_manager_;

	bool is_object_owner_;
	RecycledIdQueue recycled_id_queue_;
	ObjectIdManager local_object_id_manager_;
	ObjectIdManager remote_object_id_manager_;
	ContextObjectTable object_table_;
	ContextObjectTable physics_sender_object_table_;
	BodyTable body_table_;
	ContextObjectTable attribute_sender_object_table_;
	ContextObjectTable tick_callback_object_table_;
	ContextObjectTable micro_tick_callback_object_table_;
	Lock alarm_mutex_;
	AlarmSet alarm_callback_object_set_;
	double max_post_kill_processing_time_;
	IdSet post_kill_set_;

	logclass();
};



}
