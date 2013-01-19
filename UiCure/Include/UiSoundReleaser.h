
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
	SoundReleaser(GameUiManager* pUiManager, Cure::ContextManager* pManager, UiCure::UserSound3dResource* pSound);
	virtual ~SoundReleaser();
	void OnTick();

private:
	UiCure::UserSound3dResource* mSound;
};



}
