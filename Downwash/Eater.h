
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/cppcontextobject.h"
#include "downwashmanager.h"


namespace Downwash {



class Eater: public cure::CppContextObject {
	typedef cure::CppContextObject Parent;
public:
	Eater(cure::ContextManager* manager);
	virtual ~Eater();

protected:
	virtual void OnTrigger(tbc::PhysicsManager::BodyID trigger_id, ContextObject* other_object, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal);

	logclass();
};



}
