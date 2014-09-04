
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiCppContextObject.h"



namespace UiCure
{



class JetEngineEmitter
{
public:
	JetEngineEmitter(Cure::ResourceManager* pResourceManager, GameUiManager* pUiManager);
	virtual ~JetEngineEmitter();

	void EmitFromTag(const CppContextObject* pObject, const UiTbc::ChunkyClass::Tag& pTag, float pFrameTime);

private:
	void DrawOvershoot(const vec3& pPosition, float pDistanceScaleFactor, const vec3& pRadius, const vec3& pColor, float pOpacity, const vec3& pCameraDirection);

	Cure::ResourceManager* mResourceManager;
	GameUiManager* mUiManager;
	float mInterleaveTimeout;

	logclass();
};



}
