
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/ContextManager.h"
#include <list>
#include "../Include/ContextObjectAttribute.h"
#include "../Include/ContextObject.h"
#include "../Include/GameManager.h"
#include "../Include/TimeManager.h"



namespace Cure
{



ContextManager::ContextManager(GameManager* pGameManager):
	mGameManager(pGameManager),
	mIsObjectOwner(true),
	mLocalObjectIdManager(0x40000000, 0x7FFFFFFF-1, 0xFFFFFFFF),
	mRemoteObjectIdManager(1, 0x40000000-1, 0xFFFFFFFF)
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
	assert(mObjectTable.empty());
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
	assert(pObject->GetInstanceId() == 0);
	pObject->SetInstanceId(AllocateGameObjectId(NETWORK_OBJECT_LOCAL_ONLY));
	assert(pObject->GetManager() == 0);
	pObject->SetManager(this);
	AddObject(pObject);
}

void ContextManager::AddObject(ContextObject* pObject)
{
	assert(pObject->GetInstanceId() != 0);
	assert(mObjectTable.find(pObject->GetInstanceId()) == mObjectTable.end());
	assert(pObject->GetManager() == this);
	mObjectTable.insert(ContextObjectPair(pObject->GetInstanceId(), pObject));
}

void ContextManager::RemoveObject(ContextObject* pObject)
{
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
		log_volatile(mLog.Tracef(_T("Deleting context object %i."), pInstanceId));
		delete (lObject);
		lOk = true;
	}
	else
	{
		log_volatile(mLog.Debugf(_T("Could not delete context object %i, since not found."), pInstanceId));
	}
	return (lOk);
}

void ContextManager::PostKillObject(GameObjectId pInstanceId)
{
	mPostKillSet.insert(pInstanceId);
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
	assert(pObject->GetInstanceId() != 0);
	assert(mObjectTable.find(pObject->GetInstanceId()) != mObjectTable.end());
	assert(pObject->GetManager() == this);
	mPhysicsSenderObjectTable.insert(ContextObjectPair(pObject->GetInstanceId(), pObject));
}

void ContextManager::AddPhysicsBody(ContextObject* pObject, TBC::PhysicsManager::BodyID pBodyId)
{
	mBodyTable.insert(BodyPair(pBodyId, pObject));
}

void ContextManager::RemovePhysicsBody(TBC::PhysicsManager::BodyID pBodyId)
{
	if (pBodyId != TBC::INVALID_BODY)
	{
		mBodyTable.erase(pBodyId);
	}
}

void ContextManager::AddAttributeSenderObject(ContextObject* pObject)
{
	assert(pObject->GetInstanceId() != 0);
	assert(mObjectTable.find(pObject->GetInstanceId()) != mObjectTable.end());
	assert(pObject->GetManager() == this);
	mAttributeSenderObjectTable.insert(ContextObjectPair(pObject->GetInstanceId(), pObject));
}

void ContextManager::UnpackObjectAttribute(GameObjectId pObjectId, const uint8* pData, unsigned pSize)
{
	ContextObject* lObject = GetObject(pObjectId);
	if (lObject)
	{
		ContextObjectAttribute::Unpack(lObject, pData, pSize);
	}
	else
	{
		mLog.Errorf(_T("Trying to unpack attribute for non-existent object %u."), pObjectId);
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
	if (pNetworkType == NETWORK_OBJECT_LOCAL_ONLY)
	{
		mLocalObjectIdManager.RecycleId(pInstanceId);
	}
	else
	{
		mRemoteObjectIdManager.RecycleId(pInstanceId);
	}
}

bool ContextManager::IsLocalGameObjectId(GameObjectId pInstanceId) const
{
	return pInstanceId >= mLocalObjectIdManager.GetMinId();
}



void ContextManager::EnableTickCallback(ContextObject* pObject)
{
	assert(pObject->GetInstanceId());
	mTickCallbackObjectTable.insert(ContextObjectPair(pObject->GetInstanceId(), pObject));
}

void ContextManager::DisableTickCallback(ContextObject* pObject)
{
	mTickCallbackObjectTable.erase(pObject->GetInstanceId());
}

void ContextManager::EnableMicroTickCallback(ContextObject* pObject)
{
	mMicroTickCallbackObjectTable.insert(ContextObjectPair(pObject->GetInstanceId(), pObject));
}

void ContextManager::DisableMicroTickCallback(ContextObject* pObject)
{
	mMicroTickCallbackObjectTable.erase(pObject->GetInstanceId());
}

void ContextManager::AddAlarmCallback(ContextObject* pObject, int pAlarmId, float pSeconds, void* pExtraData)
{
	assert(pObject->GetInstanceId() != 0);
	const TimeManager* lTime = ((const GameManager*)mGameManager)->GetTimeManager();
	const int lFrame = lTime->GetCurrentPhysicsFrameAddSeconds(pSeconds);
	mAlarmCallbackObjectSet.insert(Alarm(pObject, lFrame, pAlarmId, pExtraData));
}

void ContextManager::CancelPendingAlarmCallbacksById(ContextObject* pObject, int pAlarmId)
{
	AlarmSet::iterator x = mAlarmCallbackObjectSet.begin();
	while (x != mAlarmCallbackObjectSet.end())
	{
		if (x->mObject == pObject && x->mAlarmId == pAlarmId)
		{
			mAlarmCallbackObjectSet.erase(x++);
		}
		else
		{
			++x;
		}
	}
}

void ContextManager::CancelPendingAlarmCallbacks(ContextObject* pObject)
{
	AlarmSet::iterator x = mAlarmCallbackObjectSet.begin();
	while (x != mAlarmCallbackObjectSet.end())
	{
		if (x->mObject == pObject)
		{
			mAlarmCallbackObjectSet.erase(x++);
		}
		else
		{
			++x;
		}
	}
}



void ContextManager::MicroTick(float pTimeDelta)
{
	if (GetGameManager()->IsPrimaryManager())
	{
		DispatchMicroTickCallbacks(pTimeDelta);
	}
	DispatchAlarmCallbacks();
}

void ContextManager::TickPhysics()
{
	DispatchTickCallbacks();
}

void ContextManager::HandleIdledBodies()
{
	typedef TBC::PhysicsManager::BodySet BodySet;
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
			mLog.AError("Body not present in body table!");
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
	IdSet::iterator x = mPostKillSet.begin();
	for (; x != mPostKillSet.end(); ++x)
	{
		DeleteObject(*x);
	}
	mPostKillSet.clear();
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

	std::list<Alarm> lCallbackList;
	AlarmSet::iterator x = mAlarmCallbackObjectSet.begin();
	while (x != mAlarmCallbackObjectSet.end())
	{
		if (mGameManager->GetTimeManager()->GetCurrentPhysicsFrameDelta(x->mFrameTime) >= 0)
		{
			lCallbackList.push_back(*x);
			mAlarmCallbackObjectSet.erase(x++);
		}
		else
		{
			//break;
			++x;
		}
	}

	// Callback alarms.
	for (std::list<Alarm>::iterator x = lCallbackList.begin(); x != lCallbackList.end(); ++x)
	{
		const Alarm& lAlarm = *x;
		if (lAlarm.mAlarmId < SYSTEM_ALARM_ID)
		{
			lAlarm.mObject->OnAlarm(lAlarm.mAlarmId, lAlarm.mExtraData);
		}
		else
		{
			mGameManager->OnAlarm(lAlarm.mAlarmId, lAlarm.mObject, lAlarm.mExtraData);
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT, ContextManager);



}
