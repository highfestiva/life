
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uisound.h"
#include "../../cure/include/contextmanager.h"
#include "../include/uigameuimanager.h"



namespace UiCure {



Sound::Sound(cure::ResourceManager* resource_manager, const str& sound_name, GameUiManager* ui_manager):
	Parent(resource_manager, "Sound", ui_manager) {
	sound_resource_ = new UiCure::UserSound2dResource(GetUiManager(), uilepra::SoundManager::kLoopNone);
	sound_resource_->Load(GetResourceManager(), sound_name,
		UiCure::UserSound2dResource::TypeLoadCallback(this, &Sound::LoadPlaySound2d));
}

Sound::~Sound() {
	delete sound_resource_;
	sound_resource_ = 0;
}



void Sound::SetManager(cure::ContextManager* manager) {
	Parent::SetManager(manager);
	GetManager()->EnableTickCallback(this);
}

void Sound::LoadPlaySound2d(UserSound2dResource* sound_resource) {
	deb_assert(sound_resource->GetLoadState() == cure::kResourceLoadComplete);
	if (sound_resource->GetLoadState() != cure::kResourceLoadComplete) {
		return;
	}
	ui_manager_->GetSoundManager()->Play(sound_resource->GetData(), 0.7f, 1.0);
}

void Sound::OnTick() {
	if (sound_resource_->GetLoadState() != cure::kResourceLoadInProgress &&
		sound_resource_->GetLoadState() != cure::kResourceLoadComplete) {
		GetManager()->PostKillObject(GetInstanceId());
	} else if (sound_resource_->GetLoadState() == cure::kResourceLoadComplete &&
		!ui_manager_->GetSoundManager()->IsPlaying(sound_resource_->GetData())) {
		GetManager()->PostKillObject(GetInstanceId());
	}
}



loginstance(kGameContextCpp, Sound);



}
