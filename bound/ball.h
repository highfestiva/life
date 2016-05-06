
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uicure/include/uimachine.h"
#include "bound.h"



namespace Bound {



class Ball: public UiCure::Machine {
	typedef UiCure::Machine Parent;
public:
	Ball(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager);
	virtual ~Ball();

	virtual void OnTick();

private:
	float average_speed_;
};



}
