
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uitouchstick.h"



namespace uilepra {
namespace touch {



using namespace lepra;



TouchstickInputElement::TouchstickInputElement(Type type, Interpretation interpretation, int type_index, TouchstickInputDevice* parent_device):
	InputElement(type, interpretation, type_index, parent_device),
	scale_(1),
	offset_(0) {
}

TouchstickInputElement::~TouchstickInputElement() {
}

float TouchstickInputElement::GetValueScaled() const {
	return (GetValue()-offset_) / scale_;
}

void TouchstickInputElement::SetValueScaled(float value) {
	SetValue(value*scale_ + offset_);
}

void TouchstickInputElement::SetScale(float minimum, float maximum) {
	offset_ = (maximum+minimum) / 2;
	scale_  = (maximum-minimum) / 2;	// Default scale is one, between -1 and +1.
}



TouchstickInputDevice* TouchstickInputDevice::GetByCoordinate(InputManager* manager, const PixelCoord& coord) {
	const InputManager::DeviceList& list = manager->GetDeviceList();
	InputManager::DeviceList::const_iterator x;
	for (x = list.begin(); x != list.end(); ++x) {
		TouchstickInputDevice* stick = dynamic_cast<TouchstickInputDevice*>(*x);
		if (stick) {
			if (stick->IncludesCoord(coord)) {
				return stick;
			}
		}
	}
	return 0;
}

TouchstickInputDevice::TouchstickInputDevice(InputManager* manager, InputMode mode, const PixelRect& area, int angle, int finger_radius):
	Parent(manager),
	mode_(mode),
	area_(area),
	touch_area_(area),
	angle_(angle),
	finger_radius_(finger_radius) {
	touch_area_.Shrink(finger_radius_);
	if (angle_ < -45) {
		angle_ += 360;
	}

	TouchstickInputElement* button = new TouchstickInputElement(InputElement::kDigital, InputElement::kButton, 0, this);
	TouchstickInputElement* x = new TouchstickInputElement(InputElement::kAnalogue, InputElement::kAbsoluteAxis, 0, this);
	TouchstickInputElement* y = new TouchstickInputElement(InputElement::kAnalogue, InputElement::kAbsoluteAxis, 1, this);
	SetIdentifier("Touchstick");
	button->SetIdentifier("Button");
	x->SetIdentifier("AxisX");
	y->SetIdentifier("AxisY");
	AddElement(button);
	AddElement(x);
	AddElement(y);

	GetManager()->AddInputDevice(this);
}

TouchstickInputDevice::~TouchstickInputDevice() {
	GetManager()->RemoveInputDevice(this);
}

bool TouchstickInputDevice::IsOwnedByManager() const {
	return false;
}



void TouchstickInputDevice::Move(const PixelRect& area, int angle) {
	area_ = area;
	touch_area_ = area;
	touch_area_.Shrink(finger_radius_);
	angle_ = angle;
	if (angle_ < -45) {
		angle_ += 360;
	}
}

const PixelRect& TouchstickInputDevice::GetArea() const {
	return area_;
}

int TouchstickInputDevice::GetFingerRadius() const {
	return finger_radius_;
}

void TouchstickInputDevice::SetFingerRadius(int finger_radius) {
	finger_radius_ = finger_radius;
}

void TouchstickInputDevice::ResetTap() {
	element_array_[0]->SetValue(0.0f);
	element_array_[1]->SetValue(0.0f);
	element_array_[2]->SetValue(0.0f);
}

void TouchstickInputDevice::SetTap(const PixelCoord& coord, bool is_press) {
	if (is_pressing_) {
		log_volatile(log_.Debugf("Movin': (%i; %i)", coord.x, coord.y));
	} else {
		start_ = coord;
	}
	is_pressing_ = is_press;

	const float dx = touch_area_.GetWidth()*0.5f;
	const float dy = touch_area_.GetHeight()*0.5f;
	float rx;
	float ry;
	switch (mode_) {
		default:
		case kModeRelativeCenter: {
			rx = (coord.x - area_.GetCenterX()) / dx;
			ry = (coord.y - area_.GetCenterY()) / dy;
		} break;
		case kModeRelativeStart: {
			rx = (coord.x - start_.x) / dx;
			ry = (coord.y - start_.y) / dy;
		} break;
		case kModeRelativeLast: {
			rx = (coord.x - start_.x) / dx;
			ry = (coord.y - start_.y) / dy;
			start_ = coord;
		} break;
	}

	rx = Math::Clamp(rx, -1.0f, +1.0f);
	ry = Math::Clamp(ry, -1.0f, +1.0f);
	// Handle the angles.
	if (angle_ < 45) {
	} else if (angle_ < 135) {
		ry = -ry;
		std::swap(rx, ry);
	} else if (angle_ < 225) {
		rx = -rx;
		ry = -ry;
	} else {
		rx = -rx;
		std::swap(rx, ry);
	}

	element_array_[0]->SetValue(is_pressing_? 1.0f : 0.0f);
	if (is_pressing_) {
		((TouchstickInputElement*)element_array_[1])->SetValueScaled(rx);
		((TouchstickInputElement*)element_array_[2])->SetValueScaled(ry);
	} else if (mode_ != kModeRelativeCenterNospring) {
		element_array_[1]->SetValue(0.0f);
		element_array_[2]->SetValue(0.0f);
	}
}

void TouchstickInputDevice::GetValue(float& x, float& y, bool& is_pressing) {
	x = ((TouchstickInputElement*)element_array_[1])->GetValueScaled();
	y = ((TouchstickInputElement*)element_array_[2])->GetValueScaled();
	is_pressing = element_array_[0]->GetBooleanValue();

	if (angle_ < 45) {
	} else if (angle_ < 135) {
		x = -x;
		std::swap(x, y);
	} else if (angle_ < 225) {
		x = -x;
		y = -y;
	} else {
		y = -y;
		std::swap(x, y);
	}
}

void TouchstickInputDevice::SetValue(float x, float y) {
	if (angle_ < 45) {
	} else if (angle_ < 135) {
		x = -x;
		std::swap(x, y);
	} else if (angle_ < 225) {
		x = -x;
		y = -y;
	} else {
		y = -y;
		std::swap(x, y);
	}
	((TouchstickInputElement*)element_array_[1])->SetValueScaled(x);
	((TouchstickInputElement*)element_array_[2])->SetValueScaled(y);
}

void TouchstickInputDevice::SetValueScale(float min_x, float max_x, float min_y, float max_y) {
	((TouchstickInputElement*)element_array_[1])->SetScale(min_x, max_x);
	((TouchstickInputElement*)element_array_[2])->SetScale(min_y, max_y);
}



void TouchstickInputDevice::AddElement(InputElement* element) {
	element_array_.push_back(element);
}



bool TouchstickInputDevice::IncludesCoord(const PixelCoord& coord) const {
	return area_.IsInside(coord.x, coord.y);
}



void TouchstickInputDevice::Activate() {
}

void TouchstickInputDevice::Release() {
}

void TouchstickInputDevice::PollEvents() {
}



loginstance(kUiInput, TouchstickInputDevice);



}
}
