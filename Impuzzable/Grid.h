
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#pragma once
#include "../Lepra/Include/Vector3D.h"
#include "Impuzzable.h"

#define GRID_SIZE	0.125f
#define GRID_HALF	(GRID_SIZE/2)
#define GRID_INT_INVERT	(1/GRID_SIZE)



namespace Impuzzable
{
namespace Grid
{



static inline vec3 GetCenterPosition(const vec3& pPosition)
{
	vec3 lPosition(pPosition);
	lPosition.x = int(Math::Round(lPosition.x*GRID_INT_INVERT)) * GRID_SIZE;
	lPosition.y = int(Math::Round(lPosition.y*GRID_INT_INVERT)) * GRID_SIZE;
	lPosition.z = int(Math::Round(lPosition.z*GRID_INT_INVERT)) * GRID_SIZE;
	return lPosition;
}



}
}
