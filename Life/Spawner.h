
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/Spawner.h"
#include "Life.h"



namespace Life
{



class Spawner: public Cure::Spawner
{
	typedef Cure::Spawner Parent;
public:
	Spawner(Cure::ContextManager* pManager);
	virtual ~Spawner();

private:
	virtual int GetSpawnCount() const;
};



}
