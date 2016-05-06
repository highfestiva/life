
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/cppcontextobject.h"
#include "downwashmanager.h"



namespace Downwash {



class DownwashManager;



class Automan: public cure::CppContextObject {
	typedef cure::CppContextObject Parent;
public:
	Automan(cure::GameManager* game, cure::GameObjectId car_id, const vec3& direction);
	virtual ~Automan();
	virtual void OnTick();

private:
	cure::GameObjectId car_id_;
	vec3 direction_;
	StopWatch still_timer_;
	logclass();
};



}
