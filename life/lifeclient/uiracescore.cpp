
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "uiracescore.h"
#include "../../cure/include/contextmanager.h"
#include "stopwatch.h"



namespace life {



UiRaceScore::UiRaceScore(cure::ContextObject* context_object, const str& attribute_name, ScreenPart* screen_part,
	UiCure::GameUiManager* ui_manager, const str& class_resource_name):
	Parent(context_object, attribute_name, 0, tbc::INVALID_BODY) {
	StopWatch* stop_watch_child = new StopWatch(screen_part, ui_manager, class_resource_name, attribute_name);
	stop_watch_child->Start(context_object);
}

UiRaceScore::~UiRaceScore() {
}



}
