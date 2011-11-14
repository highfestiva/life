
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "../Include/UiSound.h"
#include "../../Cure/Include/ContextManager.h"
#include "../Include/UiGameUiManager.h"



namespace UiCure
{



Sound::Sound(Cure::ResourceManager* pResourceManager, const str& pSoundName, GameUiManager* pUiManager):
	Parent(pResourceManager, _T("Sound"), pUiManager)
{
	mSoundResource = new UiCure::UserSound2dResource(GetUiManager(), UiLepra::SoundManager::LOOP_NONE);
	mSoundResource->Load(GetResourceManager(), pSoundName,
		UiCure::UserSound2dResource::TypeLoadCallback(this, &Sound::LoadPlaySound2d));
}

Sound::~Sound()
{
	delete mSoundResource;
	mSoundResource = 0;
}



void Sound::LoadPlaySound2d(UserSound2dResource* pSoundResource)
{
	assert(pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
	if (pSoundResource->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
	{
		return;
	}
	mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), 0.7f, 1.0);
	GetManager()->EnableTickCallback(this);
}

void Sound::OnTick()
{
	if (!mUiManager->GetSoundManager()->IsPlaying(mSoundResource->GetData()))
	{
		GetManager()->PostKillObject(GetInstanceId());
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Sound);



}
