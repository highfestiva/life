
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/contextpath.h"
#include "../cure/include/cppcontextobject.h"
#include "downwashmanager.h"



namespace Downwash {



class DownwashManager;



class AirBalloonPilot: public cure::CppContextObject {
	typedef cure::CppContextObject Parent;
public:
	AirBalloonPilot(DownwashManager* game, cure::GameObjectId balloon_id);
	virtual ~AirBalloonPilot();

private:
	virtual void OnTick();
	void GetClosestPathDistance(const vec3& position, vec3& closest_point);

	DownwashManager* game_;
	cure::GameObjectId balloon_id_;
	cure::ContextPath::SplinePath* path_;
	logclass();
};



}
