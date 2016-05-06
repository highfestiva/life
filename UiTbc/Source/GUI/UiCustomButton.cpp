
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/gui/uicustombutton.h"



namespace uitbc {



CustomButton::CustomButton(const wstr& text):
	Parent(text),
	on_render_(0),
	is_over_(0) {
}

CustomButton::~CustomButton() {
	delete (on_render_);
	delete (is_over_);
}

void CustomButton::SetOnRenderDelegate(const Delegate& on_renderer) {
	on_render_ = new Delegate(on_renderer);
}

void CustomButton::SetIsOverDelegate(const DelegateXY& is_over) {
	is_over_ = new DelegateXY(is_over);
}

void CustomButton::Repaint(Painter*) {
	if (on_render_) {
		(*on_render_)(this);
	}
}

bool CustomButton::IsOver(int screen_x, int screen_y) {
	if (is_over_) {
		return (*is_over_)(this, screen_x, screen_y);
	}
	return Parent::IsOver(screen_x, screen_y);
}



}
