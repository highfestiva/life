
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uimusicplayer.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/random.h"
#include "../../uilepra/include/uisoundstream.h"



namespace UiCure {



MusicPlayer::MusicPlayer(uilepra::SoundManager* sound_manager):
	sound_manager_(sound_manager),
	music_stream_(0),
	mode_(kModeIdle),
	current_song_index_(0x7FFFFFFF),
	volume_(1),
	pre_song_pause_time_(0),
	post_song_pause_time_(0) {
}

MusicPlayer::~MusicPlayer() {
	KillSong();
	sound_manager_ = 0;
}



void MusicPlayer::AddSong(const str& name) {
	song_name_list_.push_back(name);
}

void MusicPlayer::Shuffle() {
	SongNameList shuffeled_list;
	while (!song_name_list_.empty()) {
		const size_t index = Random::GetRandomNumber() % song_name_list_.size();
		const str& _name = song_name_list_[index];
		shuffeled_list.push_back(_name);
		song_name_list_.erase(song_name_list_.begin()+index);
	}
	song_name_list_ = shuffeled_list;
}

void MusicPlayer::SetVolume(float volume) {
	if (!Math::IsEpsEqual(volume_, volume)) {
		volume_ = volume;
		if (music_stream_) {
			music_stream_->SetVolume(volume_);
		}
	}
}

void MusicPlayer::SetSongPauseTime(float before, float after) {
	pre_song_pause_time_ = before;
	post_song_pause_time_ = after;
}

bool MusicPlayer::Playback() {
	if (mode_ == kModeIdle) {
		SetMode(kModePrePlayback);
	}
	return true;
}

bool MusicPlayer::Pause() {
	if (music_stream_) {
		return music_stream_->Pause();
	}
	return false;
}

bool MusicPlayer::Stop() {
	KillSong();
	SetMode(kModeIdle);
	return true;
}

bool MusicPlayer::Update() {
	mode_set_timer_.UpdateTimer();

	switch (mode_) {
		case kModeIdle:
		break;
		case kModePrePlayback: {
			if (mode_set_timer_.GetTimeDiff() >= pre_song_pause_time_) {
				StartPlayback();
				if (music_stream_) {
					SetMode(kModePlayback);
				} else {
					return false;
				}
			}
		} break;
		case kModePlayback: {
			deb_assert(music_stream_);
			if (!music_stream_) {
				return false;
			}
			music_stream_->Update();
			if (!music_stream_->IsPlaying()) {
				SetMode(kModePostPlayback);
			}
		} break;
		case kModePostPlayback: {
			KillSong();
			if (mode_set_timer_.GetTimeDiff() >= post_song_pause_time_) {
				SetMode(kModePrePlayback);
			}
		} break;
	}
	return true;
}



void MusicPlayer::SetMode(kMode mode) {
	mode_ = mode;
	mode_set_timer_.PopTimeDiff();
}

bool MusicPlayer::StartPlayback() {
	if (music_stream_) {
		delete music_stream_;
		music_stream_ = 0;
	}

	if (song_name_list_.empty()) {
		return false;
	}
	++current_song_index_;
	if (current_song_index_ >= song_name_list_.size()) {
		current_song_index_ = 0;
	}
	music_stream_ = sound_manager_->CreateSoundStream(song_name_list_[current_song_index_], uilepra::SoundManager::kLoopNone, 0);
	if (music_stream_) {
		music_stream_->SetVolume(volume_);
		return music_stream_->Playback();
	} else {
		log_.Errorf("Unable to load song %s!", song_name_list_[current_song_index_].c_str());
	}
	return false;
}

void MusicPlayer::KillSong() {
	delete music_stream_;
	music_stream_ = 0;
}


loginstance(kGame, MusicPlayer);



}
