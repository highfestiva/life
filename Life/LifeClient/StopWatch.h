
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../uicure/include/uicppcontextobject.h"
#include "../../uitbc/include/gui/uilabel.h"
#include "../life.h"



namespace life {



class ScreenPart;



class StopWatch: public UiCure::CppContextObject {
	typedef UiCure::CppContextObject Parent;
public:
	StopWatch(ScreenPart* screen_part, UiCure::GameUiManager* ui_manager, const str& class_resource_name,
		const str& attribute_name);
	virtual ~StopWatch();
	void Start(cure::ContextObject* parent);

protected:
	void OnTick();

private:
	ScreenPart* screen_part_;
	str attribute_name_;
	uitbc::Label label_;

	logclass();
};



}
