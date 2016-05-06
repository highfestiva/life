
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../life/lifeclient/explodingmachine.h"
#include "fire.h"



namespace Fire {



class BaseMachine: public life::ExplodingMachine {
	typedef life::ExplodingMachine Parent;
public:
	BaseMachine(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, life::Launcher* launcher);
	virtual ~BaseMachine();

	void AddPanic(float panic);
	float level_speed_;
	float panic_level_;
	str villain_;
	float dangerousness_;
	bool did_get_to_town_;

protected:
	virtual void OnAlarm(int alarm_id, void* extra_data);
	virtual void OnDie();
};



}
