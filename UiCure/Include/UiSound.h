
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uicppcontextobject.h"



namespace UiCure {



class Sound: public CppContextObject {
public:
	typedef CppContextObject Parent;

	Sound(cure::ResourceManager* resource_manager, const str& sound_name, GameUiManager* ui_manager);
	virtual ~Sound();

private:
	virtual void SetManager(cure::ContextManager* manager);

	void LoadPlaySound2d(UserSound2dResource* sound_resource);
	void OnTick();

	UiCure::UserSound2dResource* sound_resource_;

	logclass();
};



}
