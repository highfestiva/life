
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once
#include "../../lepra/include/vector2d.h"
#include "uicure.h"

#define kStickResolution	10



namespace uilepra {
namespace touch {
class TouchstickInputDevice;
}
}



namespace UiCure {



class GameUiManager;



class DebugStick {
public:
	static void Init(GameUiManager* ui_manager);
	static void Draw();
	static bool IsUpdated();

	DebugStick();
	DebugStick(int pos_x, int pos_y);
	void Place(int pos_x, int pos_y);
	void SetDefaultValue(float x, float y);

	vec2 value_;

private:
	void InitStick();

	int stick_x_;
	int stick_y_;

	static GameUiManager* ui_manager_;
	static uilepra::touch::TouchstickInputDevice* touch_sticks_[kStickResolution][kStickResolution];
	static vec2 previous_values_[kStickResolution][kStickResolution];
	static bool is_default_value_set_[kStickResolution][kStickResolution];
	static bool is_initialized_;
	static bool is_updated_;
};



}
