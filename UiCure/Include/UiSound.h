
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



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
	void LoadPlaySound2d(UserSound2dResource* pSoundResource);
	void OnTick();

	UiCure::UserSound2dResource* mSoundResource;

	LOG_CLASS_DECLARE();
};



}
