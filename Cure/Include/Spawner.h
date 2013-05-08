
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "CppContextObject.h"
#include "Cure.h"



namespace Cure
{



class Spawner: public CppContextObject
{
	typedef CppContextObject Parent;
public:
	Spawner(ContextManager* pManager);
	virtual ~Spawner();

	void PlaceObject(ContextObject* pObject);
	TransformationF GetSpawnPoint() const;

private:
	virtual void OnAlarm(int pAlarmId, void* pExtraData);
	void OnCreate(float pCreateInterval);
	void OnDestroy(float pDestroyInterval);
	virtual int GetSpawnCount() const;

	LOG_CLASS_DECLARE();
};



}
