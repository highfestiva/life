
// Author: Alexander Hugestrand, Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



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

bool SoundStream::IsOpen() const
{
	return mIsOpen;
}

void SoundStream::SetVolume(float pVolume)
{
	mVolume = pVolume;
}


}
