
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "clientoptionsmanager.h"
#include "../../lepra/include/lepraassert.h"
#include "../../cure/include/runtimevariable.h"
#include "../../lepra/include/cyclicarray.h"
#include "../../lepra/include/math.h"
#include "clientoptions.h"
#include "rtvar.h"



namespace life {
namespace options {



ClientOptionsManager::ClientOptionsManager(cure::RuntimeVariableScope* variable_scope, int priority):
	OptionsManager(variable_scope, priority) {
	mouse_sensitivity_ = 4.0f;
	escape_ = 0;
	show_score_ = 0;
	::memset(&steering_control_, 0, sizeof(steering_control_));
	::memset(&cam_control_, 0, sizeof(cam_control_));
	::memset(&fire_control_, 0, sizeof(fire_control_));
	SetDefault(priority);
	DoRefreshConfiguration();
}



const bool ClientOptionsManager::IsEscape() const {
	return escape_ > 0.5f;
}

const bool ClientOptionsManager::GetShowScore() const {
	return show_score_ > 0.5f;
}

const Steering& ClientOptionsManager::GetSteeringControl() const {
	return steering_control_;
}

const CamControl& ClientOptionsManager::GetCamControl() const {
	return cam_control_;
}

const FireControl& ClientOptionsManager::GetFireControl() const {
	return fire_control_;
}



void ClientOptionsManager::DoRefreshConfiguration() {
	const KeyValue entries[] =
	{
		KeyValue(kRtvarCtrlUiContoggle, &console_toggle_),
		KeyValue(kRtvarCtrlUiEscape, &escape_),
		KeyValue(kRtvarCtrlUiShowscore, &show_score_),
		KeyValue(kRtvarCtrlSteerFwd, &steering_control_.control_[Steering::kControlForward]),
		KeyValue(kRtvarCtrlSteerBack, &steering_control_.control_[Steering::kControlBackward]),
		KeyValue(kRtvarCtrlSteerFwd3D, &steering_control_.control_[Steering::kControlForward3D]),
		KeyValue(kRtvarCtrlSteerBack3D, &steering_control_.control_[Steering::kControlBackward3D]),
		KeyValue(kRtvarCtrlSteerBrkback, &steering_control_.control_[Steering::kControlBrakeAndBack]),
		KeyValue(kRtvarCtrlSteerLeft, &steering_control_.control_[Steering::kControlLeft]),
		KeyValue(kRtvarCtrlSteerRight, &steering_control_.control_[Steering::kControlRight]),
		KeyValue(kRtvarCtrlSteerLeft3D, &steering_control_.control_[Steering::kControlLeft3D]),
		KeyValue(kRtvarCtrlSteerRight3D, &steering_control_.control_[Steering::kControlRight3D]),
		KeyValue(kRtvarCtrlSteerUp, &steering_control_.control_[Steering::kControlUp]),
		KeyValue(kRtvarCtrlSteerDown, &steering_control_.control_[Steering::kControlDown]),
		KeyValue(kRtvarCtrlSteerUp3D, &steering_control_.control_[Steering::kControlUp3D]),
		KeyValue(kRtvarCtrlSteerDown3D, &steering_control_.control_[Steering::kControlDown3D]),
		KeyValue(kRtvarCtrlSteerHandbrk, &steering_control_.control_[Steering::kControlHandbrake]),
		KeyValue(kRtvarCtrlSteerBrk, &steering_control_.control_[Steering::kControlBrake]),
		KeyValue(kRtvarCtrlSteerYaw, &steering_control_.control_[Steering::kControlYawAngle]),
		KeyValue(kRtvarCtrlSteerPitch, &steering_control_.control_[Steering::kControlPitchAngle]),
		KeyValue(kRtvarCtrlUiCamleft, &cam_control_.control_[CamControl::kCamdirLeft]),
		KeyValue(kRtvarCtrlUiCamright, &cam_control_.control_[CamControl::kCamdirRight]),
		KeyValue(kRtvarCtrlUiCamforward, &cam_control_.control_[CamControl::kCamdirForward]),
		KeyValue(kRtvarCtrlUiCambackward, &cam_control_.control_[CamControl::kCamdirBackward]),
		KeyValue(kRtvarCtrlUiCamup, &cam_control_.control_[CamControl::kCamdirUp]),
		KeyValue(kRtvarCtrlUiCamdown, &cam_control_.control_[CamControl::kCamdirDown]),
		KeyValue(kRtvarCtrlFire0, &fire_control_.control_[FireControl::kFire0]),
		KeyValue(kRtvarCtrlFire1, &fire_control_.control_[FireControl::kFire1]),
		KeyValue(kRtvarCtrlFire2, &fire_control_.control_[FireControl::kFire2]),
	};
	SetValuePointers(entries, LEPRA_ARRAY_COUNT(entries));

	v_tryget(mouse_sensitivity_, =(float), variable_scope_, kRtvarCtrlMousesensitivity, 4.0f);
}

bool ClientOptionsManager::UpdateInput(uilepra::InputElement* element) {
	bool value_set = false;
	float value = element->GetValue();
	const str input_element_name = element->GetFullName();
	if (element->GetType() == uilepra::InputElement::kAnalogue) {
		const bool is_relative = (element->GetInterpretation() == uilepra::InputElement::kRelativeAxis);
		// Update both sides for analogue input.
		if (is_relative) {
			value *= mouse_sensitivity_ * 0.03f;	// Relative values are more sensitive.
			value_set |= SetValue(input_element_name, value, true);
		} else if (Math::IsEpsEqual(value, 0.0f, 0.1f)) {
			// Clamp absolute+analogue to neutral when close enough.
			value_set |= SetValue(input_element_name+"+", 0, false);
			SetValue(input_element_name+"-", 0, false);
		}
		if (value > 0) {
			value_set |= SetValue(input_element_name+"+", value, is_relative);
			SetValue(input_element_name+"-", 0, false);
		} else if (value < 0) {
			value_set |= SetValue(input_element_name+"-", -value, is_relative);
			SetValue(input_element_name+"+", 0, false);
		}
	} else {
		value_set |= SetValue(input_element_name, value, false);
	}
	return value_set;
}



bool ClientOptionsManager::SetDefault(int priority) {
	bool ok = false;

	// TODO: check for installed devices, prioritize, etc.

	// Hmmm... Same cam controls for all split-screen clients. Swell.
	v_override(variable_scope_, kRtvarCtrlUiCamup, "Key.PgUp");
	v_override(variable_scope_, kRtvarCtrlUiCamdown, "Key.Insert");
	v_override(variable_scope_, kRtvarCtrlUiCamleft, "Key.Del");
	v_override(variable_scope_, kRtvarCtrlUiCamright, "Key.PgDown");
	v_override(variable_scope_, kRtvarCtrlUiCamforward, "Key.Home");
	v_override(variable_scope_, kRtvarCtrlUiCambackward, "Key.End");

	switch (priority) {
		case 0: {
			v_override(variable_scope_, kRtvarCtrlUiContoggle, "Key.Paragraph, Key.Acute, Key.Apostrophe");
			v_override(variable_scope_, kRtvarCtrlUiEscape, "Key.Esc");
			v_override(variable_scope_, kRtvarCtrlUiShowscore, "Key.Tab");
			v_override(variable_scope_, kRtvarCtrlSteerFwd, "Key.Up");
			v_override(variable_scope_, kRtvarCtrlSteerBack, "");
			v_override(variable_scope_, kRtvarCtrlSteerFwd3D, "");
			v_override(variable_scope_, kRtvarCtrlSteerBack3D, "");
			v_override(variable_scope_, kRtvarCtrlSteerBrkback, "Key.Down");
			v_override(variable_scope_, kRtvarCtrlSteerLeft, "Key.Left");
			v_override(variable_scope_, kRtvarCtrlSteerRight, "Key.Right");
			v_override(variable_scope_, kRtvarCtrlSteerLeft3D, "");
			v_override(variable_scope_, kRtvarCtrlSteerRight3D, "");
			v_override(variable_scope_, kRtvarCtrlSteerUp, "Key.Numpad4");
			v_override(variable_scope_, kRtvarCtrlSteerDown, "Key.Numpad1");
			v_override(variable_scope_, kRtvarCtrlSteerUp3D, "Key.Numpad4");
			v_override(variable_scope_, kRtvarCtrlSteerDown3D, "Key.Numpad1");
			v_override(variable_scope_, kRtvarCtrlSteerHandbrk, "Key.Numpad0");
			v_override(variable_scope_, kRtvarCtrlSteerBrk, "");
			v_override(variable_scope_, kRtvarCtrlSteerYaw, "");
			v_override(variable_scope_, kRtvarCtrlSteerPitch, "");
			v_override(variable_scope_, kRtvarCtrlFire0, "Key.End");
			v_override(variable_scope_, kRtvarCtrlFire1, "");
			v_override(variable_scope_, kRtvarCtrlFire2, "");
			ok = true;
		} break;
		case 1: {
			v_override(variable_scope_, kRtvarCtrlUiContoggle, "Key.ScrollLock");
			v_override(variable_scope_, kRtvarCtrlUiEscape, "Key.Esc");
			v_override(variable_scope_, kRtvarCtrlUiShowscore, "Key.Tab");
			v_override(variable_scope_, kRtvarCtrlSteerFwd, "Device0.Button1");
			v_override(variable_scope_, kRtvarCtrlSteerBack, "");
			v_override(variable_scope_, kRtvarCtrlSteerFwd3D, "Device0.AbsoluteAxis2-");
			v_override(variable_scope_, kRtvarCtrlSteerBack3D, "Device0.AbsoluteAxis2+");
			v_override(variable_scope_, kRtvarCtrlSteerBrkback, "Device0.Button0");
			v_override(variable_scope_, kRtvarCtrlSteerLeft, "Device0.AbsoluteAxis3-");
			v_override(variable_scope_, kRtvarCtrlSteerRight, "Device0.AbsoluteAxis3+");
			v_override(variable_scope_, kRtvarCtrlSteerLeft3D, "Device0.AbsoluteAxis1-");
			v_override(variable_scope_, kRtvarCtrlSteerRight3D, "Device0.AbsoluteAxis1+");
			v_override(variable_scope_, kRtvarCtrlSteerUp, "Device0.AbsoluteAxis0-");
			v_override(variable_scope_, kRtvarCtrlSteerDown, "Device0.AbsoluteAxis0+");
			v_override(variable_scope_, kRtvarCtrlSteerUp3D, "Device0.AbsoluteAxis0-");
			v_override(variable_scope_, kRtvarCtrlSteerDown3D, "Device0.AbsoluteAxis0+");
			v_override(variable_scope_, kRtvarCtrlSteerHandbrk, "Device0.Button5");
			v_override(variable_scope_, kRtvarCtrlSteerBrk, "");
			v_override(variable_scope_, kRtvarCtrlSteerYaw, "");
			v_override(variable_scope_, kRtvarCtrlSteerPitch, "");
			v_override(variable_scope_, kRtvarCtrlFire0, "Device0.Button2");
			v_override(variable_scope_, kRtvarCtrlFire1, "");
			v_override(variable_scope_, kRtvarCtrlFire2, "");
			ok = true;
		} break;
		case 2: {
			v_override(variable_scope_, kRtvarCtrlUiContoggle, "Key.F12");
			v_override(variable_scope_, kRtvarCtrlUiEscape, "Key.Esc");
			v_override(variable_scope_, kRtvarCtrlUiShowscore, "Key.Tab");
			v_override(variable_scope_, kRtvarCtrlSteerFwd, "Key.W");
			v_override(variable_scope_, kRtvarCtrlSteerBack, "");
			v_override(variable_scope_, kRtvarCtrlSteerFwd3D, "");
			v_override(variable_scope_, kRtvarCtrlSteerBack3D, "");
			v_override(variable_scope_, kRtvarCtrlSteerBrkback, "Key.S");
			v_override(variable_scope_, kRtvarCtrlSteerLeft, "Key.A");
			v_override(variable_scope_, kRtvarCtrlSteerRight, "Key.D");
			v_override(variable_scope_, kRtvarCtrlSteerLeft3D, "");
			v_override(variable_scope_, kRtvarCtrlSteerRight3D, "");
			v_override(variable_scope_, kRtvarCtrlSteerUp, "Key.E");
			v_override(variable_scope_, kRtvarCtrlSteerDown, "Key.Q");
			v_override(variable_scope_, kRtvarCtrlSteerUp3D, "Key.E");
			v_override(variable_scope_, kRtvarCtrlSteerDown3D, "Key.Q");
			v_override(variable_scope_, kRtvarCtrlSteerHandbrk, "Key.E");
			v_override(variable_scope_, kRtvarCtrlSteerBrk, "");
			v_override(variable_scope_, kRtvarCtrlSteerYaw, "");
			v_override(variable_scope_, kRtvarCtrlSteerPitch, "");
			v_override(variable_scope_, kRtvarCtrlFire0, "Key.LCtrl");
			v_override(variable_scope_, kRtvarCtrlFire1, "");
			v_override(variable_scope_, kRtvarCtrlFire2, "");
			ok = true;
		} break;
		case 3: {
			v_override(variable_scope_, kRtvarCtrlUiContoggle, "Key.Pause");
			v_override(variable_scope_, kRtvarCtrlUiEscape, "Key.Esc");
			v_override(variable_scope_, kRtvarCtrlUiShowscore, "Key.Tab");
			v_override(variable_scope_, kRtvarCtrlSteerFwd, "Key.Numpad8");
			v_override(variable_scope_, kRtvarCtrlSteerBack, "");
			v_override(variable_scope_, kRtvarCtrlSteerFwd3D, "");
			v_override(variable_scope_, kRtvarCtrlSteerBack3D, "");
			v_override(variable_scope_, kRtvarCtrlSteerBrkback, "Key.Numpad5");
			v_override(variable_scope_, kRtvarCtrlSteerLeft, "Key.Numpad4");
			v_override(variable_scope_, kRtvarCtrlSteerRight, "Key.Numpad6");
			v_override(variable_scope_, kRtvarCtrlSteerLeft3D, "");
			v_override(variable_scope_, kRtvarCtrlSteerRight3D, "");
			v_override(variable_scope_, kRtvarCtrlSteerUp, "Key.Numpad9");
			v_override(variable_scope_, kRtvarCtrlSteerDown, "Key.Numpad7");
			v_override(variable_scope_, kRtvarCtrlSteerUp3D, "Key.Numpad9");
			v_override(variable_scope_, kRtvarCtrlSteerDown3D, "Key.Numpad7");
			v_override(variable_scope_, kRtvarCtrlSteerHandbrk, "Key.Numpad9");
			v_override(variable_scope_, kRtvarCtrlSteerBrk, "Key.Numpad7");
			v_override(variable_scope_, kRtvarCtrlSteerYaw, "");
			v_override(variable_scope_, kRtvarCtrlSteerPitch, "");
			v_override(variable_scope_, kRtvarCtrlFire0, "Key.NumpadPlus");
			v_override(variable_scope_, kRtvarCtrlFire1, "");
			v_override(variable_scope_, kRtvarCtrlFire2, "");
			ok = true;
		} break;
	}
	return (ok);
}



void ClientOptionsManager::operator=(const ClientOptionsManager&) {
	deb_assert(false);
}



loginstance(kUiInput, ClientOptionsManager);



}
}
