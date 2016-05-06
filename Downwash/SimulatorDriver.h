
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/cppcontextobject.h"
#include "downwashmanager.h"



namespace Downwash {



class SimulatorDriver: public cure::CppContextObject {
	typedef cure::CppContextObject Parent;
public:
	SimulatorDriver(cure::GameManager* game, cure::GameObjectId simulator_id);
	virtual ~SimulatorDriver();

	virtual void OnAlarm(int alarm_id, void* extra_data);

private:
	cure::GameManager* game_;
	cure::GameObjectId simulator_id_;
	logclass();
};



}
