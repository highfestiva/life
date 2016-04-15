
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/ContextManager.h"
#include <list>
#include "../Include/ContextObjectAttribute.h"
#include "../Include/ContextObject.h"
#include "../Include/GameManager.h"
#include "../Include/TimeManager.h"



namespace Cure
{



#define CHECK_OBJ_ALARM_ERASED(obj)	LEPRA_DEBUG_CODE( for (AlarmSet::iterator x = mAlarmCallbackObjectSet.begin(); x != mAlarmCallbackObjectSet.end(); ++x) { deb_assert(x->mObject != obj); } )



ContextManager::ContextManager(GameManager* pGameManager):
	mGameManager(pGameManager),
	mIsObjectOwner(true),
	mLocalObjectIdManager(0x40000000, 0x7FFFFFFF-1, 0xFFFFFFFF),
	mRemoteObjectIdManager(1, 0x40000000-1, 0xFFFFFFFF),
	mMaxPostKillProcessingTime(0.01)
{
}

ContextManager::~ContextManager()
{
	ClearObjects();
	mGameManager = 0;
}



GameManager* ContextManager::GetGameManager() const
{
	return (mGameManager);
}

void ContextManager::SetLocalRange(unsigned pIndex, unsigned pCount)
{
	deb_assert(mObjectTable.empty());
	const GameObjectId lStartId = 0x40000000;
	const GameObjectId lBlockSize = (0x7FFFFFFF-lStartId-pCount)/pCount;
	const GameObjectId lOffset = lStartId + lBlockSize*pIndex;
	mLocalObjectIdManager = ObjectIdManager(lOffset, lOffset+lBlockSize-1, 0xFFFFFFFF);
}


void ContextManager::SetIsObjectOwner(bool pIsObjectOwner)
{
	mIsObjectOwner = pIsObjectOwner;
}

void ContextManager::AddLocalObject(ContextObject* pObject)
{
	deb_assert(pObject->GetInstanceId() == 0);
	pObject->SetInstanceId(AllocateGameObjectId(NETWORK_OBJECT_LOCAL_ONLY));
	deb_assert(pObject->GetManager() == 0);
	pObject->SetManager(this);
	AddObject(pObject);
}

void ContextManager::AddObject(ContextObject* pObject)
{
	deb_assert(pObject->GetInstanceId() != 0);
	deb_assert(mObjectTable.find(pObject->GetInstanceId()) == mObjectTable.end());
	deb_assert(pObject->GetManager() == this);
	mObjectTable.insert(ContextObjectTable::value_type(pObject->GetInstanceId(), pObject));
}

void ContextManager::RemoveObject(ContextObject* pObject)
{
	deb_assert(Thread::GetCurrentThread()->GetThreadName() == "MainThread");
	CancelPendingAlarmCallbacks(pObject);
	DisableMicroTickCallback(pObject);
	DisableTickCallback(pObject);
	mPhysicsSenderObjectTable.erase(pObject->GetInstanceId());
	mAttributeSenderObjectTable.erase(pObject->GetInstanceId());
	mObjectTable.erase(pObject->GetInstanceId());
}

bool ContextManager::DeleteObject(GameObjectId pInstanceId)
{
	bool lOk = false;
	ContextObject* lObject = GetObject(pInstanceId, true);
	if (lObject)
	{
		log_volatile(mLog.Tracef("Deleting context object %i.", pInstanceId));
		delete (lObject);
		lOk = true;
	}
	else
	{
		log_volatile(mLog.Debugf("Could not delete context object %i, since not found.", pInstanceId));
	}
	return (lOk);
}

void ContextManager::SetPostKillTimeout(double pTimeout)
{
	mMaxPostKillProcessingTime = pTimeout;
}

void ContextManager::PostKillObject(GameObjectId pInstanceId)
{
	mPostKillSet.insert(pInstanceId);
}

void ContextManager::DelayKillObject(ContextObject* pObject, float pSeconds)
{
	AddAlarmExternalCallback(pObject, AlarmExternalCallback(this, &ContextManager::OnDelayedDelete), 1, pSeconds, 0);
}

ContextObject* ContextManager::GetObject(GameObjectId pInstanceId, bool pForce) const
{
	ContextObjectTable::const_iterator x = mObjectTable.find(pInstanceId);
	ContextObject* lObject = 0;
	if (x != mObjectTable.end())
	{
		lObject = x->second;
		if (!pForce && !lObject->IsLoaded())
		{
			lObject = 0;
		}
	}
	return (lObject);
}

const ContextManager::ContextObjectTable& ContextManager::GetObjectTable() const
{
	return (mObjectTable);
}

void ContextManager::ClearObjects()
{
	mPhysicsSenderObjectTable.clear();
	mAttributeSenderObjectTable.clear();
	while (mObjectTable.size() > 0)
	{
		ContextObject* lObject = mObjectTable.begin()->second;
		if (mIsObjectOwner)
		{
			delete (lObject);
		}
		else
		{
			lObject->SetManager(0);
			mObjectTable.erase(mObjectTable.begin());
		}
	}
}

void ContextManager::AddPhysicsSenderObject(ContextObject* pObject)
{
	deb_assert(pObject->GetInstanceId() != 0);
	deb_assert(mObjectTable.find(pObject->GetInstanceId()) != mObjectTable.end());
	deb_assert(pObject->GetManager() == this);
	deb_assert(pObject->GetManager()->GetGameManager()->GetTickLock()->IsOwner());
	mPhysicsSenderObjectTable.insert(ContextObjectTable::value_type(pObject->GetInstanceId(), pObject));
}

void ContextManager::AddPhysicsBody(ContextObject* pObject, Tbc::PhysicsManager::BodyID pBodyId)
{
	mBodyTable.insert(BodyPair(pBodyId, pObject));
}

void ContextManager::RemovePhysicsBody(Tbc::PhysicsManager::BodyID pBodyId)
{
	if (pBodyId != Tbc::INVALID_BODY)
	{
		mBodyTable.erase(pBodyId);
	}
}

void ContextManager::AddAttributeSenderObject(ContextObject* pObject)
{
	deb_assert(pObject->GetInstanceId() != 0);
	deb_assert(mObjectTable.find(pObject->GetInstanceId()) != mObjectTable.end());
	deb_assert(pObject->GetManager() == this);
	mAttributeSenderObjectTable.insert(ContextObjectTable::value_type(pObject->GetInstanceId(), pObject));
}

void ContextManager::UnpackObjectAttribute(GameObjectId pObjectId, const uint8* pData, unsigned pSize)
{
	ContextObject* lObject = GetObject(pObjectId, true);
	if (lObject)
	{
		ContextObjectAttribute::Unpack(lObject, pData, pSize);
	}
	else
	{
		log_volatile(mLog.Debugf("Trying to unpack attribute for non-existent object %u.", pObjectId));
	}
}



GameObjectId ContextManager::AllocateGameObjectId(NetworkObjectType pNetworkType)
{
	GameObjectId lInstanceId;
	if (pNetworkType == NETWORK_OBJECT_LOCAL_ONLY)
	{
		lInstanceId = mLocalObjectIdManager.GetFreeId();
	}
	else
	{
		lInstanceId = mRemoteObjectIdManager.GetFreeId();
	}
	return (lInstanceId);
}

void ContextManager::FreeGameObjectId(NetworkObjectType pNetworkType, GameObjectId pInstanceId)
{
	mRecycledIdQueue.push_back(GameObjectIdRecycleInfo(pInstanceId, pNetworkType));
}

bool ContextManager::IsLocalGameObjectId(GameObjectId pInstanceId) const
{
	return pInstanceId >= mLocalObjectIdManager.GetMinId();
}



void ContextManager::EnableTickCallback(ContextObject* pObject)
{
	deb_assert(pObject->GetInstanceId());
	mTickCallbackObjectTable.insert(ContextObjectTable::value_type(pObject->GetInstanceId(), pObject));
}

void ContextManager::DisableTickCallback(ContextObject* pObject)
{
	mTickCallbackObjectTable.erase(pObject->GetInstanceId());
}

void ContextManager::EnableMicroTickCallback(ContextObject* pObject)
{
	if (pObject->GetNetworkObjectType() == NETWORK_OBJECT_LOCAL_ONLY || GetGameManager()->IsPrimaryManager())
	{
		mMicroTickCallbackObjectTable.insert(ContextObjectTable::value_type(pObject->GetInstanceId(), pObject));
	}
}

void ContextManager::DisableMicroTickCallback(ContextObject* pObject)
{
	mMicroTickCallbackObjectTable.erase(pObject->GetInstanceId());
}

void ContextManager::AddAlarmCallback(ContextObject* pObject, int pAlarmId, float pSeconds, void* pExtraData)
{
	Alarm lAlarm(pObject, 0, pAlarmId, pExtraData);
	DoAddAlarmCallback(lAlarm, pSeconds);
}

void ContextManager::AddGameAlarmCallback(ContextObject* pObject, int pAlarmId, float pSeconds, void* pExtraData)
{
	pSeconds /= ((const GameManager*)mGameManager)->GetTimeManager()->GetRealTimeRatio();
	AddAlarmCallback(pObject, pAlarmId, pSeconds, pExtraData);
}

void ContextManager::AddAlarmExternalCallback(ContextObject* pObject, const AlarmExternalCallback& pCallback, int pAlarmId, float pSeconds, void* pExtraData)
{
	Alarm lAlarm(pObject, pCallback, 0, pAlarmId, pExtraData);
	DoAddAlarmCallback(lAlarm, pSeconds);
}

void ContextManager::CancelPendingAlarmCallbacksById(ContextObject* pObject, int pAlarmId)
{
	ScopeLock lLock(&mAlarmMutex);
	AlarmSet::iterator x = mAlarmCallbackObjectSet.begin();
	while (x != mAlarmCallbackObjectSet.end())
	{
		if (x->mObject == pObject && x->mAlarmId == pAlarmId)
		{
			mAlarmCallbackObjectSet.erase(x);
			x = mAlarmCallbackObjectSet.begin();	// Shouldn't be necessary, but is!
		}
		else
		{
			++x;
		}
	}
}

void ContextManager::CancelPendingAlarmCallbacks(ContextObject* pObject)
{
	deb_assert(Thread::GetCurrentThread()->GetThreadName() == "MainThread");

	ScopeLock lLock(&mAlarmMutex);
	AlarmSet::iterator x = mAlarmCallbackObjectSet.begin();
	while (x != mAlarmCallbackObjectSet.end())
	{
		if (x->mObject == pObject)
		{
			mAlarmCallbackObjectSet.erase(x);
			x = mAlarmCallbackObjectSet.begin();	// Shouldn't be necessary, but is!
		}
		else
		{
			++x;
		}
	}

	CHECK_OBJ_ALARM_ERASED(pObject);
}



void ContextManager::MicroTick(float pTimeDelta)
{
	DispatchMicroTickCallbacks(pTimeDelta);
}

void ContextManager::TickPhysics()
{
	DispatchTickCallbacks();
	DispatchAlarmCallbacks();
}

void ContextManager::HandleIdledBodies()
{
	typedef Tbc::PhysicsManager::BodySet BodySet;
	const BodySet& lBodySet = mGameManager->GetPhysicsManager()->GetIdledBodies();
	BodySet::const_iterator x = lBodySet.begin();
	for (; x != lBodySet.end(); ++x)
	{
		BodyTable::iterator y = mBodyTable.find(*x);
		if (y != mBodyTable.end())
		{
			mGameManager->OnStopped(y->second, y->first);
		}
		/*else
		{
			mLog.Error("Body not present in body table!");
		}*/
	}
}

void ContextManager::HandlePhysicsSend()
{
	ContextObjectTable::iterator x = mPhysicsSenderObjectTable.begin();
	while (x != mPhysicsSenderObjectTable.end())
	{
		if (mGameManager->OnPhysicsSend(x->second))
		{
			mPhysicsSenderObjectTable.erase(x++);
		}
		else
		{
			++x;
		}
	}
}

void ContextManager::HandleAttributeSend()
{
	ContextObjectTable::iterator x = mAttributeSenderObjectTable.begin();
	while (x != mAttributeSenderObjectTable.end())
	{
		if (mGameManager->OnAttributeSend(x->second))
		{
			mAttributeSenderObjectTable.erase(x++);
		}
		else
		{
			++x;
		}
	}
}

void ContextManager::HandlePostKill()
{
	HiResTimer lTimer(false);
	while (!mPostKillSet.empty())
	{
		mGameManager->DeleteContextObject(*mPostKillSet.begin());
		mPostKillSet.erase(mPostKillSet.begin());
		double lDelta = lTimer.QueryTimeDiff();
		if (lDelta > mMaxPostKillProcessingTime)	// Time's up, have a go later.
		{
			break;
		}
	}

	RecycledIdQueue::iterator y = mRecycledIdQueue.begin();
	while (y != mRecycledIdQueue.end())
	{
		if (y->mTimer.QueryTimeDiff() < 10.0)
		{
			break;
		}
		if (y->mNetworkType == NETWORK_OBJECT_LOCAL_ONLY)
		{
			mLocalObjectIdManager.RecycleId(y->mInstanceId);
		}
		else
		{
			mRemoteObjectIdManager.RecycleId(y->mInstanceId);
		}
		y = mRecycledIdQueue.erase(y);
	}
}



void ContextManager::DoAddAlarmCallback(Alarm& pAlarm, float pSeconds)
{
	deb_assert(pAlarm.mObject->GetInstanceId() != 0);
	deb_assert(pAlarm.mObject->GetManager() == this);
	deb_assert(GetObject(pAlarm.mObject->GetInstanceId(), true) == pAlarm.mObject);

	const TimeManager* lTime = ((const GameManager*)mGameManager)->GetTimeManager();
	pAlarm.mFrameTime = lTime->GetCurrentPhysicsFrameAddSeconds(pSeconds);
	ScopeLock lLock(&mAlarmMutex);
	mAlarmCallbackObjectSet.insert(pAlarm);
}

void ContextManager::OnDelayedDelete(int, ContextObject* pObject, void*)
{
	PostKillObject(pObject->GetInstanceId());
}

void ContextManager::DispatchTickCallbacks()
{
	ContextObjectTable::iterator x = mTickCallbackObjectTable.begin();
	for (; x != mTickCallbackObjectTable.end(); ++x)
	{
		ContextObject* lObject = x->second;
		lObject->OnTick();
	}
}

void ContextManager::DispatchMicroTickCallbacks(float pTimeDelta)
{
	ContextObjectTable::iterator x = mMicroTickCallbackObjectTable.begin();
	for (; x != mMicroTickCallbackObjectTable.end(); ++x)
	{
		x->second->OnMicroTick(pTimeDelta);
	}
}

void ContextManager::DispatchAlarmCallbacks()
{
	// Divide dispatch into two parts to avoid callbacks messing up the skiplist:
	// 1. Extract due alarms into list.
	// 2. Callback alarms.

	ScopeLock lLock(&mAlarmMutex);

	std::list<Alarm> lCallbackList;
	AlarmSet::iterator x = mAlarmCallbackObjectSet.begin();
	while (x != mAlarmCallbackObjectSet.end())
	{
		if (mGameManager->GetTimeManager()->GetCurrentPhysicsFrameDelta(x->mFrameTime) >= 0)
		{
			deb_assert(!x->mObject->GetClassId().empty());
			lCallbackList.push_back(*x);
			mAlarmCallbackObjectSet.erase(x);
			x = mAlarmCallbackObjectSet.begin();	// Shouldn't be necessary, but is!
		}
		else
		{
			++x;
		}
	}

	// Callback alarms.
	for (std::list<Alarm>::iterator x = lCallbackList.begin(); x != lCallbackList.end(); ++x)
	{
		const Alarm& lAlarm = *x;
		if (lAlarm.mCallback.empty())
		{
			lAlarm.mObject->OnAlarm(lAlarm.mAlarmId, lAlarm.mExtraData);
		}
		else
		{
			lAlarm.mCallback(lAlarm.mAlarmId, lAlarm.mObject, lAlarm.mExtraData);
		}
	}
}



loginstance(GAME_CONTEXT, ContextManager);



}
