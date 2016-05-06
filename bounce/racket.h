
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uicure/include/uimachine.h"
#include "game.h"



namespace cure {
class ContextPath;
}



namespace bounce {



class Racket: public UiCure::Machine {
public:
	typedef UiCure::Machine Parent;

	Racket(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager);
	virtual ~Racket();

	cure::ContextPath* QueryPath();

private:
	cure::ContextPath* path_;

	logclass();
};



}
