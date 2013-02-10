
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "UiCppContextObject.h"



namespace UiCure
{



class SoundReleaser: public CppContextObject
{
	typedef CppContextObject Parent;
public:
	SoundReleaser(Cure::ResourceManager* pResourceManager, GameUiManager* pUiManager, Cure::ContextManager* pManager,
		const str& pSoundName, UiCure::UserSound3dResource* pSound, const Vector3DF& pPosition, const Vector3DF& pVelocity,
		float pVolume, float pPitch);
	virtual ~SoundReleaser();

private:
	void OnTick();
	void LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource);

	UiCure::UserSound3dResource* mSound;
	Vector3DF mPosition;
	Vector3DF mVelocity;
	float mVolume;
	float mPitch;
};



}
