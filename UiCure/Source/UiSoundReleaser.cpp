
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiSoundReleaser.h"
#include "../../Cure/Include/ContextManager.h"
#include "../Include/UiGameUiManager.h"



namespace UiCure
{



SoundReleaser::SoundReleaser(GameUiManager* pUiManager, Cure::ContextManager* pManager, UiCure::UserSound3dResource* pSound):
	Parent(0, _T("SoundReleaser"), pUiManager),
	mSound(pSound)
{
	if (!mUiManager->GetSoundManager()->IsPlaying(mSound->GetData()))
	{
		delete this;
		return;
	}
	pManager->AddLocalObject(this);
	pManager->EnableTickCallback(this);
}

SoundReleaser::~SoundReleaser()
{
	delete mSound;
	mSound = 0;
}

void SoundReleaser::OnTick()
{
	if (!mUiManager->GetSoundManager()->IsPlaying(mSound->GetData()))
	{
		GetManager()->PostKillObject(GetInstanceId());
	}
}



}
