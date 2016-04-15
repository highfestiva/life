
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/UiSound.h"
#include "../../Cure/Include/ContextManager.h"
#include "../Include/UiGameUiManager.h"



namespace UiCure
{



Sound::Sound(Cure::ResourceManager* pResourceManager, const str& pSoundName, GameUiManager* pUiManager):
	Parent(pResourceManager, "Sound", pUiManager)
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



void Sound::SetManager(Cure::ContextManager* pManager)
{
	Parent::SetManager(pManager);
	GetManager()->EnableTickCallback(this);
}

void Sound::LoadPlaySound2d(UserSound2dResource* pSoundResource)
{
	deb_assert(pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
	if (pSoundResource->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
	{
		return;
	}
	mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), 0.7f, 1.0);
}

void Sound::OnTick()
{
	if (mSoundResource->GetLoadState() != Cure::RESOURCE_LOAD_IN_PROGRESS &&
		mSoundResource->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
	{
		GetManager()->PostKillObject(GetInstanceId());
	}
	else if (mSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE &&
		!mUiManager->GetSoundManager()->IsPlaying(mSoundResource->GetData()))
	{
		GetManager()->PostKillObject(GetInstanceId());
	}
}



loginstance(GAME_CONTEXT_CPP, Sound);



}
