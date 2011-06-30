
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once



namespace UiLepra
{



class SoundStream
{
public:
	virtual bool Playback() = 0;
	virtual bool IsPlaying() const = 0;
	virtual bool Stop() = 0;
	virtual bool Update() = 0;
	bool IsOpen() const;

protected:
	bool mIsOpen;
	bool mIsLooping;
};



}
