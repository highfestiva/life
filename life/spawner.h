
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/spawner.h"
#include "life.h"



namespace life {



class Spawner: public cure::Spawner {
	typedef cure::Spawner Parent;
public:
	Spawner(cure::ContextManager* manager);
	virtual ~Spawner();

private:
	virtual int GetSpawnCount() const;
};



}
