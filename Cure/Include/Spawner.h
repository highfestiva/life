
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "CppContextObject.h"
#include "../../Lepra/Include/HiResTimer.h"
#include "Cure.h"



namespace Cure
{



class Spawner: public CppContextObject
{
	typedef CppContextObject Parent;
public:
	Spawner(ContextManager* pManager);
	virtual ~Spawner();

	void PlaceObject(ContextObject* pObject, int pSpawnPointIndex);
	xform GetSpawnPoint() const;
	static void EaseDown(Tbc::PhysicsManager* pPhysicsManager, ContextObject* pObject, const vec3* pStartPosition);

protected:
	virtual void OnTick();
	virtual void OnAlarm(int pAlarmId, void* pExtraData);
	void OnCreate(float pCreateInterval, bool pHasRecreate);
	void OnDestroy(float pDestroyInterval);
	void OnRecreate(float pRecreateInterval);
	void Create();
	virtual int GetSpawnCount() const;

private:
	typedef std::vector<GameObjectId> GameObjectIdArray;

	StopWatch mRecreateTimer;
	int mSpawnPointIndex;
	GameObjectIdArray mEaseDownObjects;

	logclass();
};



}
