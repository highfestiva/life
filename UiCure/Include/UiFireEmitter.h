
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "UiCppContextObject.h"



namespace UiCure
{



class FireEmitter
{
public:
	FireEmitter(Cure::ResourceManager* pResourceManager, GameUiManager* pUiManager);
	virtual ~FireEmitter();

	void EmitFromTag(const CppContextObject* pObject, const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime);

	Cure::ResourceManager* mResourceManager;
	GameUiManager* mUiManager;
	float mInterleaveTimeout;

	LOG_CLASS_DECLARE();
};



}
