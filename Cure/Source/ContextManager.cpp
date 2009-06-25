
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



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
	mLocalObjectIdManager(0x4FFFFFFF, 0x7FFFFFFF-1, 0xFFFFFFFF),
	mRemoteObjectIdManager(1, 0x4FFFFFFF-1, 0xFFFFFFFF)
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
	mObjectTable.erase(pObject->GetInstanceId());
}

ContextObject* ContextManager::GetObject(GameObjectId pInstanceId) const
{
	ContextObjectTable::const_iterator x = mObjectTable.find(pInstanceId);
	ContextObject* lObject = 0;
	if (x != mObjectTable.end())
	{
		lObject = x->second;
	}
	return (lObject);
}

const ContextManager::ContextObjectTable& ContextManager::GetObjectTable() const
{
	return (mObjectTable);
}

void ContextManager::ClearObjects()
{
	if (mIsObjectOwner)
	{
		while (mObjectTable.size() > 0)
		{
			ContextObject* lObject = mObjectTable.begin()->second;
			delete (lObject);
		}
	}
	else
	{
		mObjectTable.clear();
	}
}

void ContextManager::AddPhysicsSenderObject(ContextObject* pObject)
{
	assert(pObject->GetInstanceId() != 0);
	assert(mObjectTable.find(pObject->GetInstanceId()) != mObjectTable.end());
	assert(pObject->GetManager() == this);
	mPhysicsSenderObjectTable.insert(ContextObjectPair(pObject->GetInstanceId(), pObject));
}

void ContextManager::AddPhysicsBody(ContextObject* pObject, TBC::PhysicsEngine::BodyID pBodyId)
{
	mBodyTable.insert(BodyPair(pBodyId, pObject));
}

void ContextManager::RemovePhysicsBody(TBC::PhysicsEngine::BodyID pBodyId)
{
	if (pBodyId != TBC::INVALID_BODY)
	{
		mBodyTable.erase(pBodyId);
		GetGameManager()->GetPhysicsManager()->DeleteBody(pBodyId);
	}
}

void ContextManager::RemovePhysicsJoint(TBC::PhysicsEngine::JointID pJointId)
{
	if (pJointId != TBC::INVALID_JOINT)
	{
		GetGameManager()->GetPhysicsManager()->DeleteJoint(pJointId);
	}
}



GameObjectId ContextManager::AllocateGameObjectId(NetworkObjectType pNetworkType)
{
	GameObjectId lObjectId;
	if (pNetworkType == NETWORK_OBJECT_LOCAL_ONLY)
	{
		lObjectId = mLocalObjectIdManager.GetFreeId();
	}
	else
	{
		lObjectId = mRemoteObjectIdManager.GetFreeId();
	}
	return (lObjectId);
}

void ContextManager::FreeGameObjectId(NetworkObjectType pNetworkType, GameObjectId pObjectId)
{
	if (pNetworkType == NETWORK_OBJECT_LOCAL_ONLY)
	{
		mLocalObjectIdManager.RecycleId(pObjectId);
	}
	else
	{
		mRemoteObjectIdManager.RecycleId(pObjectId);
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
			mAlarmCallbackObjectMap.erase(x++);
		}
		else
		{
			++x;
		}
	}
}



void ContextManager::Tick()
{
	DispatchTickCallbacks(mGameManager->GetConstTimeManager()->GetCurrentFrameTime());
	DispatchAlarmCallbacks();
}

void ContextManager::TickPhysics()
{
	DispatchPhysicsUpdateCallbacks();
}

void ContextManager::HandleIdledBodies()
{
	typedef TBC::PhysicsEngine::BodySet BodySet;
	BodySet lBodySet = mGameManager->GetPhysicsManager()->GetIdledBodies();
	BodySet::iterator x = lBodySet.begin();
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
	for (; x != mPhysicsSenderObjectTable.end(); ++x)
	{
		mGameManager->OnPhysicsSend(x->second);
	}
	mPhysicsSenderObjectTable.clear();
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

void ContextManager::DispatchTickCallbacks(float pFrameTimeDelta)
{
	ContextObjectTable::iterator x = mTickCallbackObjectTable.begin();
	for (; x != mTickCallbackObjectTable.end(); ++x)
	{
		x->second->OnTick(pFrameTimeDelta);
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
			mAlarmCallbackObjectMap.erase(x++);
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
