
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "UiCppContextObject.h"



namespace UiCure
{



class ExhaustEmitter
{
public:
	ExhaustEmitter(Cure::ResourceManager* pResourceManager, GameUiManager* pUiManager, float pScale, float pAmount, float pLifeTime);
	virtual ~ExhaustEmitter();

	void EmitFromTag(const CppContextObject* pObject, const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime);

	Cure::ResourceManager* mResourceManager;
	GameUiManager* mUiManager;
	float mExhaustTimeout;
	float mScale;
	float mDelay;
	float mLifeTime;

	LOG_CLASS_DECLARE();
};



}
