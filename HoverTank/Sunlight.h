
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Lepra/Include/HiResTimer.h"
#include "../UiTBC/Include/UiRenderer.h"
#include "HoverTank.h"



namespace UiCure
{
class GameUiManager;
}



namespace HoverTank
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

	void SetDirection(const Vector3DF& pDirection);
	void SetColor(const Vector3DF& pColor);

private:
	UiCure::GameUiManager* mUiManager;
	UiTbc::Renderer::LightID mLightId;
	Vector3DF mCamSunDirection;
	HiResTimer mTime;
	float mAngle;
};



}
