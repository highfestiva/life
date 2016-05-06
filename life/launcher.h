
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../lepra/include/transformation.h"
#include "life.h"



namespace tbc {
class ChunkyBoneGeometry;
}



namespace life {



class Launcher {
public:
	virtual void Shoot(cure::ContextObject* avatar, int weapon) = 0;
	virtual void Detonate(cure::ContextObject* explosive, const tbc::ChunkyBoneGeometry* explosive_geometry, const vec3& position, const vec3& velocity, const vec3& normal, float strength) = 0;
	virtual void OnBulletHit(cure::ContextObject* bullet, cure::ContextObject* hit_object) = 0;
};



}
