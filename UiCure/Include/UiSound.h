
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiCppContextObject.h"



namespace UiCure
{



class Sound: public CppContextObject
{
public:
	typedef CppContextObject Parent;

	Sound(Cure::ResourceManager* pResourceManager, const str& pSoundName, GameUiManager* pUiManager);
	virtual ~Sound();

private:
	virtual void SetManager(Cure::ContextManager* pManager);

	void LoadPlaySound2d(UserSound2dResource* pSoundResource);
	void OnTick();

	UiCure::UserSound2dResource* mSoundResource;

	logclass();
};



}
