
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/gui/uifixedlayouter.h"
#include "../../include/gui/uidialog.h"



namespace uitbc {



FixedLayouter::FixedLayouter(Dialog* parent):
	dialog_(parent),
	content_width_part_(0.7f),
	content_height_part_(0.7f),
	max_row_count_(1),
	content_x_margin_(0),
	content_y_margin_(0) {
	const int margin = parent->GetPreferredHeight() / 30;
	SetContentMargin(margin);
}

FixedLayouter::~FixedLayouter() {
	dialog_ = 0;
}

void FixedLayouter::SetContentWidthPart(float content_width_part) {
	content_width_part_ = content_width_part;
}

void FixedLayouter::SetContentHeightPart(float content_height_part) {
	content_height_part_ = content_height_part;
}

void FixedLayouter::SetContentMargin(int content_margin) {
	SetContentXMargin(content_margin);
	SetContentYMargin(content_margin);
}

int FixedLayouter::GetContentXMargin() const {
	return content_x_margin_;
}

void FixedLayouter::SetContentXMargin(int content_x_margin) {
	content_x_margin_ = content_x_margin;
}

int FixedLayouter::GetContentYMargin() const {
	return content_y_margin_;
}

void FixedLayouter::SetContentYMargin(int content_y_margin) {
	content_y_margin_ = content_y_margin;
}

void FixedLayouter::AddComponent(Component* component, int r, int rc, int c, int cw, int cc) {
	max_row_count_ = std::max(max_row_count_, rc);

	component->UpdateLayout();
	dialog_->AddChild(component);

	const float full_width  = content_width_part_  * dialog_->GetPreferredWidth();
	const float full_height = content_height_part_ * dialog_->GetPreferredHeight();
	const float left = dialog_->GetPreferredWidth()/2 - full_width/2;
	const float top = dialog_->GetPreferredHeight()/2 - full_height/2;
	const float split_col_width  = full_width  / cc;
	const float split_row_height = full_height / rc;
	const float x_margin_count = float(cc-cw);
	const float y_margin_count = rc-1.0f;
	const float component_width  = split_col_width  - x_margin_count*content_x_margin_/cc;
	const float component_height = split_row_height - y_margin_count*content_y_margin_/rc;
	const float col_width_and_margin  = split_col_width  + content_x_margin_/2;	// Divide by two as half of the margin is already included in the evenly split size.
	const float row_height_and_margin = split_row_height + content_y_margin_/2;	// Divide by two as half of the margin is already included in the evenly split size.
	component->SetPos(int(left + c*col_width_and_margin), int(top + r*row_height_and_margin));
	component->SetSize(int(cw*component_width), (int)component_height);
	component->SetPreferredSize(component->GetSize(), false);
}

void FixedLayouter::AddWindow(Window* window, int r, int rc, int c, int cw, int cc) {
	AddComponent(window, r, rc, c, cw, cc);
	window->SetRoundedRadius(window->GetPreferredHeight() / 3);
}

void FixedLayouter::AddButton(Button* button, int tag, int r, int rc, int c, int cw, int cc, bool auto_dismiss) {
	deb_assert(tag < 0);	// Otherwise dialog auto-layouts.
	AddWindow(button, r, rc, c, cw, cc);
	button->SetHorizontalMargin(button->GetPreferredHeight() / 3);
	dialog_->SetButtonHandler(tag, button, auto_dismiss);
}

void FixedLayouter::AddCornerButton(Button* corner_button, int tag) {
	AddButton(corner_button, tag, 0, 2, 0, 1, 1, true);
	const int min_size = corner_button->GetRoundedRadius()*2+1;
	corner_button->SetPreferredSize(min_size, min_size);
	PixelCoord close_button_pos = dialog_->GetSize();
	const int r = dialog_->GetCornerRadius();
	close_button_pos.x += -min_size/2 - int(0.293*r);	// 0.293 ~ 1-1/sqrt(2)
	close_button_pos.y  = -min_size/2 + int(0.293*r);
	corner_button->SetPos(close_button_pos);
}



}
