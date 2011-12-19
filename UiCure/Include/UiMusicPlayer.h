
// Author: Jonas Bystr�m
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "UiCure.h"
#include "../../Lepra/Include/HiResTimer.h"
#include "../../UiLepra/Include/UiSoundManager.h"



namespace UiLepra
{
class UiLepra::SoundManager;
}



namespace UiCure
{



class MusicPlayer
{
public:
	MusicPlayer(UiLepra::SoundManager* pSoundManager);
	virtual ~MusicPlayer();

	void SetVolume(float pVolume);
	void SetSongPauseTime(float pBefore, float pAfter);
	void AddSong(const str& pName);
	void Shuffle();
	bool Playback();
	bool Pause();
	bool Stop();
	bool Update();

private:
	enum MODE
	{
		MODE_IDLE = 1,
		MODE_PRE_PLAYBACK,
		MODE_PLAYBACK,
		MODE_POST_PLAYBACK,
	};

	typedef std::vector<str> SongNameList;

	void SetMode(MODE pMode);
	bool StartPlayback();
	void KillSong();

	UiLepra::SoundManager* mSoundManager;
	UiLepra::SoundStream* mMusicStream;
	MODE mMode;
	SongNameList mSongNameList;
	size_t mCurrentSongIndex;
	float mVolume;
	float mPreSongPauseTime;
	float mPostSongPauseTime;
	HiResTimer mModeSetTimer;

	LOG_CLASS_DECLARE();
};



}
