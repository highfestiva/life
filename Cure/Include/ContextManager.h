
// Author: Jonas Bystr�m
// Copyright (c) 2002-2007, Righteous Games



#pragma once

#include <map>
#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/IdManager.h"
#include "../../TBC/Include/PhysicsEngine.h"
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
	ContextObject* GetObject(GameObjectId pInstanceId) const;
	const ContextObjectTable& GetObjectTable() const;
	void ClearObjects();
	void AddPhysicsBody(ContextObject* pObject, TBC::PhysicsEngine::BodyID pBodyId);
	void RemovePhysicsBody(TBC::PhysicsEngine::BodyID pBodyId);
	void RemovePhysicsJoint(TBC::PhysicsEngine::JointID pJointId);

	GameObjectId AllocateGameObjectId(NetworkObjectType pNetworkType);
	void FreeGameObjectId(NetworkObjectType pNetworkType, GameObjectId pObjectId);

	void EnablePhysicsUpdateCallback(ContextObject* pObject);
	void DisablePhysicsUpdateCallback(ContextObject* pObject);
	void EnableTickCallback(ContextObject* pObject);
	void DisableTickCallback(ContextObject* pObject);
	void SetAlarmCallback(ContextObject* pObject, int pAlarmId, float pSeconds);
	void CancelPendingAlarmCallbacks(ContextObject* pObject);

	void Tick();
	void TickPhysics();
	void HandleIdledBodies();

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
	typedef std::hash_map<TBC::PhysicsEngine::BodyID, ContextObject*> BodyTable;
	typedef std::pair<TBC::PhysicsEngine::BodyID, ContextObject*> BodyPair;

	GameManager* mGameManager;

	bool mIsObjectOwner;
	ObjectIdManager mLocalObjectIdManager;
	ObjectIdManager mRemoteObjectIdManager;
	ContextObjectTable mObjectTable;
	BodyTable mBodyTable;
	ContextObjectTable mPhysicsUpdateCallbackObjectTable;
	ContextObjectTable mTickCallbackObjectTable;
	AlarmMap mAlarmCallbackObjectMap;

	LOG_CLASS_DECLARE();
};



}
