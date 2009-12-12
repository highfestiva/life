
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#ifndef UICURESOUNDMANAGER_H
#define UICURESOUNDMANAGER_H



#include <list>
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/Vector3D.h"
#include "../../UiLepra/Include/UiSoundManager.h"
#include "../Include/UiCure.h"



namespace Cure
{
class ContextObject;
}



namespace UiCure
{



class SoundManager
{
public:
	SoundManager();
	virtual ~SoundManager();

	void Tick(double pFrameTimeDelta);

	UiLepra::SoundManager* GetSoundManager() const;

	void SetListenerCount(int pListenerCount);
	void SetActiveListener(int pListenerIndex);
	void SetMicrophonePosition(const Lepra::Vector3DF& pPosition);
	void SetMicrophoneVelocity(const Lepra::Vector3DF& pVelocity);
	void SetMicrophoneOrientation(const Lepra::Vector3DF& pOrientation);

	// A fire-and-forget player for 3D-positioned sounds.
	void Play3dSound(Cure::ContextObject* pSource, const Lepra::String& pName, double pVolume, double pPitch);
	void Stop3dSounds(Cure::ContextObject* pSource);

private:
	struct ContextSound
	{
		ContextSound()
		{
		};
		ContextSound(Cure::ContextObject* pSource, UiLepra::SoundManager::SoundID pId,
			UiLepra::SoundManager::SoundInstanceID pInstanceId):
			mSource(pSource),
			mId(pId),
			mInstanceId(pInstanceId)
		{
		};
		Cure::ContextObject* mSource;
		UiLepra::SoundManager::SoundID mId;
		UiLepra::SoundManager::SoundInstanceID mInstanceId;
	};

	UiLepra::SoundManager* mSound;
	int mListenerCount;
	std::list<ContextSound> mPlayList;
};



}



#endif // !UICURESOUNDMANAGER_H
