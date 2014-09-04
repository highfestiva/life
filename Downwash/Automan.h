
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/CppContextObject.h"
#include "DownwashManager.h"



namespace Downwash
{



class DownwashManager;



class Automan: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	Automan(Cure::GameManager* pGame, Cure::GameObjectId pCarId, const vec3& pDirection);
	virtual ~Automan();
	virtual void OnTick();

private:
	Cure::GameManager* mGame;
	Cure::GameObjectId mCarId;
	vec3 mDirection;
	StopWatch mStillTimer;
	logclass();
};



}
