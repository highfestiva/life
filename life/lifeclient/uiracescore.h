
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../racescore.h"



namespace UiCure {
class GameUiManager;
}



namespace life {



class ScreenPart;



class UiRaceScore: public RaceScore {
	typedef RaceScore Parent;
public:
	UiRaceScore(cure::ContextObject* context_object, const str& attribute_name, ScreenPart* screen_part,
		UiCure::GameUiManager* ui_manager, const str& class_resource_name);
	virtual ~UiRaceScore();

private:
};



}
