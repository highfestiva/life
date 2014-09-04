
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/CppContextObject.h"



namespace GrenadeRun
{



class Spawner: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	Spawner(Cure::ContextManager* pManager);
	virtual ~Spawner();

private:
	virtual void SetSpawner(const Tbc::PhysicsSpawner* pSpawner);

	logclass();
};



}
