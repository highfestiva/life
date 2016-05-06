
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/gui/uibutton.h"
#include "../../include/gui/uidesktopwindow.h"



namespace uitbc {



Button::Button(const wstr& text):
	Parent(),
	on_press_(0),
	on_release_(0),
	on_click_(0),
	on_drag_(0),
	icon_id_(Painter::kInvalidImageid),
	highlighted_icon_id_(Painter::kInvalidImageid),
	disabled_icon_id_(Painter::kInvalidImageid),
	icon_alignment_(kIconCenter),
	text_(),
	text_backg_color_(255, 255, 255),
	pressed_(false),
	image_button_(false),
	state_(kPressed),
	extra_data_(0) {
	SetBaseColor(GetColor());
	Init();
	SetText(text);
	SetName(strutil::Encode(text));
}

Button::Button(const Color& color, const wstr& text):
	Parent(color),
	on_press_(0),
	on_release_(0),
	on_click_(0),
	on_drag_(0),
	icon_id_(Painter::kInvalidImageid),
	highlighted_icon_id_(Painter::kInvalidImageid),
	disabled_icon_id_(Painter::kInvalidImageid),
	icon_alignment_(kIconCenter),
	text_(text),
	text_backg_color_(255, 255, 255),
	pressed_(false),
	image_button_(false),
	state_(kPressed),
	extra_data_(0) {
	SetBaseColor(GetColor());
	Init();
	SetText(text);
	SetName(strutil::Encode(text));
}

Button::Button(BorderComponent::BorderShadeFunc shade_func, int border_width, const Color& color, const wstr& text):
	Parent((shade_func == BorderComponent::kLinear ? Parent::kBorderLinearshading : 0), border_width, color),
	on_press_(0),
	on_release_(0),
	on_click_(0),
	on_drag_(0),
	icon_id_(Painter::kInvalidImageid),
	highlighted_icon_id_(Painter::kInvalidImageid),
	disabled_icon_id_(Painter::kInvalidImageid),
	icon_alignment_(kIconCenter),
	text_(text),
	text_backg_color_(255, 255, 255),
	pressed_(false),
	image_button_(false),
	state_(kPressed),
	extra_data_(0) {
	SetBaseColor(GetColor());
	Init();
	SetText(text);
	SetName(strutil::Encode(text));
}

Button::Button(Painter::ImageID released_image_id, Painter::ImageID pressed_image_id,
	Painter::ImageID released_active_image_id,	// Mouse over.
	Painter::ImageID pressed_active_image_id, Painter::ImageID releasing_image_id,
	Painter::ImageID pressing_image_id, const wstr& text):
	Parent(released_image_id),
	on_press_(0),
	on_release_(0),
	on_click_(0),
	on_drag_(0),
	released_image_id_(released_image_id),
	pressed_image_id_(pressed_image_id),
	released_active_image_id_(released_active_image_id),
	pressed_active_image_id_(pressed_active_image_id),
	releasing_image_id_(releasing_image_id),
	pressing_image_id_(pressing_image_id),
	icon_id_(Painter::kInvalidImageid),
	highlighted_icon_id_(Painter::kInvalidImageid),
	disabled_icon_id_(Painter::kInvalidImageid),
	icon_alignment_(kIconCenter),
	text_(text),
	text_backg_color_(255, 255, 255),
	pressed_(false),
	image_button_(true),
	state_(kPressed),
	extra_data_(0) {
	SetBaseColor(GetColor());
	Init();
	SetText(text);
	SetName(strutil::Encode(text));
}

Button::~Button() {
	delete on_press_;
	delete on_release_;
	delete on_click_;
	delete on_drag_;
}

void Button::SetBaseColor(const Color& color) {
	Parent::SetBaseColor(color);
	hoover_color_ = GetColor() * 1.2f;
	press_color_ = GetColor() * 0.95f;
	GetClientRectComponent()->SetIsHollow(false);
	SetPressed(false);
}

void Button::SetPressColor(const Color& color) {
	press_color_ = color;
}

void Button::SetPressed(bool pressed) {
	pressed_ = pressed;
	if (pressed_) {
		SetState(kPressed);
	} else {
		if (HasMouseFocus()) {
			SetState(kReleasedHoover);
		} else {
			SetState(kReleased);
		}
	}
}

void Button::SetState(State state) {
	if (state_ == state || !enabled_) {
		return;
	}

	SetNeedsRepaint(true);

	log_volatile(log_.Debugf("Button changing state from %i->%i.", state_, state));
	state_ = state;

	if (image_button_ == true) {
		switch(state_) {
		case kReleased:
			Parent::SetBackgroundImage(released_image_id_);
			break;
		case kReleasedHoover:
			Parent::SetBackgroundImage(released_active_image_id_);
			break;
		case kReleasing:
			Parent::SetBackgroundImage(releasing_image_id_);
			break;
		case kPressed:
			Parent::SetBackgroundImage(pressed_image_id_);
			break;
		case kPressedHoover:
			Parent::SetBackgroundImage(pressed_active_image_id_);
			break;
		case kPressing:
			Parent::SetBackgroundImage(pressing_image_id_);
			break;
		}
	} else {
		if (state_ == kPressed || state_ == kPressing || state_ == kPressedHoover) {
			Parent::SetColor(press_color_);
			unsigned style = Parent::GetBorderStyle() | Parent::kBorderSunken;
			Parent::SetBorder(style, Parent::GetBorderWidth());
		} else if (state_ == kReleasedHoover) {
			Parent::SetColor(hoover_color_);
			unsigned style = Parent::GetBorderStyle() | Parent::kBorderSunken;
			Parent::SetBorder(style, Parent::GetBorderWidth());
		} else {
			Parent::SetColor(GetColor());
			unsigned style = (Parent::GetBorderStyle() & (~Parent::kBorderSunken));
			Parent::SetBorder(style, Parent::GetBorderWidth());
		}
	}

	switch(state_) {
	case kReleased:
	case kReleasedHoover:
		if (on_release_ != 0) {
			(*on_release_)(this);
		} break;
	case kReleasing:
		break;
	case kPressed:
	case kPressedHoover:
		break;
	case kPressing:
		if (on_press_ != 0) {
			(*on_press_)(this);
		} break;
	}
}

Button::StateComponentList Button::GetStateList(ComponentState state) {
	StateComponentList list;
	if (state == kStateClickable) {
		list.push_back(StateComponent(0, this));
	}
	return (list);
}

void Button::Repaint(Painter* painter) {
	RepaintBackground(painter);

	ActivateFont(painter);
	GUIImageManager* i_man = GetImageManager();

	painter->PushAttrib(Painter::kAttrAll);
	PixelRect rect(GetClientRect());
	//painter->ReduceClippingRect(rect);

	int offset = GetPressed() ? 1 : 0;
	int text_x = rect.left_ + horizontal_margin_;
	switch (icon_alignment_) {
		case kIconCenter:
			text_x = rect.GetCenterX() - painter->GetStringWidth(text_)/2;
		break;
	}

	Painter::ImageID _icon_id = GetCurrentIcon();
	if (_icon_id != Painter::kInvalidImageid) {
		PixelCoord image_size(i_man->GetImageSize(_icon_id));

		int x = 0;
		int y = 0;
		switch (icon_alignment_) {
			case kIconLeft:
				x = rect.left_ + offset;
				y = rect.top_ + (rect.GetHeight() - image_size.y) / 2 + offset;
				text_x = rect.left_ + image_size.x + horizontal_margin_;
			break;
			case kIconCenter:
				x = rect.left_ + (rect.GetWidth()  - image_size.x) / 2 + offset;
				if (!text_.empty()) {
					y = rect.top_;
				} else {
					y = rect.GetCenterY() - image_size.y/2;
				} break;
			case kIconRight:
				x = rect.right_ - image_size.x + offset - horizontal_margin_;
				y = rect.top_ + (rect.GetHeight() - image_size.y) / 2 + offset;
			break;
		}
		i_man->DrawImage(_icon_id, x, y);
	}

	RepaintComponents(painter);

	if (icon_id_ != Painter::kInvalidImageid) {
		int y;
		if (icon_alignment_ != kIconCenter) {
			y = rect.top_ + (rect.GetHeight() - painter->GetFontHeight()) / 2;
		} else {
			y = rect.bottom_ - painter->GetFontHeight();
		}
		PrintText(painter, text_x + offset, y);
	} else {
		PrintText(painter,
			  text_x + offset,
			  rect.top_ + (rect.GetHeight() - painter->GetFontHeight()) / 2 + offset);
	}

	painter->PopAttrib();
}

void Button::RepaintBackground(Painter* painter) {
	GetClientRectComponent()->RepaintBackground(painter);
}

void Button::RepaintComponents(Painter* painter) {
	if (NeedsRepaint() || GetBorderWidth() > 0) {
		Parent::RepaintComponents(painter);
	} else {
		GetClientRectComponent()->RepaintComponents(painter);
	}
}

void Button::PrintText(Painter* painter, int x, int y) {
	painter->SetColor(text_backg_color_, 1);
	PrintTextDeactivate(painter, text_, x, y);
}

void Button::SetExtraData(void* data) {
	extra_data_ = data;
}

void* Button::GetExtraData() const {
	return (extra_data_);
}

void Button::SetTag(int tag) {
	SetExtraData((void*)tag);
}

int Button::GetTag() const {
	return (int)(intptr_t)GetExtraData();
}

bool Button::OnLButtonDown(int mouse_x, int mouse_y) {
	if (IsOver(mouse_x, mouse_y) == true) {
		switch (state_) {
			case kReleased:
			case kReleasedHoover:	SetState(kPressing);	break;
			case kPressed:
			case kPressedHoover:	SetState(kReleasing);	break;
		}

		SetMouseFocus();

		Click(false);
	}

	return true;
}

bool Button::OnLButtonUp(int mouse_x, int mouse_y) {
	bool call_functor = HasMouseFocus();

	if (IsOver(mouse_x, mouse_y) == true) {
		SetPressed(false);
		if (call_functor) {
			Click(true);
		}
		ReleaseMouseFocus();
	} else {
		ReleaseMouseFocus();
		SetPressed(false);
	}

	return true;
}

bool Button::OnMouseMove(int mouse_x, int mouse_y, int delta_x, int delta_y) {
	if (HasMouseFocus() == true) {
		switch(state_) {
		case kReleased:
		case kReleasedHoover:
			if (IsOver(mouse_x, mouse_y) == true)
				SetState(kPressing);
			else
				SetState(kReleased);
			break;
		case kReleasing:
			if (IsOver(mouse_x, mouse_y) == false)
				SetState(kPressed);
			break;
		case kPressed:
		case kPressedHoover:
			if (IsOver(mouse_x, mouse_y) == true)
				SetState(kReleasing);
			else
				SetState(kPressed);
			break;
		case kPressing:
			if (IsOver(mouse_x, mouse_y) == false)
				SetState(kReleased);
			break;
		}

		if (on_drag_ != 0) {
			(*on_drag_)(this, mouse_x, mouse_x, delta_x, delta_y);
		}
	} else {
		switch(state_) {
		case kReleased:
		case kReleasedHoover:
		case kReleasing:
			if (IsOver(mouse_x, mouse_y) == true)
				SetState(kReleasedHoover);
			else
				SetState(kReleased);
			break;
		case kPressed:
		case kPressedHoover:
		case kPressing:
			if (IsOver(mouse_x, mouse_y) == true)
				SetState(kPressedHoover);
			else
				SetState(kPressed);
			break;
		}
	}
	return Parent::OnMouseMove(mouse_x, mouse_y, delta_x, delta_y);
}

bool Button::Click(bool depress) {
	bool clicked = false;
	if (depress && on_click_) {
		(*on_click_)(this);
		clicked = true;
	}
	return (clicked);
}

bool Button::GetPressed() {
	return pressed_;
}

Button::State Button::GetState() const {
	return state_;
}

void Button::SetIcon(Painter::ImageID icon_id, IconAlignment alignment) {
	icon_id_ = icon_id;
	icon_alignment_ = alignment;

	if (icon_id_ != Painter::kInvalidImageid && icon_alignment_ == kIconCenter) {
		PixelCoord size = GetImageManager()->GetImageSize(icon_id_);
		DesktopWindow* topmost = (DesktopWindow*)GetParentOfType(kDesktopwindow);
		const int font_height = topmost? topmost->GetPainter()->GetFontHeight()*3/2 : 21;	// Just try anything if not into system yet.
		const int dh = GetText().empty()? 0 : font_height;
		SetPreferredSize(size.x, size.y + dh);
	}

	SetNeedsRepaint(true);
}

Painter::ImageID Button::GetIconCanvas() const {
	return icon_id_;
}

void Button::SetHighlightedIcon(Painter::ImageID icon_id) {
	highlighted_icon_id_ = icon_id;
	SetNeedsRepaint(true);
}

void Button::SetDisabledIcon(Painter::ImageID icon_id) {
	disabled_icon_id_ = icon_id;
	SetNeedsRepaint(true);
}

Painter::ImageID Button::GetCurrentIcon() const {
	if (disabled_icon_id_ != Painter::kInvalidImageid && !enabled_) {
		return disabled_icon_id_;
	} else if (highlighted_icon_id_ != Painter::kInvalidImageid &&
		(state_ == kPressed || state_ == kPressing
#ifndef LEPRA_TOUCH	// Hoover states only exist and work on non-touch devices (i.e. computers w/ mouse/trackball).
		|| state_ == kPressedHoover || state_ == kReleasedHoover
#endif // !touch
		)) {
		return highlighted_icon_id_;
	}
	return icon_id_;
}

void Button::SetText(const wstr& text,
		     const Color& text_color,
		     const Color& backg_color) {
	deb_assert(text_color != BLACK);
	text_           = text;
	SetFontColor(text_color);
	text_backg_color_ = backg_color;

	OnTextChanged();
}

const wstr& Button::GetText() {
	return text_;
}

Button::Type Button::GetType() const {
	return kButton;
}

void Button::ForceRepaint() {
	SetNeedsRepaint(true);
}

void Button::OnTextChanged() {
}

void Button::SetOnPressDelegate(const Delegate& on_press) {
	delete on_press_;
	on_press_ = new Delegate(on_press);
}

void Button::SetOnReleaseDelegate(const Delegate& on_release) {
	delete on_release_;
	on_release_ = new Delegate(on_release);
}

void Button::SetOnClickDelegate(const Delegate& on_click) {
	delete on_click_;
	on_click_ = new Delegate(on_click);
}

void Button::SetOnDragDelegate(const DelegateXYXY& on_drag) {
	delete on_drag_;
	on_drag_ = new DelegateXYXY(on_drag);
}



loginstance(kUiGfx2D, Button);



}
