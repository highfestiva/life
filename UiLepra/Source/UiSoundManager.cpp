
// Author: Jonas Byström
//Copyright (c) 2002-2008, Righteous Games



#include "../Include/UiSoundManagerFMod.h"



namespace UiLepra
{



SoundManager* SoundManager::CreateSoundManager(ContextType /*pType*/)
{
	return (new SoundManagerFMod(44100));
}



SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}



}
