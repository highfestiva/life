
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <hash_map>
#include "../../Lepra/Include/IdManager.h"
#include "../../TBC/Include/PhysicsManager.h"
#include "Cure.h"



namespace Cure
{



class ContextObject;
class GameManager;
class TimeManager;



class ContextManager
{
public:
	typedef std::hash_map<GameObjectId, ContextObject*> ContextObjectTable;
	enum
	{
		SYSTEM_ALARM_ID = 1000,
	};

	ContextManager(GameManager* pGameManager);
	virtual ~ContextManager();

	GameManager* GetGameManager() const;

	void SetIsObjectOwner(bool pIsObjectOwner);
	void AddLocalObject(ContextObject* pObject);
	void AddObject(ContextObject* pObject);
	virtual void RemoveObject(ContextObject* pObject);
	bool DeleteObject(GameObjectId pInstanceId);
	ContextObject* GetObject(GameObjectId pInstanceId, bool pForce = false) const;
	const ContextObjectTable& GetObjectTable() const;
	void ClearObjects();
	void AddPhysicsSenderObject(ContextObject* pObject);
	void AddPhysicsBody(ContextObject* pObject, TBC::PhysicsManager::BodyID pBodyId);
	void RemovePhysicsBody(TBC::PhysicsManager::BodyID pBodyId);

	GameObjectId AllocateGameObjectId(NetworkObjectType pNetworkType);
	void FreeGameObjectId(NetworkObjectType pNetworkType, GameObjectId pInstanceId);
	bool IsLocalGameObjectId(GameObjectId pInstanceId) const;

	void EnablePhysicsUpdateCallback(ContextObject* pObject);
	void DisablePhysicsUpdateCallback(ContextObject* pObject);
	void EnableTickCallback(ContextObject* pObject);
	void DisableTickCallback(ContextObject* pObject);
	void AddAlarmCallback(ContextObject* pObject, int pAlarmId, float pSeconds, void* pExtraData);
	void CancelPendingAlarmCallbacksById(ContextObject* pObject, int pAlarmId);
	void CancelPendingAlarmCallbacks(ContextObject* pObject);

	void Tick(float pTimeDelta);
	void TickPhysics();
	void HandleIdledBodies();
	void HandlePhysicsSend();

private:
	void DispatchPhysicsUpdateCallbacks();
	void DispatchTickCallbacks(float pFrameTimeDelta);
	void DispatchAlarmCallbacks();

	typedef IdManager<GameObjectId> ObjectIdManager;
	typedef std::pair<GameObjectId, ContextObject*> ContextObjectPair;
	struct Alarm
	{
		ContextObject* mObject;
		int mFrameTime;
		int mAlarmId;
		void* mExtraData;
		inline Alarm() {}
		inline Alarm(ContextObject* pObject, int pFrameTime, int pAlarmId, void* pExtraData):
			mObject(pObject),
			mFrameTime(pFrameTime),
			mAlarmId(pAlarmId),
			mExtraData(pExtraData)
		{
		}
		inline bool operator<(const Alarm& pOther) const
		{
			return (mObject < pOther.mObject &&
				mFrameTime < pOther.mFrameTime &&
				mAlarmId < pOther.mAlarmId &&
				mExtraData < pOther.mExtraData);
		}
		inline bool operator==(const Alarm& pOther) const
		{
			return (mObject == pOther.mObject &&
				mFrameTime == pOther.mFrameTime &&
				mAlarmId == pOther.mAlarmId &&
				mExtraData == pOther.mExtraData);
		}
	};
	struct AlarmHasher
	{
		inline size_t operator()(const Alarm& pAlarm) const
		{
			return (pAlarm.mFrameTime + pAlarm.mAlarmId +
				(size_t)pAlarm.mObject);
		}
	};
	typedef std::hash_set<Alarm, AlarmHasher> AlarmSet;
	typedef std::hash_map<TBC::PhysicsManager::BodyID, ContextObject*> BodyTable;
	typedef BodyTable::value_type BodyPair;

	GameManager* mGameManager;

	bool mIsObjectOwner;
	ObjectIdManager mLocalObjectIdManager;
	ObjectIdManager mRemoteObjectIdManager;
	ContextObjectTable mObjectTable;
	ContextObjectTable mPhysicsSenderObjectTable;
	BodyTable mBodyTable;
	ContextObjectTable mPhysicsUpdateCallbackObjectTable;
	ContextObjectTable mTickCallbackObjectTable;
	AlarmSet mAlarmCallbackObjectSet;

	LOG_CLASS_DECLARE();
};



}
