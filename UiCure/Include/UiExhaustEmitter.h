
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiCppContextObject.h"



namespace UiCure
{



class ExhaustEmitter
{
public:
	ExhaustEmitter(Cure::ResourceManager* pResourceManager, GameUiManager* pUiManager);
	virtual ~ExhaustEmitter();

	void EmitFromTag(const CppContextObject* pObject, const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime);

	Cure::ResourceManager* mResourceManager;
	GameUiManager* mUiManager;
	float mExhaustTimeout;

	logclass();
};



}
