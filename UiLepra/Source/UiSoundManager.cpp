
// Author: Jonas Bystr�m
//Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiSoundManagerFMod.h"
#include "../Include/UiSoundManagerOpenAL.h"



namespace UiLepra
{



SoundManager* SoundManager::CreateSoundManager(ContextType pType)
{
	((void*)pType);
#if !defined(LEPRA_WITHOUT_FMOD)
	if (pType == CONTEXT_FMOD)
	{
		return (new SoundManagerFMod(44100));
	}
#endif // !WITHOUT_FMOD
	return (new SoundManagerOpenAL(44100));
}



SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}



}
