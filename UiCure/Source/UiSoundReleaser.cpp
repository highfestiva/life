
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/UiSoundReleaser.h"
#include "../../Cure/Include/ContextManager.h"
#include "../Include/UiGameUiManager.h"



namespace UiCure
{



SoundReleaser::SoundReleaser(Cure::ResourceManager* pResourceManager, GameUiManager* pUiManager, Cure::ContextManager* pManager,
	const str& pSoundName, UiCure::UserSound3dResource* pSound, const vec3& pPosition, const vec3& pVelocity,
	float pVolume, float pPitch):
	Parent(pResourceManager, _T("SoundReleaser"), pUiManager),
	mSound3d(pSound),
	mSound2d(0),
	mPosition(pPosition),
	mVelocity(pVelocity),
	mVolume(pVolume),
	mPitch(pPitch)
{
	pManager->AddLocalObject(this);

	mSound3d->Load(GetResourceManager(), pSoundName,
		UiCure::UserSound3dResource::TypeLoadCallback(this, &SoundReleaser::LoadPlaySound3d));
}

SoundReleaser::SoundReleaser(Cure::ResourceManager* pResourceManager, GameUiManager* pUiManager, Cure::ContextManager* pManager,
	const str& pSoundName, UiCure::UserSound2dResource* pSound, float pVolume, float pPitch):
	Parent(pResourceManager, _T("SoundReleaser"), pUiManager),
	mSound3d(0),
	mSound2d(pSound),
	mVolume(pVolume),
	mPitch(pPitch)
{
	pManager->AddLocalObject(this);

	mSound2d->Load(GetResourceManager(), pSoundName,
		UiCure::UserSound2dResource::TypeLoadCallback(this, &SoundReleaser::LoadPlaySound2d));
}

SoundReleaser::~SoundReleaser()
{
	delete mSound3d;
	mSound3d = 0;
	delete mSound2d;
	mSound2d = 0;
}

void SoundReleaser::OnTick()
{
	if (mSound3d && !mUiManager->GetSoundManager()->IsPlaying(mSound3d->GetData()))
	{
		GetManager()->PostKillObject(GetInstanceId());
	}
	if (mSound2d && !mUiManager->GetSoundManager()->IsPlaying(mSound2d->GetData()))
	{
		GetManager()->PostKillObject(GetInstanceId());
	}
}

void SoundReleaser::LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource)
{
	deb_assert(pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
	if (pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetSoundManager()->SetSoundPosition(pSoundResource->GetData(), mPosition, mVelocity);
		mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), mVolume, mPitch);
		GetManager()->EnableTickCallback(this);
	}
	else
	{
		deb_assert(false);
		GetManager()->PostKillObject(GetInstanceId());
	}
}

void SoundReleaser::LoadPlaySound2d(UiCure::UserSound2dResource* pSoundResource)
{
	deb_assert(pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
	if (pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), mVolume, mPitch);
		GetManager()->EnableTickCallback(this);
	}
	else
	{
		deb_assert(false);
		GetManager()->PostKillObject(GetInstanceId());
	}
}



}
