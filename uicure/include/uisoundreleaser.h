
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uicppcontextobject.h"



namespace UiCure {



class SoundReleaser: public CppContextObject {
	typedef CppContextObject Parent;
public:
	SoundReleaser(cure::ResourceManager* resource_manager, GameUiManager* ui_manager, cure::ContextManager* manager,
		const str& sound_name, UiCure::UserSound3dResource* sound, const vec3& position, const vec3& velocity,
		float volume, float pitch);
	SoundReleaser(cure::ResourceManager* resource_manager, GameUiManager* ui_manager, cure::ContextManager* manager,
		const str& sound_name, UiCure::UserSound2dResource* sound, float volume, float pitch);
	virtual ~SoundReleaser();

private:
	void OnTick();
	void LoadPlaySound3d(UiCure::UserSound3dResource* sound_resource);
	void LoadPlaySound2d(UiCure::UserSound2dResource* sound_resource);

	UiCure::UserSound3dResource* sound3d_;
	UiCure::UserSound2dResource* sound2d_;
	vec3 position_;
	vec3 velocity_;
	float volume_;
	float pitch_;
};



}
