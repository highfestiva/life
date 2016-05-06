
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../cure/include/runtimevariable.h"
#include "../../uilepra/include/uitouchstick.h"
#include "../../uitbc/include/uipainter.h"
#include "../include/uidebugstick.h"
#include "../include/uigameuimanager.h"

#define kStickDiameter	20



namespace UiCure {



void DebugStick::Init(GameUiManager* ui_manager) {
	if (!ui_manager_) {
		ui_manager_ = ui_manager;
		::memset(touch_sticks_, 0, sizeof(touch_sticks_));
		::memset(previous_values_, 0, sizeof(previous_values_));
		::memset(is_default_value_set_, 0, sizeof(is_default_value_set_));
		is_initialized_ = false;
	}
	is_updated_ = false;
}

void DebugStick::Draw() {
	bool is_debugging;
	v_get(is_debugging, =, ui_manager_->GetVariableScope(), kRtvarDebugEnable, false);
	if (!is_debugging) {
		if (is_initialized_) {
			is_initialized_ = false;
			for (int y = 0; y < kStickResolution; ++y) {
				for (int x = 0; x < kStickResolution; ++x) {
					delete touch_sticks_[y][x];
					touch_sticks_[y][x] = 0;
				}
			}
		}
		return;
	}
	for (int y = 0; y < kStickResolution; ++y) {
		for (int x = 0; x < kStickResolution; ++x) {
			if (touch_sticks_[y][x]) {
				ui_manager_->GetPainter()->SetColor(WHITE);
				PixelRect rect = touch_sticks_[y][x]->GetArea();
				ui_manager_->GetPainter()->DrawRect(rect);
				ui_manager_->GetPainter()->SetColor(RED);
				float tx;
				float ty;
				bool is_pressing;
				touch_sticks_[y][x]->GetValue(tx, ty, is_pressing);
				int ix = (int)(tx*(rect.GetWidth()/2-kStickDiameter/2)+rect.GetCenterX()) - kStickDiameter/2;
				int iy = (int)(ty*(rect.GetHeight()/2-kStickDiameter/2)+rect.GetCenterY()) - kStickDiameter/2;
				ui_manager_->GetPainter()->DrawArc(ix, iy, kStickDiameter, kStickDiameter, 0, 360, is_pressing);
			}
		}
	}
}

bool DebugStick::IsUpdated() {
	return is_updated_;
}



DebugStick::DebugStick():
	stick_x_(-1),
	stick_y_(-1) {
}

DebugStick::DebugStick(int pos_x, int pos_y) {
	Place(pos_x, pos_y);
}

void DebugStick::Place(int pos_x, int pos_y) {
	deb_assert(ui_manager_);
	deb_assert(pos_x >= 0 && pos_x < kStickResolution);
	deb_assert(pos_y >= 0 && pos_y < kStickResolution);
	stick_x_ = pos_x;
	stick_y_ = pos_y;
	value_ = previous_values_[stick_y_][stick_x_];

	bool is_debugging;
	v_get(is_debugging, =, ui_manager_->GetVariableScope(), kRtvarDebugEnable, false);
	if (!is_debugging) {
		return;
	}

	InitStick();
	float x;
	float y;
	bool is_pressing;
	touch_sticks_[stick_y_][stick_x_]->GetValue(x, y, is_pressing);
	if (is_pressing && (!Math::IsEpsEqual(x, value_.x) || !Math::IsEpsEqual(y, value_.y))) {
		value_.Set(x, y);
		previous_values_[stick_y_][stick_x_] = value_;
		is_updated_ = true;
	}
}

void DebugStick::SetDefaultValue(float x, float y) {
	if (!is_default_value_set_[stick_y_][stick_x_] && touch_sticks_[stick_y_][stick_x_]) {
		is_default_value_set_[stick_y_][stick_x_] = true;
		touch_sticks_[stick_y_][stick_x_]->SetValue(x, y);
		value_.Set(x, y);
		previous_values_[stick_y_][stick_x_] = value_;
		is_updated_ = true;
	}
}

void DebugStick::InitStick() {
	if (touch_sticks_[stick_y_][stick_x_]) {
		return;
	}
	int w = ui_manager_->GetCanvas()->GetWidth() / kStickResolution;
	int h = ui_manager_->GetCanvas()->GetHeight() / kStickResolution;
	PixelRect rect(w*stick_x_, h*stick_y_, w*(stick_x_+1), h*(stick_y_+1));
	touch_sticks_[stick_y_][stick_x_] = new uilepra::touch::TouchstickInputDevice(ui_manager_->GetInputManager(), uilepra::touch::TouchstickInputDevice::kModeRelativeCenterNospring, rect, 0, kStickDiameter/2);
	is_initialized_ = true;
}


GameUiManager* DebugStick::ui_manager_ = 0;
uilepra::touch::TouchstickInputDevice* DebugStick::touch_sticks_[kStickResolution][kStickResolution];
vec2 DebugStick::previous_values_[kStickResolution][kStickResolution];
bool DebugStick::is_default_value_set_[kStickResolution][kStickResolution];
bool DebugStick::is_initialized_ = false;
bool DebugStick::is_updated_ = false;



}
