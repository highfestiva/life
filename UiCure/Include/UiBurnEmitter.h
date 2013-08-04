
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiCppContextObject.h"



namespace UiCure
{



class BurnEmitter
{
public:
	BurnEmitter(Cure::ResourceManager* pResourceManager, GameUiManager* pUiManager);
	virtual ~BurnEmitter();

	void EmitFromTag(const CppContextObject* pObject, const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime, float pIntensity);

	Cure::ResourceManager* mResourceManager;
	GameUiManager* mUiManager;
	float mBurnTimeout;

	LOG_CLASS_DECLARE();
};



}