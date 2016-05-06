
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uicure/include/uimachine.h"
#include "game.h"



namespace bounce {



class Ball: public UiCure::Machine {
public:
	typedef UiCure::Machine Parent;

	Ball(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager);
	virtual ~Ball();

private:
	virtual void OnMicroTick(float frame_time);
	virtual void OnLoaded();

	logclass();
};



}
