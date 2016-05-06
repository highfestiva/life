
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once



namespace uilepra {



class SoundManager;



class SoundStream {
public:
	SoundStream(SoundManager* sound_manager);
	virtual ~SoundStream();
	virtual bool Playback() = 0;
	virtual bool IsPlaying() const = 0;
	virtual bool Stop() = 0;
	virtual bool Pause() = 0;
	virtual bool Update() = 0;
	bool IsOpen() const;
	void SetVolume(float volume);

protected:
	SoundManager* sound_manager_;
	bool is_open_;
	bool is_looping_;
	float volume_;
};



}
