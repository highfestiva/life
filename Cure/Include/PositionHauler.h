
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "Cure.h"



namespace Tbc
{
class ChunkyPhysics;
class PhysicsManager;
}



namespace Cure
{



class ObjectPositionalData;



class PositionHauler
{
public:
	static bool Get(ObjectPositionalData& pPosition, const Tbc::PhysicsManager* pPhysicsManager, const Tbc::ChunkyPhysics* pStructure, float pTotalMass);
	static void Set(const ObjectPositionalData& pPosition, Tbc::PhysicsManager* pPhysicsManager, Tbc::ChunkyPhysics* pStructure, float pTotalMass, bool pAllowMoveRoot);

private:
	logclass();
};



}
