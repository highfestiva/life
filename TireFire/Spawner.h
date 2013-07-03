
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/CppContextObject.h"



namespace TireFire
{



class Spawner: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	Spawner(Cure::ContextManager* pManager);
	virtual ~Spawner();

private:
	virtual void SetSpawner(const TBC::PhysicsSpawner* pSpawner);

	LOG_CLASS_DECLARE();
};



}
