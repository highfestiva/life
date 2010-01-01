
// Author: Jonas Byström
//Copyright (c) 2002-2009, Righteous Games



#include <assert.h>
#include "../Include/UiSoundManagerFMod.h"
#include "../Include/UiSoundManagerOpenAL.h"



namespace UiLepra
{



SoundManager* SoundManager::CreateSoundManager(ContextType pType)
{
	switch (pType)
	{
		case CONTEXT_FMOD:
#if !defined(LEPRA_WITHOUT_FMOD)
			return (new SoundManagerFMod(44100));
#endif // !WITHOUT_FMOD
		case CONTEXT_OPENAL:
			return (new SoundManagerOpenAL(44100));
	}
	assert(false);
	return (0);
}



SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}



}
