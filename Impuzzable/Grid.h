
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#pragma once
#include "../lepra/include/vector3d.h"
#include "impuzzable.h"

#define GRID_SIZE	0.125f
#define GRID_HALF	(GRID_SIZE/2)
#define GRID_INT_INVERT	(1/GRID_SIZE)



namespace Impuzzable {
namespace grid {



static inline vec3 GetCenterPosition(const vec3& position) {
	vec3 _position(position);
	_position.x = int(Math::Round(_position.x*GRID_INT_INVERT)) * GRID_SIZE;
	_position.y = int(Math::Round(_position.y*GRID_INT_INVERT)) * GRID_SIZE;
	_position.z = int(Math::Round(_position.z*GRID_INT_INVERT)) * GRID_SIZE;
	return _position;
}



}
}
