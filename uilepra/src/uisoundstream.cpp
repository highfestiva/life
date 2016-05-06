
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uisoundstream.h"



namespace uilepra {



SoundStream::SoundStream(SoundManager* sound_manager):
	sound_manager_(sound_manager),
	is_open_(false),
	is_looping_(false),
	volume_(1) {
}

SoundStream::~SoundStream() {
}

bool SoundStream::IsOpen() const {
	return is_open_;
}

void SoundStream::SetVolume(float volume) {
	volume_ = volume;
}


}
