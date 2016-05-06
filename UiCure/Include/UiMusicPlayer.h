
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uicure.h"
#include "../../lepra/include/hirestimer.h"
#include "../../uilepra/include/uisoundmanager.h"



namespace uilepra {
class SoundManager;
}



namespace UiCure {



class MusicPlayer {
public:
	MusicPlayer(uilepra::SoundManager* sound_manager);
	virtual ~MusicPlayer();

	void SetVolume(float volume);
	void SetSongPauseTime(float before, float after);
	void AddSong(const str& name);
	void Shuffle();
	bool Playback();
	bool Pause();
	bool Stop();
	bool Update();

private:
	enum kMode {
		kModeIdle = 1,
		kModePrePlayback,
		kModePlayback,
		kModePostPlayback,
	};

	typedef std::vector<str> SongNameList;

	void SetMode(kMode mode);
	bool StartPlayback();
	void KillSong();

	uilepra::SoundManager* sound_manager_;
	uilepra::SoundStream* music_stream_;
	kMode mode_;
	SongNameList song_name_list_;
	size_t current_song_index_;
	float volume_;
	float pre_song_pause_time_;
	float post_song_pause_time_;
	HiResTimer mode_set_timer_;

	logclass();
};



}
