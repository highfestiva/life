
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



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

	LOG_CLASS_DECLARE();
};



}
