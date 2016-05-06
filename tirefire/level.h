
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uicure/include/uimachine.h"
#include "game.h"



namespace cure {
class ContextPath;
}



namespace tirefire {



class Level: public UiCure::Machine {
public:
	typedef UiCure::Machine Parent;

	Level(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager);
	virtual ~Level();

	cure::ContextPath* QueryPath();

private:
	cure::ContextPath* path_;

	logclass();
};



}
