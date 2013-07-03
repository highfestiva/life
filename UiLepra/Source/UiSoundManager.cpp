
// Author: Jonas Byström
//Copyright (c) Pixel Doctrine


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
			return (new SoundManagerFMod(44100));
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



void SoundManager::SetSoundPosition(SoundInstanceID pSoundIID, const Vector3DF& pPos, const Vector3DF& pVel)
{
	ScopeLock lLock(&mLock);
	MicrophoneLocation& lLocation = mMicrophoneArray[mCurrentMicrophone];
	const Vector3DF lMicRelativePos = lLocation.mTransform.InverseTransform(pPos);
	const Vector3DF lMicRelativeVel = lLocation.mVelocityTransform.InverseTransform(pVel);
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

void SoundManager::SetListenerPosition(const Vector3DF& pPos, const Vector3DF& pVel,
	const Vector3DF& pUp, const Vector3DF& pForward)
{
	ScopeLock lLock(&mLock);
	const Vector3DF lLeft = pForward.Cross(pUp);
	RotationMatrixF lRotation(lLeft, pForward, pUp);
	MicrophoneLocation& lLocation = mMicrophoneArray[mCurrentMicrophone];
	lLocation.mTransform.SetPosition(pPos);
	lLocation.mTransform.SetOrientation(lRotation);
	lLocation.mVelocityTransform.SetPosition(pVel);
	lLocation.mVelocityTransform.SetOrientation(lRotation);
}



}
