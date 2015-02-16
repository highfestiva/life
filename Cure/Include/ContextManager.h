
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../ThirdParty/FastDelegate/FastDelegate.h"
#include "../../Lepra/Include/Unordered.h"
#include "../../Lepra/Include/HiResTimer.h"
#include "../../Lepra/Include/IdManager.h"
#include "../../Lepra/Include/Thread.h"
#include "../../Tbc/Include/PhysicsManager.h"
#include "Cure.h"



namespace Cure
{



class ContextObject;
class GameManager;
class TimeManager;



class ContextManager
{
public:
	typedef std::unordered_map<GameObjectId, ContextObject*> ContextObjectTable;
	typedef fastdelegate::FastDelegate3<int,ContextObject*,void*,void> AlarmExternalCallback;

	ContextManager(GameManager* pGameManager);
	virtual ~ContextManager();

	GameManager* GetGameManager() const;
	void SetLocalRange(unsigned pIndex, unsigned pCount);	// Sets the range that will be used for local ID's.

	void SetIsObjectOwner(bool pIsObjectOwner);
	void AddLocalObject(ContextObject* pObject);
	void AddObject(ContextObject* pObject);
	virtual void RemoveObject(ContextObject* pObject);
	bool DeleteObject(GameObjectId pInstanceId);
	void SetPostKillTimeout(double pTimeout);
	void PostKillObject(GameObjectId pInstanceId);
	void DelayKillObject(ContextObject* pObject, float pSeconds);
	ContextObject* GetObject(GameObjectId pInstanceId, bool pForce = false) const;
	const ContextObjectTable& GetObjectTable() const;
	void ClearObjects();
	void AddPhysicsSenderObject(ContextObject* pObject);
	void AddPhysicsBody(ContextObject* pObject, Tbc::PhysicsManager::BodyID pBodyId);
	void RemovePhysicsBody(Tbc::PhysicsManager::BodyID pBodyId);

	void AddAttributeSenderObject(ContextObject* pObject);
	void UnpackObjectAttribute(GameObjectId pObjectId, const uint8* pData, unsigned pSize);

	GameObjectId AllocateGameObjectId(NetworkObjectType pNetworkType);
	void FreeGameObjectId(NetworkObjectType pNetworkType, GameObjectId pInstanceId);
	bool IsLocalGameObjectId(GameObjectId pInstanceId) const;

	void EnableTickCallback(ContextObject* pObject);
	void DisableTickCallback(ContextObject* pObject);
	void EnableMicroTickCallback(ContextObject* pObject);
	void DisableMicroTickCallback(ContextObject* pObject);
	void AddAlarmCallback(ContextObject* pObject, int pAlarmId, float pSeconds, void* pExtraData);
	void AddGameAlarmCallback(ContextObject* pObject, int pAlarmId, float pSeconds, void* pExtraData);	// Scale time by RTR.
	void AddAlarmExternalCallback(ContextObject* pObject, const AlarmExternalCallback& pCallback, int pAlarmId, float pSeconds, void* pExtraData);
	void CancelPendingAlarmCallbacksById(ContextObject* pObject, int pAlarmId);
	void CancelPendingAlarmCallbacks(ContextObject* pObject);

	void MicroTick(float pTimeDelta);
	void TickPhysics();
	void HandleIdledBodies();
	void HandlePhysicsSend();
	void HandleAttributeSend();
	void HandlePostKill();

private:
	typedef IdManager<GameObjectId> ObjectIdManager;
	struct GameObjectIdRecycleInfo
	{
		GameObjectIdRecycleInfo(GameObjectId pInstanceId, NetworkObjectType pNetworkType):
			mInstanceId(pInstanceId),
			mNetworkType(pNetworkType)
		{
		}
		HiResTimer mTimer;
		GameObjectId mInstanceId;
		NetworkObjectType mNetworkType;
	};
	typedef std::vector<GameObjectIdRecycleInfo> RecycledIdQueue;
	struct Alarm
	{
		ContextObject* mObject;
		AlarmExternalCallback mCallback;
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
		inline Alarm(ContextObject* pObject, const AlarmExternalCallback& pCallback, int pFrameTime, int pAlarmId, void* pExtraData):
			mObject(pObject),
			mCallback(pCallback),
			mFrameTime(pFrameTime),
			mAlarmId(pAlarmId),
			mExtraData(pExtraData)
		{
		}
		inline bool operator<(const Alarm& pOther) const
		{
			return (mObject < pOther.mObject &&
				mCallback < pOther.mCallback &&
				mFrameTime < pOther.mFrameTime &&
				mAlarmId < pOther.mAlarmId &&
				mExtraData < pOther.mExtraData);
		}
		inline bool operator==(const Alarm& pOther) const
		{
			return (mObject == pOther.mObject &&
				mCallback == pOther.mCallback &&
				mFrameTime == pOther.mFrameTime &&
				mAlarmId == pOther.mAlarmId &&
				mExtraData == pOther.mExtraData);
		}
	};
	struct AlarmHasher
	{
		inline size_t operator()(const Alarm& pAlarm) const
		{
			const int f = pAlarm.mCallback.empty()? 5527 : 1;
			return (pAlarm.mFrameTime + pAlarm.mAlarmId + (size_t)pAlarm.mObject) * f;
		}
	};
	typedef std::unordered_set<Alarm, AlarmHasher> AlarmSet;
	typedef std::unordered_map<Tbc::PhysicsManager::BodyID, ContextObject*> BodyTable;
	typedef std::unordered_set<GameObjectId> IdSet;
	typedef BodyTable::value_type BodyPair;

	void OnDelayedDelete(int, ContextObject* pObject, void*);
	void DoAddAlarmCallback(Alarm& pAlarm, float pSeconds);
	void DispatchTickCallbacks();
	void DispatchMicroTickCallbacks(float pFrameTimeDelta);
	void DispatchAlarmCallbacks();

	GameManager* mGameManager;

	bool mIsObjectOwner;
	RecycledIdQueue mRecycledIdQueue;
	ObjectIdManager mLocalObjectIdManager;
	ObjectIdManager mRemoteObjectIdManager;
	ContextObjectTable mObjectTable;
	ContextObjectTable mPhysicsSenderObjectTable;
	BodyTable mBodyTable;
	ContextObjectTable mAttributeSenderObjectTable;
	ContextObjectTable mTickCallbackObjectTable;
	ContextObjectTable mMicroTickCallbackObjectTable;
	Lock mAlarmMutex;
	AlarmSet mAlarmCallbackObjectSet;
	double mMaxPostKillProcessingTime;
	IdSet mPostKillSet;

	logclass();
};



}
