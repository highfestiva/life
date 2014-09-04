
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiCppContextObject.h"



namespace UiCure
{



class SoundReleaser: public CppContextObject
{
	typedef CppContextObject Parent;
public:
	SoundReleaser(Cure::ResourceManager* pResourceManager, GameUiManager* pUiManager, Cure::ContextManager* pManager,
		const str& pSoundName, UiCure::UserSound3dResource* pSound, const vec3& pPosition, const vec3& pVelocity,
		float pVolume, float pPitch);
	SoundReleaser(Cure::ResourceManager* pResourceManager, GameUiManager* pUiManager, Cure::ContextManager* pManager,
		const str& pSoundName, UiCure::UserSound2dResource* pSound, float pVolume, float pPitch);
	virtual ~SoundReleaser();

private:
	void OnTick();
	void LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource);
	void LoadPlaySound2d(UiCure::UserSound2dResource* pSoundResource);

	UiCure::UserSound3dResource* mSound3d;
	UiCure::UserSound2dResource* mSound2d;
	vec3 mPosition;
	vec3 mVelocity;
	float mVolume;
	float mPitch;
};



}
