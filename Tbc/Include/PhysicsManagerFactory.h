
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// The sole purpose of this file is to not have to include ode.h, PhysX.h
// or any other junk file in the rest of the system (which fucks up path
// settings on all other projects).



#pragma once

#include "../../lepra/include/log.h"
#include "../include/tbc.h"



namespace tbc {



class PhysicsManager;



class PhysicsManagerFactory {
public:
	enum Engine {
		kEngineOde = 1,
		kEnginePhysx = 2,
		kEngineBullet = 3,
	};

	// Radius says how large area around origin you want to use, levels is how many hierarchical levels
	// you'll allow in the collision tree; sensitivity tells the engine what threshold to use to determine
	// what objects are still (= auto disabled in ODE).
	static PhysicsManager* Create(Engine engine, float radius, int levels, float sensitivity);

	logclass();
};



}
