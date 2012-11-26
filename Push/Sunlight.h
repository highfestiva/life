
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Lepra/Include/HiResTimer.h"
#include "../UiTBC/Include/UiRenderer.h"
#include "Push.h"



namespace UiCure
{
class GameUiManager;
}



namespace Push
{



class Sunlight
{
public:
	Sunlight(UiCure::GameUiManager* pUiManager);
	virtual ~Sunlight();

	void Tick(float pFactor);
	const Vector3DF& GetDirection() const;
	void AddSunColor(Vector3DF& pBaseColor, float pFactor) const;
	float GetTimeOfDay() const;	// Value from 0->1. 0 is 9 am.

	void SetDirection(float x, float y, float z);
	void SetColor(float r, float g, float b);

private:
	UiCure::GameUiManager* mUiManager;
	UiTbc::Renderer::LightID mLightId;
	Vector3DF mCamSunDirection;
	HiResTimer mTime;
	float mAngle;
};



}
