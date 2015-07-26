
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// The sole purpose of this file is to not have to include ode.h, PhysX.h
// or any other junk file in the rest of the system (which fucks up path
// settings on all other projects).



#pragma once

#include "../../Lepra/Include/Log.h"
#include "../Include/Tbc.h"



namespace Tbc
{



class PhysicsManager;



class PhysicsManagerFactory
{
public:
	enum Engine
	{
		ENGINE_ODE = 1,
		ENGINE_PHYSX = 2,
		ENGINE_BULLET = 3,
	};

	// Radius says how large area around origin you want to use, levels is how many hierarchical levels
	// you'll allow in the collision tree; sensitivity tells the engine what threshold to use to determine
	// what objects are still (= auto disabled in ODE).
	static PhysicsManager* Create(Engine pEngine, float pRadius, int pLevels, float pSensitivity);

	logclass();
};



}
