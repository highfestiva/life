
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/physicsmanagerfactory.h"
#include "../include/physicsmanagerode.h"



namespace tbc {



PhysicsManager* PhysicsManagerFactory::Create(Engine engine, float radius, int levels, float sensitivity) {
	PhysicsManager* _engine = 0;
	switch (engine) {
		case kEngineOde:	_engine = new PhysicsManagerODE(radius, levels, sensitivity);	break;
		default:		log_.Error("Physics type not supported.");				break;
	}
	return (_engine);
}



loginstance(kPhysics, PhysicsManagerFactory);



}
