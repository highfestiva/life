
// Author: Jonas Byström
//Copyright (c) Pixel Doctrine


#include "pch.h"
#include "../../Lepra/Include/LepraAssert.h"
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
			//return (new SoundManagerFMod(44100));
#endif // !WITHOUT_FMOD
		case CONTEXT_OPENAL:
			return (new SoundManagerOpenAL(44100));
	}
	deb_assert(false);
	return (0);
}



SoundManager::SoundManager()
{
	SetCurrentListener(0, 1);
}

SoundManager::~SoundManager()
{
}



void SoundManager::SetSoundPosition(SoundInstanceID pSoundIID, const vec3& pPos, const vec3& pVel)
{
	ScopeLock lLock(&mLock);
	MicrophoneLocation& lLocation = mMicrophoneArray[mCurrentMicrophone];
	const vec3 lMicRelativePos = lLocation.mTransform.InverseTransform(pPos);
	const vec3 lMicRelativeVel = lLocation.mVelocityTransform.InverseTransform(pVel);
	DoSetSoundPosition(pSoundIID, lMicRelativePos, lMicRelativeVel);
}

void SoundManager::SetCurrentListener(int pListenerIndex, int pListenerCount)
{
	ScopeLock lLock(&mLock);
	deb_assert(pListenerIndex < pListenerCount);
	mCurrentMicrophone = pListenerIndex;
	if (mMicrophoneArray.size() != (size_t)pListenerCount)
	{
		mMicrophoneArray.resize(pListenerCount);
	}
}

void SoundManager::SetListenerPosition(const vec3& pPos, const vec3& pVel,
	const vec3& pUp, const vec3& pForward)
{
	ScopeLock lLock(&mLock);
	const vec3 lLeft = pForward.Cross(pUp);
	RotationMatrixF lRotation(lLeft, pForward, pUp);
	MicrophoneLocation& lLocation = mMicrophoneArray[mCurrentMicrophone];
	lLocation.mTransform.SetPosition(pPos);
	lLocation.mTransform.SetOrientation(lRotation);
	lLocation.mVelocityTransform.SetPosition(pVel);
	lLocation.mVelocityTransform.SetOrientation(lRotation);
}



}
