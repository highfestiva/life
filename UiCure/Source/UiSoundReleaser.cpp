
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiSoundReleaser.h"
#include "../../Cure/Include/ContextManager.h"
#include "../Include/UiGameUiManager.h"



namespace UiCure
{



SoundReleaser::SoundReleaser(Cure::ResourceManager* pResourceManager, GameUiManager* pUiManager, Cure::ContextManager* pManager,
	const str& pSoundName, UiCure::UserSound3dResource* pSound, const Vector3DF& pPosition, const Vector3DF& pVelocity,
	float pVolume, float pPitch):
	Parent(pResourceManager, _T("SoundReleaser"), pUiManager),
	mSound(pSound),
	mPosition(pPosition),
	mVelocity(pVelocity),
	mVolume(pVolume),
	mPitch(pPitch)
{
	pManager->AddLocalObject(this);

	mSound->Load(GetResourceManager(), pSoundName,
		UiCure::UserSound3dResource::TypeLoadCallback(this, &SoundReleaser::LoadPlaySound3d));
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

void SoundReleaser::LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource)
{
	assert(pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
	if (pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetSoundManager()->SetSoundPosition(pSoundResource->GetData(), mPosition, mVelocity);
		mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), mVolume, mPitch);
		GetManager()->EnableTickCallback(this);
	}
	else
	{
		GetManager()->PostKillObject(GetInstanceId());
	}
}



}
