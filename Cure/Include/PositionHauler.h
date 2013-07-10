
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "Cure.h"



namespace TBC
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
	static bool Get(ObjectPositionalData& pPosition, const TBC::PhysicsManager* pPhysicsManager, const TBC::ChunkyPhysics* pStructure, float pTotalMass);
	static void Set(const ObjectPositionalData& pPosition, TBC::PhysicsManager* pPhysicsManager, TBC::ChunkyPhysics* pStructure, float pTotalMass, bool pAllowMoveRoot);

private:
	LOG_CLASS_DECLARE();
};



}
