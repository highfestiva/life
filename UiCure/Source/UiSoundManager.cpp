
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "../../Cure/Include/ContextObject.h"
#include "../../Lepra/Include/RotationMatrix.h"
#include "../../UiLepra/Include/UiSoundManagerFMod.h"
#include "../Include/UiSoundManager.h"
#include "../Include/UiCure.h"



namespace UiCure
{



SoundManager::SoundManager():
	mSound(UiLepra::SoundManager::CreateSoundManager(UiLepra::SoundManager::CONTEXT_FMOD)),
	mListenerCount(1)
{
}

SoundManager::~SoundManager()
{
	delete (mSound);
	mSound = 0;
}



void SoundManager::Tick(double pFrameTimeDelta)
{
	pFrameTimeDelta;
	mSound->Update();

	// Update sound attributes, and remove sounds that are no longer playing.
	std::list<ContextSound>::iterator x = mPlayList.begin();
	while (x != mPlayList.end())
	{
		if (mSound->IsPlaying((*x).mInstanceId))
		{
			Lepra::Vector3DF lPosition = (*x).mSource->GetPosition();
			Lepra::Vector3DF lVelocity(0, 0, 0);	// TODO: = (*y).mSource->GetVelocity();
			mSound->Set3DSoundAttributes((*x).mInstanceId, Lepra::Vector3DF((float)lPosition.x, (float)lPosition.y, (float)lPosition.z),
			Lepra::Vector3DF((float)lVelocity.x, (float)lVelocity.y, (float)lVelocity.z));
			++x;
		}
		else
		{
			mSound->DeleteSoundInstance((*x).mInstanceId);
			mSound->Release((*x).mId);
			mPlayList.erase(x++);
		}
	}
}



UiLepra::SoundManager* SoundManager::GetSoundManager() const
{
	return (mSound);
}



void SoundManager::SetListenerCount(int pListenerCount)
{
	mListenerCount = pListenerCount;
}

void SoundManager::SetActiveListener(int pListenerIndex)
{
	assert(pListenerIndex < mListenerCount);
	mSound->Set3dCurrentListener(pListenerIndex, mListenerCount);
}

void SoundManager::SetMicrophonePosition(const Lepra::Vector3DF& pPosition)
{
	Lepra::Vector3DF lOldPosition;
	Lepra::Vector3DF lVelocity;
	Lepra::Vector3DF lUp;
	Lepra::Vector3DF lForward;
	mSound->Get3DListenerAttributes(lOldPosition, lVelocity, lUp, lForward);
	mSound->Set3DListenerAttributes(Lepra::Vector3DF(pPosition.x, pPosition.y, pPosition.z), lVelocity, lUp, lForward);
}

void SoundManager::SetMicrophoneVelocity(const Lepra::Vector3DF& pVelocity)
{
	Lepra::Vector3DF lPosition;
	Lepra::Vector3DF lOldVelocity;
	Lepra::Vector3DF lUp;
	Lepra::Vector3DF lForward;
	mSound->Get3DListenerAttributes(lPosition, lOldVelocity, lUp, lForward);
	mSound->Set3DListenerAttributes(lPosition, Lepra::Vector3DF((float)pVelocity.x, (float)pVelocity.y, (float)pVelocity.z), lUp, lForward);
}

void SoundManager::SetMicrophoneOrientation(const Lepra::Vector3DF& pOrientation)
{
	Lepra::Vector3DF lUp(-1, 0, 0);
	Lepra::Vector3DF lForward(0, 0, 1);
	Lepra::RotationMatrixF lRotation;
	lRotation.MakeIdentity();
	lRotation.RotateAroundWorldY((float)pOrientation.y);
	lRotation.RotateAroundWorldZ((float)pOrientation.x);
	lRotation.RotateAroundOwnZ((float)pOrientation.z);
	lUp = lRotation*lUp;
	lForward = lRotation*lForward;
	Lepra::Vector3DF lOldOrientation;
	Lepra::Vector3DF lPosition;
	Lepra::Vector3DF lVelocity;
	mSound->Get3DListenerAttributes(lPosition, lVelocity, lOldOrientation, lOldOrientation);
	mSound->Set3DListenerAttributes(lPosition, lVelocity, lUp, lForward);
}



void SoundManager::Play3dSound(Cure::ContextObject* pSource, const Lepra::String& pName, double pVolume, double pPitch)
{
	bool lOk = true;
	UiLepra::SoundManager::SoundID lId = UiLepra::SoundManager::INVALID_SOUNDID;
	if (lOk)
	{
		lId = mSound->LoadSound3D(pName, UiLepra::SoundManager::LOOP_NONE, 0);
		lOk = (lId != UiLepra::SoundManager::INVALID_SOUNDID);
	}
	UiLepra::SoundManager::SoundInstanceID lInstanceId = UiLepra::SoundManager::INVALID_SOUNDINSTANCEID;
	if (lOk)
	{
		lInstanceId = mSound->CreateSoundInstance(lId);
		lOk = (lInstanceId != UiLepra::SoundManager::INVALID_SOUNDINSTANCEID);
	}
	if (lOk)
	{
		Lepra::Vector3DF lPosition = pSource->GetPosition();
		Lepra::Vector3DF lVelocity(0, 0, 0);	// TODO: = pSource->GetVelocity();
		mSound->Set3DSoundAttributes(lInstanceId, Lepra::Vector3DF((float)lPosition.x, (float)lPosition.y, (float)lPosition.z),
			Lepra::Vector3DF((float)lVelocity.x, (float)lVelocity.y, (float)lVelocity.z));
		mSound->Play(lInstanceId, (float)pVolume, (float)pPitch);

		mPlayList.push_back(ContextSound(pSource, lId, lInstanceId));
	}
}

void SoundManager::Stop3dSounds(Cure::ContextObject* pSource)
{
	std::list<ContextSound>::iterator x = mPlayList.begin();
	while (x != mPlayList.end())
	{
		if ((*x).mSource == pSource)
		{
			mPlayList.erase(x++);
		}
		else
		{
			++x;
		}
	}
}



}
