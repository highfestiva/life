
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "fastprojectile.h"



namespace life {



class HomingProjectile: public FastProjectile {
public:
	typedef FastProjectile Parent;

	HomingProjectile(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, Launcher* launcher);
	virtual ~HomingProjectile();
	void SetTarget(cure::GameObjectId target);

protected:
	virtual void OnTick();

	cure::GameObjectId target_;

	logclass();
};



}
