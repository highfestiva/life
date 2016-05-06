
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uicppcontextobject.h"



namespace UiCure {



class ExhaustEmitter {
public:
	ExhaustEmitter(cure::ResourceManager* resource_manager, GameUiManager* ui_manager);
	virtual ~ExhaustEmitter();

	void EmitFromTag(const CppContextObject* object, const uitbc::ChunkyClass::Tag& tag, float frame_time);

	cure::ResourceManager* resource_manager_;
	GameUiManager* ui_manager_;
	float exhaust_timeout_;

	logclass();
};



}
