
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/gui/uiradiobutton.h"



namespace uitbc {



RadioButton::RadioButton(const Color& body_color, const wstr& text):
	Parent(body_color, text) {
	SetBaseColor(GetColor());
}

RadioButton::~RadioButton() {
}

void RadioButton::SetBaseColor(const Color& color) {
	Parent::SetBaseColor(color);
	press_color_ = hoover_color_ * 1.6f;
}

bool RadioButton::OnLButtonUp(int mouse_x, int mouse_y) {
	if (IsOver(mouse_x, mouse_y) == true) {
		Component* parent = GetParent();
		Layout* layout = parent->GetLayout();

		// Update all the other radio buttons.
		Component* child = layout->GetFirst();
		while (child != 0) {
			if (child != this && child->GetType() == Component::kRadiobutton) {
				RadioButton* button = (RadioButton*)child;
				button->SetPressed(false);
			}

			child = layout->GetNext();
		}

		SetPressed(true);
		Click(true);
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

Component::Type RadioButton::GetType() const {
	return Component::kRadiobutton;
}



}
