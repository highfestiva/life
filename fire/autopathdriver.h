
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/contextpath.h"
#include "../cure/include/cppcontextobject.h"
#include "firemanager.h"



namespace Fire {



class AutoPathDriver: public cure::CppContextObject {
	typedef cure::CppContextObject Parent;
public:
	AutoPathDriver(FireManager* game, cure::GameObjectId vehicle_id, const str& path_name);
	virtual ~AutoPathDriver();

private:
	virtual void OnTick();
	void GetClosestPathDistance(const vec3& position, vec3& closest_point, float wanted_distance);

	FireManager* game_;
	cure::GameObjectId vehicle_id_;
	const str path_name_;
	cure::ContextPath::SplinePath* path_;
	StopWatch still_timer_;
	logclass();
};



}
