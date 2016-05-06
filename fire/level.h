
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../life/lifeclient/level.h"
#include "fire.h"



namespace cure {
class ContextPath;
}



namespace Fire {



class Level: public life::Level {
	typedef life::Level Parent;
public:
	Level(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, cure::ContextForceListener* gravel_emitter);
	virtual ~Level();

	virtual void OnLoaded();
	cure::ContextPath* QueryPath();
	float GetLevelSpeed() const;

private:
	cure::ContextPath* path_;
	float level_speed_;
};



}
