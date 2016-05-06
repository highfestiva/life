
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uisoundreleaser.h"
#include "../../cure/include/contextmanager.h"
#include "../include/uigameuimanager.h"



namespace UiCure {



SoundReleaser::SoundReleaser(cure::ResourceManager* resource_manager, GameUiManager* ui_manager, cure::ContextManager* manager,
	const str& sound_name, UiCure::UserSound3dResource* sound, const vec3& position, const vec3& velocity,
	float volume, float pitch):
	Parent(resource_manager, "SoundReleaser", ui_manager),
	sound3d_(sound),
	sound2d_(0),
	position_(position),
	velocity_(velocity),
	volume_(volume),
	pitch_(pitch) {
	manager->AddLocalObject(this);

	sound3d_->Load(GetResourceManager(), sound_name,
		UiCure::UserSound3dResource::TypeLoadCallback(this, &SoundReleaser::LoadPlaySound3d));
}

SoundReleaser::SoundReleaser(cure::ResourceManager* resource_manager, GameUiManager* ui_manager, cure::ContextManager* manager,
	const str& sound_name, UiCure::UserSound2dResource* sound, float volume, float pitch):
	Parent(resource_manager, "SoundReleaser", ui_manager),
	sound3d_(0),
	sound2d_(sound),
	volume_(volume),
	pitch_(pitch) {
	manager->AddLocalObject(this);

	sound2d_->Load(GetResourceManager(), sound_name,
		UiCure::UserSound2dResource::TypeLoadCallback(this, &SoundReleaser::LoadPlaySound2d));
}

SoundReleaser::~SoundReleaser() {
	delete sound3d_;
	sound3d_ = 0;
	delete sound2d_;
	sound2d_ = 0;
}

void SoundReleaser::OnTick() {
	if (sound3d_ && !ui_manager_->GetSoundManager()->IsPlaying(sound3d_->GetData())) {
		GetManager()->PostKillObject(GetInstanceId());
	}
	if (sound2d_ && !ui_manager_->GetSoundManager()->IsPlaying(sound2d_->GetData())) {
		GetManager()->PostKillObject(GetInstanceId());
	}
}

void SoundReleaser::LoadPlaySound3d(UiCure::UserSound3dResource* sound_resource) {
	//deb_assert(sound_resource->GetLoadState() == cure::kResourceLoadComplete);
	if (sound_resource->GetLoadState() == cure::kResourceLoadComplete) {
		ui_manager_->GetSoundManager()->SetSoundPosition(sound_resource->GetData(), position_, velocity_);
		ui_manager_->GetSoundManager()->Play(sound_resource->GetData(), volume_, pitch_);
		GetManager()->EnableTickCallback(this);
	} else {
		//deb_assert(false);
		GetManager()->PostKillObject(GetInstanceId());

	}
}

void SoundReleaser::LoadPlaySound2d(UiCure::UserSound2dResource* sound_resource) {
	deb_assert(sound_resource->GetLoadState() == cure::kResourceLoadComplete);
	if (sound_resource->GetLoadState() == cure::kResourceLoadComplete) {
		ui_manager_->GetSoundManager()->Play(sound_resource->GetData(), volume_, pitch_);
		GetManager()->EnableTickCallback(this);
	} else {
		deb_assert(false);
		GetManager()->PostKillObject(GetInstanceId());
	}
}



}
