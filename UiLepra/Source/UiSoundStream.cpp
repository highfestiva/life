
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/UiSoundStream.h"



namespace UiLepra
{



SoundStream::SoundStream(SoundManager* pSoundManager):
	mSoundManager(pSoundManager),
	mIsOpen(false),
	mIsLooping(false),
	mVolume(1)
{
}

SoundStream::~SoundStream()
{
}

bool SoundStream::IsOpen() const
{
	return mIsOpen;
}

void SoundStream::SetVolume(float pVolume)
{
	mVolume = pVolume;
}


}
