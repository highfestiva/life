
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uicure/include/uimachine.h"



namespace tirefire {



using namespace lepra;



class Vehicle: public UiCure::Machine {
public:
	typedef UiCure::Machine Parent;

	Vehicle(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager);
	virtual ~Vehicle();

	void DrainHealth(float drain);
	float GetHealth() const;
	bool QueryFlip();
	bool IsUpsideDown() const;

private:
	virtual void OnTick();

	float health_;
	int kill_joints_tick_count_;
	int wheel_expel_tick_count_;

	logclass();
};



}
