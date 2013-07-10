
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/CppContextObject.h"
#include "HeliForceManager.h"



namespace HeliForce
{



class HeliForceManager;



class Automan: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	Automan(Cure::GameManager* pGame, Cure::GameObjectId pCarId, const Vector3DF& pDirection);
	virtual ~Automan();
	virtual void OnTick();

private:
	Cure::GameManager* mGame;
	Cure::GameObjectId mCarId;
	Vector3DF mDirection;
	StopWatch mStillTimer;
	LOG_CLASS_DECLARE();
};



}
