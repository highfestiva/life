
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/cppcontextobject.h"



namespace grenaderun {



class Spawner: public cure::CppContextObject {
	typedef cure::CppContextObject Parent;
public:
	Spawner(cure::ContextManager* manager);
	virtual ~Spawner();

private:
	virtual void SetSpawner(const tbc::PhysicsSpawner* spawner);

	logclass();
};



}
