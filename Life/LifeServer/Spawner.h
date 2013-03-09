
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../Cure/Include/CppContextObject.h"
#include "../../Lepra/Include/HiResTimer.h"
#include "../Life.h"



namespace Life
{



class Spawner: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	Spawner(Cure::ContextManager* pManager);
	virtual ~Spawner();

	void PlaceObject(Cure::ContextObject* pObject);

private:
	virtual void OnAlarm(int pAlarmId, void* pExtraData);
	void OnCreate(float pCreateInterval);
	void OnDestroy(float pDestroyInterval);

	LOG_CLASS_DECLARE();
};



}
