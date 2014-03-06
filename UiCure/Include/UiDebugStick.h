
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once
#include "../../Lepra/Include/Vector2D.h"
#include "UiCure.h"

#define STICK_RESOLUTION	10



namespace UiLepra
{
namespace Touch
{
class TouchstickInputDevice;
}
}



namespace UiCure
{



class GameUiManager;



class DebugStick
{
public:
	static void Init(GameUiManager* pInputManager);
	static void Draw();
	static bool IsUpdated();

	DebugStick();
	DebugStick(int pPosX, int pPosY);
	void Place(int pPosX, int pPosY);
	void SetDefaultValue(float x, float y);

	Vector2DF mValue;

private:
	void InitStick();

	int mStickX;
	int mStickY;

	static GameUiManager* mUiManager;
	static UiLepra::Touch::TouchstickInputDevice* mTouchSticks[STICK_RESOLUTION][STICK_RESOLUTION];
	static Vector2DF mPreviousValues[STICK_RESOLUTION][STICK_RESOLUTION];
	static bool mIsDefaultValueSet[STICK_RESOLUTION][STICK_RESOLUTION];
	static bool mIsUpdated;
};



}
