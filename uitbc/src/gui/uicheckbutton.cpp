
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/gui/uicheckbutton.h"



namespace uitbc {



CheckButton::CheckButton(const Color& body_color, const wstr& text):
	Parent(body_color, text),
	checked_icon_id_(Painter::kInvalidImageid) {
}

CheckButton::~CheckButton() {
}

void CheckButton::Repaint(Painter* painter) {
	Parent::Repaint(painter);
}

bool CheckButton::OnLButtonUp(int mouse_x, int mouse_y) {
	if (IsOver(mouse_x, mouse_y) == true) {
		switch(GetState()) {
		case kReleasing:
			SetPressed(false);
			break;
		case kPressing:
			SetPressed(true);
			break;
		default:
			break;
		}
		if (on_click_ != 0) {
			(*on_click_)(this);
		}
	} else {
		// Go back to previous state.
		switch(GetState()) {
		case kReleasing:
			SetState(kPressed);
			break;
		case kPressing:
			SetState(kReleased);
			break;
		default:
			break;
		}
	}

	ReleaseMouseFocus();
	return true;
}

void CheckButton::SetCheckedIcon(Painter::ImageID icon_id) {
	checked_icon_id_ = icon_id;
}

Painter::ImageID CheckButton::GetCurrentIcon() const {
	State state = GetState();
	if (checked_icon_id_ != Painter::kInvalidImageid && (state == kPressed || state == kPressedHoover)) {
		return checked_icon_id_;
	}
	return Parent::GetCurrentIcon();
}



}
