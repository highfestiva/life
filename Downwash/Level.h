
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../life/lifeclient/level.h"
#include "downwash.h"



namespace cure {
class ContextPath;
}



namespace Downwash {



class Level: public life::Level {
	typedef life::Level Parent;
public:
	Level(cure::ResourceManager* resource_manager, const str& class_id, UiCure::GameUiManager* ui_manager, cure::ContextForceListener* gravel_emitter);
	virtual ~Level();

	virtual void OnLoaded();
	const str& GetBackgroundName() const;
	cure::ContextPath* QueryPath();

private:
	str background_name_;
	cure::ContextPath* path_;
};



}
