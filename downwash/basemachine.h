
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../life/lifeclient/explodingmachine.h"
#include "downwash.h"



namespace Downwash {



class BaseMachine: public life::ExplodingMachine {
	typedef life::ExplodingMachine Parent;
public:
	BaseMachine(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, life::Launcher* launcher);
	virtual ~BaseMachine();

protected:
	virtual void OnDie();
};



}
