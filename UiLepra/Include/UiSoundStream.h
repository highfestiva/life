
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once



namespace UiLepra
{



class SoundManager;



class SoundStream
{
public:
	SoundStream(SoundManager* pSoundManager);
	virtual ~SoundStream();
	virtual bool Playback() = 0;
	virtual bool IsPlaying() const = 0;
	virtual bool Stop() = 0;
	virtual bool Pause() = 0;
	virtual bool Update() = 0;
	bool IsOpen() const;
	void SetVolume(float pVolume);

protected:
	SoundManager* mSoundManager;
	bool mIsOpen;
	bool mIsLooping;
	float mVolume;
};



}
