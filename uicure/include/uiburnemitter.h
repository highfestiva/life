
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uicppcontextobject.h"



namespace UiCure {



class BurnEmitter {
public:
	BurnEmitter(cure::ResourceManager* resource_manager, GameUiManager* ui_manager);
	virtual ~BurnEmitter();

	void EmitFromTag(const CppContextObject* object, const uitbc::ChunkyClass::Tag& tag, float frame_time, float intensity);
	void SetFreeFlow();

	cure::ResourceManager* resource_manager_;
	GameUiManager* ui_manager_;
	float burn_timeout_;
	bool free_flow_;

	logclass();
};



}
