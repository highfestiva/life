
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../uicure/include/uimachine.h"
#include "../life.h"



namespace life {



class Launcher;



class ExplodingMachine: public UiCure::Machine {
public:
	typedef UiCure::Machine Parent;

	ExplodingMachine(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, Launcher* launcher);
	virtual ~ExplodingMachine();

	void SetExplosiveStrength(float explosive_strength);
	void SetDeathFrameDelay(int death_frame_delay);
	void SetDisappearAfterDeathDelay(float disappear_delay);

protected:
	virtual void OnTick();
	virtual void OnDie();

	Launcher* launcher_;
	int trigger_death_frame_;
	int death_frame_delay_;
	float disappear_after_death_delay_;
	bool is_detonated_;
	float explosive_strength_;

	logclass();
};



}
