/*
	Class:  Caption
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../../include/gui/uicaption.h"
#include "../../include/gui/uicenterlayout.h"
#include "../../include/gui/uistacklayout.h"

namespace uitbc {

Caption::Caption(const Color& active_color, const Color& inactive_color, int height) :
	RectComponent(active_color, new StackLayout(1)),
	label_(0),
	caption_rect_(0),
	button_rect_(0),
	left_image_rect_(0),
	center_image_rect_(0),
	right_image_rect_(0),
	left_button_(0),
	right_button_(0),
	middle_button_(0),
	active_tl_color_(active_color),
	active_tr_color_(active_color),
	active_bl_color_(active_color),
	active_br_color_(active_color),
	inactive_tl_color_(inactive_color),
	inactive_tr_color_(inactive_color),
	inactive_bl_color_(inactive_color),
	inactive_br_color_(inactive_color),
	active_left_image_id_(Painter::kInvalidImageid),
	active_right_image_id_(Painter::kInvalidImageid),
	active_center_image_id_(Painter::kInvalidImageid),
	inactive_left_image_id_(Painter::kInvalidImageid),
	inactive_right_image_id_(Painter::kInvalidImageid),
	inactive_center_image_id_(Painter::kInvalidImageid),
	text_(),
	active_text_backg_color_(255, 255, 255),
	inactive_text_color_(0, 0, 0),
	inactive_text_backg_color_(255, 255, 255),
	moving_window_(false),
	active_(true),
	style_(kSinglecolorStyle) {
	SetFontColor(OFF_BLACK);

	SetMinSize(0, height);
	SetPreferredSize(PixelCoord(0, height));

	label_ = new Label(GetTextColor(), text_);
	label_->SetPreferredHeight(height);

	RectComponent* top_layer = new RectComponent(new GridLayout(1, 2));
	top_layer->SetPreferredSize(0, 0);
	AddChild(top_layer);

	caption_rect_ = new RectComponent(new GridLayout(1, 1));
	caption_rect_->AddChild(label_);
	top_layer->AddChild(caption_rect_);

	RectComponent* button_rect = new RectComponent(new CenterLayout);
	button_rect->SetPreferredSize(0, 0);
	top_layer->AddChild(button_rect);

	button_rect_ = new RectComponent(Color(255, 255, 255), new GridLayout(1, 3));
	button_rect_->SetPreferredSize(1, 0);
	button_rect->AddChild(button_rect_);
}

Caption::Caption(const Color& active_top_left_color, const Color& active_top_right_color,
	const Color& active_bottom_left_color, const Color& active_bottom_right_color,
	const Color& inactive_top_left_color, const Color& inactive_top_right_color,
	const Color& inactive_bottom_left_color, const Color& inactive_bottom_right_color,
	int height) :
	RectComponent(active_top_left_color, active_top_right_color, active_bottom_right_color, active_bottom_left_color, new StackLayout(1)),
	label_(0),
	caption_rect_(0),
	button_rect_(0),
	left_image_rect_(0),
	center_image_rect_(0),
	right_image_rect_(0),
	left_button_(0),
	right_button_(0),
	middle_button_(0),
	active_tl_color_(active_top_left_color),
	active_tr_color_(active_top_right_color),
	active_bl_color_(active_bottom_left_color),
	active_br_color_(active_bottom_right_color),
	inactive_tl_color_(inactive_top_left_color),
	inactive_tr_color_(inactive_top_right_color),
	inactive_bl_color_(inactive_bottom_left_color),
	inactive_br_color_(inactive_bottom_right_color),
	active_left_image_id_(Painter::kInvalidImageid),
	active_right_image_id_(Painter::kInvalidImageid),
	active_center_image_id_(Painter::kInvalidImageid),
	inactive_left_image_id_(Painter::kInvalidImageid),
	inactive_right_image_id_(Painter::kInvalidImageid),
	inactive_center_image_id_(Painter::kInvalidImageid),
	text_(),
	active_text_backg_color_(255, 255, 255),
	inactive_text_color_(0, 0, 0),
	inactive_text_backg_color_(255, 255, 255),
	moving_window_(false),
	active_(true),
	style_(kMulticolorStyle) {
	SetFontColor(OFF_BLACK);

	SetMinSize(0, height);
	SetPreferredSize(PixelCoord(0, height));

	label_ = new Label(GetTextColor(), text_);
	label_->SetPreferredHeight(height);

	RectComponent* top_layer = new RectComponent(new GridLayout(1, 2));
	AddChild(top_layer);

	caption_rect_ = new RectComponent(new GridLayout(1, 1));
	caption_rect_->AddChild(label_);
	top_layer->AddChild(caption_rect_);

	RectComponent* button_rect = new RectComponent(new CenterLayout);
	button_rect->SetPreferredSize(0, 0);
	top_layer->AddChild(button_rect);

	button_rect_ = new RectComponent(Color(255, 255, 255), new GridLayout(1, 3));
	button_rect_->SetPreferredSize(1, 0);
	button_rect->AddChild(button_rect_);
}

Caption::Caption(Painter::ImageID active_left_image_id, Painter::ImageID active_right_image_id,
	Painter::ImageID active_center_image_id, Painter::ImageID inactive_left_image_id,
	Painter::ImageID inactive_right_image_id, Painter::ImageID inactive_center_image_id,
	int height) :
	RectComponent(new StackLayout(2)),
	label_(0),
	caption_rect_(0),
	button_rect_(0),
	left_image_rect_(0),
	center_image_rect_(0),
	right_image_rect_(0),
	left_button_(0),
	right_button_(0),
	middle_button_(0),
	active_tl_color_(0, 0, 0),
	active_tr_color_(0, 0, 0),
	active_bl_color_(0, 0, 0),
	active_br_color_(0, 0, 0),
	inactive_tl_color_(0, 0, 0),
	inactive_tr_color_(0, 0, 0),
	inactive_bl_color_(0, 0, 0),
	inactive_br_color_(0, 0, 0),
	active_left_image_id_(active_left_image_id),
	active_right_image_id_(active_right_image_id),
	active_center_image_id_(active_center_image_id),
	inactive_left_image_id_(inactive_left_image_id),
	inactive_right_image_id_(inactive_right_image_id),
	inactive_center_image_id_(inactive_center_image_id),
	text_(),
	active_text_backg_color_(255, 255, 255),
	inactive_text_color_(0, 0, 0),
	inactive_text_backg_color_(255, 255, 255),
	moving_window_(false),
	active_(true),
	style_(kImageStyle) {
	SetFontColor(OFF_BLACK);

	GUIImageManager* i_man = GetImageManager();

	PixelCoord left_image_size(i_man->GetImageSize(active_left_image_id));
	PixelCoord right_image_size(i_man->GetImageSize(active_right_image_id));
	PixelCoord center_image_size(i_man->GetImageSize(active_center_image_id));

	SetMinSize(left_image_size.x + right_image_size.x, height);
	SetPreferredSize(0, height);

	label_ = new Label(GetTextColor(), text_);
	label_->SetPreferredHeight(height);

	RectComponent* bottom_layer = new RectComponent(new GridLayout(1, 3));
	bottom_layer->SetPreferredSize(0, 0);
	AddChild(bottom_layer);

	RectComponent* top_layer = new RectComponent(new GridLayout(1, 2));
	top_layer->SetPreferredSize(0, 0);
	AddChild(top_layer);

	caption_rect_ = new RectComponent(new GridLayout(1, 1));
	caption_rect_->AddChild(label_);
	top_layer->AddChild(caption_rect_);

	RectComponent* button_rect = new RectComponent(new CenterLayout);
	button_rect->SetPreferredSize(0, 0);
	top_layer->AddChild(button_rect);

	button_rect_ = new RectComponent(new GridLayout(1, 3));
	button_rect_->SetPreferredSize(1, 0);
	button_rect->AddChild(button_rect_);

	left_image_rect_ = new RectComponent(active_left_image_id);
	left_image_rect_->SetPreferredSize(left_image_size);
	bottom_layer->AddChild(left_image_rect_, 0, 0);

	center_image_rect_ = new RectComponent(active_center_image_id);
	center_image_rect_->SetPreferredSize(0, center_image_size.y, false);
	bottom_layer->AddChild(center_image_rect_, 0, 1);

	right_image_rect_ = new RectComponent(active_right_image_id);
	right_image_rect_->SetPreferredSize(right_image_size);
	bottom_layer->AddChild(right_image_rect_, 0, 2);
}

Caption::~Caption() {
}

void Caption::InitCaption() {
}

Button* Caption::SetLeftButton(Button* button) {
	return SetButton(left_button_, button);
}

Button* Caption::SetRightButton(Button* button) {
	return SetButton(right_button_, button);
}

Button* Caption::SetMiddleButton(Button* button) {
	return SetButton(middle_button_, button);
}

Button* Caption::SetButton(Button*& member_button, Button* new_button) {
	Button* old = member_button;

	if (member_button != 0) {
		button_rect_->RemoveChild(member_button, 0);
		if (button_rect_->GetNumChildren() == 0) {
			button_rect_->SetPreferredSize(1, 0, true);
		}
	}

	member_button = new_button;

	if (member_button != 0) {
		button_rect_->AddChild(member_button, 0, 1);
		button_rect_->SetPreferredSize(0, 0, true);
		const int gap_pixels = 4;
		button_rect_->GetParent()->SetPreferredWidth(button_rect_->GetMinSize().x +
			button_rect_->GetNumChildren() * gap_pixels);
	}

	return old;
}

void Caption::ForceRepaint() {
	SetNeedsRepaint(true);
}

void Caption::SetActive(bool active) {
	if (active_ == active) {
		return;	// TRICKY: RAII simplifies.
	}

	active_ = active;

	GUIImageManager* i_man = GetImageManager();

	switch(style_) {
	case kSinglecolorStyle:
		if (active_ == true) {
			RectComponent::SetColor(active_tl_color_);
		} else {
			RectComponent::SetColor(inactive_tl_color_);
		} break;
	case kMulticolorStyle:
		if (active_ == true) {
			RectComponent::SetColor(active_tl_color_, active_tr_color_, active_br_color_, active_bl_color_);
		} else {
			RectComponent::SetColor(inactive_tl_color_, inactive_tr_color_, inactive_br_color_, inactive_bl_color_);
		} break;
	case kImageStyle: {
		if (active_ == true) {
			left_image_rect_->SetImage(active_left_image_id_);
			center_image_rect_->SetImage(active_center_image_id_);
			right_image_rect_->SetImage(active_right_image_id_);
		} else {
			left_image_rect_->SetImage(inactive_left_image_id_);
			center_image_rect_->SetImage(inactive_center_image_id_);
			right_image_rect_->SetImage(inactive_right_image_id_);
		}

		PixelCoord left_image_size(i_man->GetImageSize(active_left_image_id_));
		PixelCoord right_image_size(i_man->GetImageSize(active_right_image_id_));
		PixelCoord center_image_size(i_man->GetImageSize(active_center_image_id_));

		left_image_rect_->SetPreferredSize(left_image_size);
		center_image_rect_->SetPreferredSize(0, center_image_size.y);
		right_image_rect_->SetPreferredSize(right_image_size);
	} break;
	}

	if (active_ == true) {
		label_->SetFontColor(GetTextColor());
	} else {
		label_->SetFontColor(inactive_text_color_);
	}
}

bool Caption::OnLButtonDown(int mouse_x, int mouse_y) {
	Component::OnLButtonDown(mouse_x, mouse_y);
	Component* child = GetChild(mouse_x, mouse_y);

	while (child != 0 &&
		  child != left_button_ &&
		  child != right_button_ &&
		  child != middle_button_) {
		child = child->GetChild(mouse_x, mouse_y);
	}

	if (child == 0) {
		if (IsOver(mouse_x, mouse_y) == true) {
			moving_window_ = true;
			SetMouseFocus();
			return true;
		} else {
			return false;
		}
	}

	return true;
}

bool Caption::OnLButtonUp(int mouse_x, int mouse_y) {
	moving_window_ = false;
	ReleaseMouseFocus();
	return Component::OnLButtonUp(mouse_x, mouse_y);
}

bool Caption::OnMouseMove(int mouse_x, int mouse_y, int delta_x, int delta_y) {
	if (moving_window_ == true) {
		Window* parent_window = (Window*)Component::GetParentOfType(kWindow);

		if (parent_window != 0) {
			parent_window->SetPos(parent_window->GetPos() + PixelCoord(delta_x, delta_y));
		}

		return true;
	} else {
		return Component::OnMouseMove(mouse_x, mouse_y, delta_x, delta_y);
	}
}

void Caption::SetIcon(Painter::ImageID icon_id) {
	label_->SetIcon(icon_id, Label::kIconLeft);
}

void Caption::SetText(const wstr& text,
					  const Color& active_text_color,
					  const Color& active_backg_color,
					  const Color& inactive_text_color,
					  const Color& inactive_backg_color) {
	text_                   = text;
	active_text_backg_color_   = active_backg_color;
	inactive_text_color_      = inactive_text_color;
	inactive_text_backg_color_ = inactive_backg_color;
	SetFontColor(active_text_color);

	label_->SetText(text_);
}

bool Caption::Check(unsigned flags, unsigned pFlag) {
	return ((flags & pFlag) != 0);
}

Component::Type Caption::GetType() const {
	return Component::kCaption;
}

bool Caption::GetActive() const {
	return active_;
}



}
