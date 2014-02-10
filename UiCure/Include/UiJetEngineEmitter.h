
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
	void DrawOvershoot(const Vector3DF& pPosition, float pDistanceScaleFactor, const Vector3DF& pRadius, const Vector3DF& pColor, float pOpacity, const Vector3DF& pCameraDirection);

	Cure::ResourceManager* mResourceManager;
	GameUiManager* mUiManager;
	float mInterleaveTimeout;

	LOG_CLASS_DECLARE();
};



}
