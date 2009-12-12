
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include <list>
#include "../Include/ContextObject.h"
#include "../Include/ContextManager.h"
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



void ContextManager::SetIsObjectOwner(bool pIsObjectOwner)
{
	mIsObjectOwner = pIsObjectOwner;
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
	DisableTickCallback(pObject);
	DisablePhysicsUpdateCallback(pObject);
	mPhysicsSenderObjectTable.erase(pObject->GetInstanceId());
	mObjectTable.erase(pObject->GetInstanceId());
}

bool ContextManager::DeleteObject(GameObjectId pInstanceId)
{
	bool lOk = false;
	ContextObject* lObject = GetObject(pInstanceId, true);
	if (lObject)
	{
		log_volatile(mLog.Debugf(_T("Deleting context object %i."), pInstanceId));
		delete (lObject);
		lOk = true;
	}
	else
	{
		log_volatile(mLog.Debugf(_T("Could not delete context object %i, since not found."), pInstanceId));
	}
	return (lOk);
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



void ContextManager::EnablePhysicsUpdateCallback(ContextObject* pObject)
{
	mPhysicsUpdateCallbackObjectTable.insert(ContextObjectPair(pObject->GetInstanceId(), pObject));
}

void ContextManager::DisablePhysicsUpdateCallback(ContextObject* pObject)
{
	mPhysicsUpdateCallbackObjectTable.erase(pObject->GetInstanceId());
}

void ContextManager::EnableTickCallback(ContextObject* pObject)
{
	mTickCallbackObjectTable.insert(ContextObjectPair(pObject->GetInstanceId(), pObject));
}

void ContextManager::DisableTickCallback(ContextObject* pObject)
{
	mTickCallbackObjectTable.erase(pObject->GetInstanceId());
}

void ContextManager::SetAlarmCallback(ContextObject* pObject, int pAlarmId, float pSeconds)
{
	const TimeManager* lTime = ((const GameManager*)mGameManager)->GetConstTimeManager();
	int lFrame = lTime->GetCurrentPhysicsFrame()+lTime->ConvertSecondsToPhysicsFrames(pSeconds);
	mAlarmCallbackObjectMap.insert(AlarmPair(lFrame, AlarmInfo(pObject, pAlarmId)));
}

void ContextManager::CancelPendingAlarmCallbacks(ContextObject* pObject)
{
	AlarmMap::iterator x = mAlarmCallbackObjectMap.begin();
	while (x != mAlarmCallbackObjectMap.end())
	{
		if ((*x).second.mObject == pObject)
		{
			AlarmMap::iterator y = x;
			++x;
			mAlarmCallbackObjectMap.erase(y);
		}
		else
		{
			++x;
		}
	}
}



void ContextManager::Tick(float pTimeDelta)
{
	DispatchTickCallbacks(pTimeDelta);
	DispatchAlarmCallbacks();
}

void ContextManager::TickPhysics()
{
	DispatchPhysicsUpdateCallbacks();
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
		else
		{
			mLog.AError("Body not present in body table!");
		}
	}
}

void ContextManager::HandlePhysicsSend()
{
	ContextObjectTable::iterator x = mPhysicsSenderObjectTable.begin();
	while (x != mPhysicsSenderObjectTable.end())
	{
		if (mGameManager->OnPhysicsSend(x->second))
		{
			ContextObjectTable::iterator y = x;
			++x;
			mPhysicsSenderObjectTable.erase(y);
		}
		else
		{
			++x;
		}
	}
}



void ContextManager::DispatchPhysicsUpdateCallbacks()
{
	ContextObjectTable::iterator x = mPhysicsUpdateCallbackObjectTable.begin();
	for (; x != mPhysicsUpdateCallbackObjectTable.end(); ++x)
	{
		ContextObject* lObject = x->second;
		lObject->OnPhysicsTick();
	}
}

void ContextManager::DispatchTickCallbacks(float pTimeDelta)
{
	ContextObjectTable::iterator x = mTickCallbackObjectTable.begin();
	for (; x != mTickCallbackObjectTable.end(); ++x)
	{
		x->second->OnTick(pTimeDelta);
	}
}

void ContextManager::DispatchAlarmCallbacks()
{
	// Divide dispatch into two parts to avoid callbacks messing up the skiplist:
	// 1. Extract due alarms into list.
	// 2. Callback alarms.

	int lCurrentPhysicsFrame = mGameManager->GetConstTimeManager()->GetCurrentPhysicsFrame();

	// Extract overdue alarms.
	std::list<AlarmInfo> lCallbackList;
	AlarmMap::iterator x = mAlarmCallbackObjectMap.begin();
	while (x != mAlarmCallbackObjectMap.end())
	{
		if (lCurrentPhysicsFrame-(*x).first <= 0)
		{
			lCallbackList.push_back((*x).second);
			AlarmMap::iterator y = x;
			++x;
			mAlarmCallbackObjectMap.erase(y);
		}
		else
		{
			break;
		}
	}

	// Callback alarms.
	while (!lCallbackList.empty())
	{
		const AlarmInfo& lAlarm = lCallbackList.front();
		lAlarm.mObject->OnAlarm(lAlarm.mAlarmId);
		lCallbackList.erase(lCallbackList.begin());
	}
}



ContextManager::AlarmInfo::AlarmInfo():
	mObject(0),
	mAlarmId(0)
{
}

ContextManager::AlarmInfo::AlarmInfo(ContextObject* pObject, int pAlarmId):
	mObject(pObject),
	mAlarmId(pAlarmId)
{
}



LOG_CLASS_DEFINE(GAME_CONTEXT, ContextManager);



}
