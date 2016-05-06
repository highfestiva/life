
// Author: Jonas Byström
//Copyright (c) Pixel Doctrine


#include "pch.h"
#include "../../lepra/include/lepraassert.h"
#include "../include/uisoundmanagerfmod.h"
#include "../include/uisoundmanageropenal.h"



namespace uilepra {



SoundManager* SoundManager::CreateSoundManager(ContextType type) {
	switch (type) {
		case kContextFmod:
#if !defined(LEPRA_WITHOUT_FMOD)
			//return (new SoundManagerFMod(44100));
#endif // !WITHOUT_FMOD
		case kContextOpenal:
			return (new SoundManagerOpenAL(44100));
	}
	deb_assert(false);
	return (0);
}



SoundManager::SoundManager() {
	SetCurrentListener(0, 1);
}

SoundManager::~SoundManager() {
}



void SoundManager::SetSoundPosition(SoundInstanceID sound_iid, const vec3& pos, const vec3& vel) {
	ScopeLock lock(&lock_);
	MicrophoneLocation& location = microphone_array_[current_microphone_];
	const vec3 mic_relative_pos = location.transform_.InverseTransform(pos);
	const vec3 mic_relative_vel = location.velocity_transform_.InverseTransform(vel);
	DoSetSoundPosition(sound_iid, mic_relative_pos, mic_relative_vel);
}

void SoundManager::SetCurrentListener(int listener_index, int listener_count) {
	ScopeLock lock(&lock_);
	deb_assert(listener_index < listener_count);
	current_microphone_ = listener_index;
	if (microphone_array_.size() != (size_t)listener_count) {
		microphone_array_.resize(listener_count);
	}
}

void SoundManager::SetListenerPosition(const vec3& pos, const vec3& vel,
	const vec3& up, const vec3& forward) {
	ScopeLock lock(&lock_);
	const vec3 left = forward.Cross(up);
	RotationMatrixF rotation(left, forward, up);
	MicrophoneLocation& location = microphone_array_[current_microphone_];
	location.transform_.SetPosition(pos);
	location.transform_.SetOrientation(rotation);
	location.velocity_transform_.SetPosition(vel);
	location.velocity_transform_.SetOrientation(rotation);
}



}
