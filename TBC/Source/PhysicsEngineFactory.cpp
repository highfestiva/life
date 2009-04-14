
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include "../Include/PhysicsEngineFactory.h"
#include "../Include/PhysicsEngineODE.h"



namespace TBC
{



PhysicsEngine* PhysicsEngineFactory::Create(Engine pEngine)
{
	PhysicsEngine* lEngine = 0;
	switch (pEngine)
	{
		case ENGINE_ODE:	lEngine = new PhysicsEngineODE();	break;
	}
	return (lEngine);
}



}
