
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../life.h"
#include "explodingmachine.h"



namespace life {



class Launcher;



class Mine: public ExplodingMachine {
public:
	typedef ExplodingMachine Parent;

	Mine(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, Launcher* launcher);
	virtual ~Mine();

	void EnableDeleteDetonation(bool enable);
	virtual void OnDie();

private:
	bool enable_delete_detonation_;

	logclass();
};



}
