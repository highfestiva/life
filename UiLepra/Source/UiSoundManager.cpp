
// Author: Jonas Byström
//Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiSoundManagerFMod.h"
#include "../Include/UiSoundManagerOpenAL.h"



namespace UiLepra
{



SoundManager* SoundManager::CreateSoundManager(ContextType pType)
{
	if (pType == CONTEXT_FMOD)
	{
		return (new SoundManagerFMod(44100));
	}
	return (new SoundManagerOpenAL(44100));
}



SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}



}
