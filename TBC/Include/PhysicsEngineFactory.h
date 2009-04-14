
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games

// The sole purpose of this file is to not have to include ode.h, PhysX.h
// or any other junk file in the rest of the system (which fucks up path
// settings on all other projects).



#ifndef PHYSICSENGINEFACTORY_H
#define PHYSICSENGINEFACTORY_H



namespace TBC
{



class PhysicsEngine;



class PhysicsEngineFactory
{
public:
	enum Engine
	{
		ENGINE_ODE = 1,
		ENGINE_PHYSX = 2,
	};

	static PhysicsEngine* Create(Engine pEngine);
};



}



#endif // !PHYSICSENGINEFACTORY_H
