
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

// The sole purpose of this file is to not have to include ode.h, PhysX.h
// or any other junk file in the rest of the system (which fucks up path
// settings on all other projects).



#pragma once

#include "../../Lepra/Include/Log.h"



namespace TBC
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

	static PhysicsManager* Create(Engine pEngine);

	LOG_CLASS_DECLARE();
};



}
