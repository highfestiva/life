
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#pragma once

#include <map>
#include "../../Lepra/Include/HashTable.h"
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

	ContextManager(GameManager* pGameManager);
	virtual ~ContextManager();

	GameManager* GetGameManager() const;

	void SetIsObjectOwner(bool pIsObjectOwner);
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

	void EnablePhysicsUpdateCallback(ContextObject* pObject);
	void DisablePhysicsUpdateCallback(ContextObject* pObject);
	void EnableTickCallback(ContextObject* pObject);
	void DisableTickCallback(ContextObject* pObject);
	void SetAlarmCallback(ContextObject* pObject, int pAlarmId, float pSeconds);
	void CancelPendingAlarmCallbacks(ContextObject* pObject);

	void Tick(float pTimeDelta);
	void TickPhysics();
	void HandleIdledBodies();
	void HandlePhysicsSend();

private:
	void DispatchPhysicsUpdateCallbacks();
	void DispatchTickCallbacks(float pFrameTimeDelta);
	void DispatchAlarmCallbacks();

	typedef Lepra::IdManager<GameObjectId> ObjectIdManager;
	typedef std::pair<GameObjectId, ContextObject*> ContextObjectPair;
	struct AlarmInfo
	{
		AlarmInfo();
		AlarmInfo(ContextObject* pObject, int pAlarmId);
		ContextObject* mObject;
		int mAlarmId;
	};
	typedef std::map<int, AlarmInfo> AlarmMap;
	typedef std::pair<int, AlarmInfo> AlarmPair;
	typedef std::hash_map<TBC::PhysicsManager::BodyID, ContextObject*> BodyTable;
	typedef std::pair<TBC::PhysicsManager::BodyID, ContextObject*> BodyPair;

	GameManager* mGameManager;

	bool mIsObjectOwner;
	ObjectIdManager mLocalObjectIdManager;
	ObjectIdManager mRemoteObjectIdManager;
	ContextObjectTable mObjectTable;
	ContextObjectTable mPhysicsSenderObjectTable;
	BodyTable mBodyTable;
	ContextObjectTable mPhysicsUpdateCallbackObjectTable;
	ContextObjectTable mTickCallbackObjectTable;
	AlarmMap mAlarmCallbackObjectMap;

	LOG_CLASS_DECLARE();
};



}
