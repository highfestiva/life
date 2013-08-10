
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
	TransformationF GetSpawnPoint() const;

private:
	virtual void OnAlarm(int pAlarmId, void* pExtraData);
	void OnCreate(float pCreateInterval, bool pHasRecreate);
	void OnDestroy(float pDestroyInterval);
	void OnRecreate(float pRecreateInterval);
	void Create();
	virtual int GetSpawnCount() const;

	StopWatch mRecreateTimer;
	int mSpawnPointIndex;

	LOG_CLASS_DECLARE();
};



}
