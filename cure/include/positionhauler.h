
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "cure.h"



namespace tbc {
class ChunkyPhysics;
class PhysicsManager;
}



namespace cure {



class ObjectPositionalData;



class PositionHauler {
public:
	static bool Get(ObjectPositionalData& position, const tbc::PhysicsManager* physics_manager, const tbc::ChunkyPhysics* structure, float total_mass);
	static void Set(const ObjectPositionalData& position, tbc::PhysicsManager* physics_manager, tbc::ChunkyPhysics* structure, float total_mass, bool allow_move_root);

private:
	logclass();
};



}
