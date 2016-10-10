
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/gui/uidialog.h"
#include "../../include/gui/uidesktopwindow.h"

#define kSpeed		30
#define kAcceleration	20
#define kMargin		0



namespace uitbc {



Dialog::Dialog(Component* parent, Action target):
	Parent(WHITE, new FloatingLayout),
	label_(0),
	target_(target),
	clicked_button_(0),
	is_closing_(false),
	animation_step_(0),
	direction_(0) {
	SetCornerRadius(20);
	color_[1] = DARK_GRAY;
	parent->AddChild(this);

	const PixelCoord& parent_size = GetParent()->GetSize();
	PixelCoord __size = parent_size - PixelCoord(100, 100);
	SetSize(__size);
	SetPreferredSize(__size);

	SetDirection(+1, true);
}

Dialog::~Dialog() {
	target_.clear();
	pre_click_target_.clear();
	post_click_target_.clear();
}

void Dialog::Dismiss() {
	if (!is_closing_) {
		GetTopParent()->ReleaseKeyboardFocus(kRecurseDown);
		animation_step_ = -kSpeed * direction_;
		is_closing_ = true;
	}
}



void Dialog::SetColor(const Color& top_left_color, const Color& top_right_color, const Color& bottom_right_color, const Color& bottom_left_color) {
	Parent::SetColor(top_left_color, top_right_color, bottom_right_color, bottom_left_color);
	if (label_) {
		label_->SetText(label_->GetText());
	}
}

void Dialog::SetDirection(int direction, bool set_pos) {
	direction_ = direction;

	const PixelCoord& parent_size = GetParent()->GetSize();
	const PixelCoord& __size = GetSize();
	const int start_x = (direction_ > 0)? parent_size.x+kMargin : -__size.x-kMargin;

	// Calculate what animation speed is required to get right end speed using the acceleration.
	const int target_x = parent_size.x/2 - __size.x/2;
	int x = target_x;
	animation_step_ = -kSpeed * direction_;
	for (int i = 0; i < 1000; ++i) {
		x -= animation_step_;
		if ((direction_ > 0)? (x >= start_x) : (x <= start_x)) {
			break;
		}
		animation_step_ -= kAcceleration * direction_;
	}
	if (set_pos) {
		SetPos(x, parent_size.y/2 - __size.y/2);
	}
}

void Dialog::SetPreClickTarget(Action pre_click_target) {
	pre_click_target_ = pre_click_target;
}

void Dialog::SetPostClickTarget(Action post_click_target) {
	post_click_target_ = post_click_target;
}

void Dialog::Center() {
	if (animation_step_) {
		return;
	}
	const PixelCoord& parent_size = GetParent()->GetSize();
	const PixelCoord& __size = GetSize();
	SetPos(parent_size.x/2 - __size.x/2, parent_size.y/2 - __size.y/2);
}

Label* Dialog::SetQueryLabel(const wstr& text, uitbc::FontManager::FontId font_id) {
	if (!label_) {
		label_ = new uitbc::Label(color_[1], text);
	}
	label_->SetFontId(font_id);
	//label_->ActivateFont(_painter);
	label_->SetText(text);
	uitbc::Painter* _painter = ((DesktopWindow*)GetParentOfType(kDesktopwindow))->GetPainter();
	const int w = _painter->GetStringWidth(text);
	const int h = _painter->GetFontHeight();
	label_->SetPreferredSize(w+2, h);
	AddChild(label_);
	// Set position.
	PixelCoord coord;
	const PixelCoord __size = GetSize();
	coord.x = __size.x/2 - w/2;
	coord.y = __size.y/3 - h;
	label_->SetPos(coord+offset_);
	//label_->DeactivateFont(_painter);
	return label_;
}

void Dialog::SetQueryLabel(Label* label) {
	delete label_;
	label_ = label;
}

void Dialog::UpdateQueryLabel(const wstr& text, const Color& color) {
	deb_assert(label_);
	if (label_) {
		label_->SetText(text);
		label_->SetFontColor(color);
	}
}

void Dialog::AddButton(int tag, const wstr& text, bool auto_dismiss) {
	Button* _button = new Button(BorderComponent::kZigzag, 3, Color(color_[0], color_[1], 0.2f), text);
	_button->SetText(text, color_[1]);
	_button->SetPreferredSize(57, 57);
	AddButton(tag, _button, auto_dismiss);
}

void Dialog::AddButton(int tag, Button* button, bool auto_dismiss) {
	//button->SetBaseColor(Color(0, 0, 0, 0));
	button->SetText(button->GetText(), color_[1]);
	AddChild(button);
	button_list_.push_back(button);
	SetButtonHandler(tag, button, auto_dismiss);
	UpdateLayout();
}

void Dialog::SetButtonHandler(int tag, Button* button, bool auto_dismiss) {
	button->SetTag(tag);
	if (auto_dismiss) {
		button->SetOnClick(Dialog, OnDismissClick);
	} else {
		button->SetOnClick(Dialog, OnClick);
	}
}

bool Dialog::IsAutoDismissButton(Button* button) const {
	return *button->on_click_ == uitbc::ButtonType<uitbc::Button>::Delegate((uitbc::Dialog*)this, &Dialog::OnDismissClick);
}

void Dialog::SetOffset(PixelCoord offset) {
	offset_ = offset;
	UpdateLayout();
}

void Dialog::UpdateLayout() {
	const int button_count = (int)button_list_.size();
	if (!button_count) {
		Parent::UpdateLayout();
		return;
	}

	const PixelCoord& __size = GetSize();

	if (label_) {
		PixelCoord label_size = label_->GetPreferredSize(true);
		PixelCoord coord(__size.x/2 - label_size.x/2, __size.y/3 - label_size.y);
		label_->SetPos(coord + offset_);
	}

	// Find first auto-layouted button.
	Button* _button = 0;
	for (int i = 0; i < button_count; ++i) {
		_button = button_list_[i];
		if (_button->GetTag() >= 0) {
			break;
		}
	}
	const PixelCoord button_size = _button->GetPreferredSize();
	const bool layout_x = ((__size.x - button_size.x*button_count) > (__size.y - button_size.y*button_count));
	if (layout_x) {
		const int space_per_each = button_size.x*3/2;
		const int half_gap = (space_per_each - button_size.x)/2;
		int x = __size.x/2 + half_gap - space_per_each*button_count/2;
		const int y = label_? __size.y/2 : __size.y/2-button_size.y/2;
		for (int i = 0; i < button_count; ++i) {
			Button* _btn = button_list_[i];
			if (_btn->GetTag() >= 0) {
				_btn->SetPos(PixelCoord(x, y) + offset_);
				x += space_per_each;
			}
		}
	} else {
		const int space_per_each = button_size.y*3/2;
		const int half_gap = (space_per_each - button_size.y)/2;
		const int x = __size.x/2 - button_size.x/2;
		int y = __size.y/2 + half_gap - space_per_each*button_count/2;
		y += label_? button_size.y/2 : 0;
		for (int i = 0; i < button_count; ++i) {
			Button* _btn = button_list_[i];
			if (_btn->GetTag() >= 0) {
				_btn->SetPos(PixelCoord(x, y) + offset_);
				y += space_per_each;
			}
		}
	}

	Parent::UpdateLayout();
}



void Dialog::Repaint(Painter* painter) {
	Animate();	// Slides dialog in on create and out on destroy.
	Parent::Repaint(painter);
}

void Dialog::Animate() {
	if (!IsComplete()) {
		return;
	}
	if (animation_step_) {
		const PixelCoord& parent_size = GetParent()->GetSize();
		const PixelCoord& __size = GetSize();
		PixelCoord pos = GetPos();
		pos.x += animation_step_;
		pos.y = parent_size.y/2 - __size.y/2;
		SetPos(pos);
		if (!is_closing_) {
			// Move in from right.
			animation_step_ += kAcceleration * direction_;
			const int x = parent_size.x/2-__size.x/2;
			if ((direction_ > 0 && (pos.x <= x || animation_step_ >= 0)) ||
				(direction_ < 0 && (pos.x >= x || animation_step_ <= 0))) {
				animation_step_ = 0;
				Center();
			}
		} else {
			// Move out to left.
			animation_step_ -= kAcceleration * direction_;
			if ((direction_ > 0 && (pos.x+GetSize().x < -kMargin || animation_step_ >= 0)) ||
				(direction_ < 0 && (pos.x > parent_size.x+kMargin || animation_step_ <= 0))) {
				DoClick(clicked_button_);
				((DesktopWindow*)GetParentOfType(kDesktopwindow))->PostDeleteComponent(this, 0);
			}
		}
	}

	// If animating in coinsides with dismissal.
	if (!animation_step_ && is_closing_) {
		is_closing_ = false;
		Dismiss();
	}
}

void Dialog::OnDismissClick(Button* button) {
	if (!clicked_button_) {
		if (pre_click_target_) {
			pre_click_target_(button);
		}
		clicked_button_ = button;
		Dismiss();
	}
}

void Dialog::OnClick(Button* button) {
	if (pre_click_target_) {
		pre_click_target_(button);
	}
	DoClick(button);
}

void Dialog::DoClick(Button* button) {
	Action _post_click_target = post_click_target_;	// Save in case of destruction.
	//if (button)	Always click!
	{
		target_(button);
	}
	if (_post_click_target) {
		_post_click_target(button);
	}
}



}
