
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../cure/include/cppcontextobject.h"
#include "../hovertank.h"



namespace life {
class Launcher;
}



namespace HoverTank {



class BombPlane: public cure::CppContextObject {
public:
	typedef cure::CppContextObject Parent;

	BombPlane(cure::ResourceManager* resource_manager, const str& class_id, life::Launcher* launcher, const vec3& target);
	virtual ~BombPlane();

private:
	virtual void OnLoaded();
	virtual void OnTick();

	life::Launcher* launcher_;
	vec3 target_;
	int last_bomb_tick_;
	float bombing_radius_squared_;
	float drop_interval_;
	bool is_detonated_;

	logclass();
};



}
