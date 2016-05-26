
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/gui/uiwindow.h"
#include "../../include/gui/uicaption.h"
#include "../../include/gui/uifloatinglayout.h"
#include "../../include/gui/uidesktopwindow.h"
#include "../../../lepra/include/log.h"



namespace uitbc {



Window::Window(Layout* layout) :
	Component(new GridLayout(1, 1)),
	tl_border_(0),
	tr_border_(0),
	br_border_(0),
	bl_border_(0),
	t_border_(0),
	b_border_(0),
	l_border_(0),
	r_border_(0),
	center_component_(0),
	client_rect_(0),
	caption_(0),
	border_(true),
	border_width_(0),
	body_color_(0, 0, 0),
	border_style_(0) {
	client_rect_ = new RectComponent(layout);
	center_component_ = new RectComponent(new GridLayout(2, 1));
	center_component_->AddChild(client_rect_, 1, 0);
	//Init();
}

Window::Window(unsigned border_style,
			   int border_width,
			   const Color& color,
			   Layout* layout) :
	Component(new GridLayout(3, 3)),
	tl_border_(0),
	tr_border_(0),
	br_border_(0),
	bl_border_(0),
	t_border_(0),
	b_border_(0),
	l_border_(0),
	r_border_(0),
	center_component_(0),
	client_rect_(0),
	caption_(0),
	border_(true),
	border_width_(border_width),
	body_color_(color),
	border_style_(border_style) {
	client_rect_ = new RectComponent(color, layout);
	//client_rect_->SetPreferredSize(0, 0, true);
	center_component_ = new RectComponent(new GridLayout(2, 1));
	center_component_->AddChild(client_rect_, 1, 0);
	//InitBorder();
}

Window::Window(unsigned border_style,
			 int border_width,
			 Painter::ImageID image_id,
			 Layout* layout) :
	Component(new GridLayout(3, 3)),
	tl_border_(0),
	tr_border_(0),
	br_border_(0),
	bl_border_(0),
	t_border_(0),
	b_border_(0),
	l_border_(0),
	r_border_(0),
	center_component_(0),
	client_rect_(0),
	caption_(0),
	border_(true),
	border_width_(border_width),
	body_color_(192, 192, 192),
	border_style_(border_style) {
	client_rect_ = new RectComponent(image_id, layout);
	//client_rect_->SetPreferredSize(0, 0, true);
	center_component_ = new RectComponent(new GridLayout(2, 1));
	center_component_->AddChild(client_rect_, 1, 0);
	//InitBorder();
}

Window::Window(const Color& color,
			 Layout* layout) :
	Component(new GridLayout(1, 1)),
	tl_border_(0),
	tr_border_(0),
	br_border_(0),
	bl_border_(0),
	t_border_(0),
	b_border_(0),
	l_border_(0),
	r_border_(0),
	center_component_(0),
	client_rect_(0),
	caption_(0),
	border_(false),
	border_width_(0),
	body_color_(color),
	border_style_(0) {
	client_rect_ = new RectComponent(color, layout);
	//client_rect_->SetPreferredSize(0, 0, true);
	center_component_ = new RectComponent(new GridLayout(2, 1));
	center_component_->AddChild(client_rect_, 1, 0);
	//InitBorder();
}

Window::Window(Painter::ImageID image_id,
			 Layout* layout) :
	Component(new GridLayout(1, 1)),
	tl_border_(0),
	tr_border_(0),
	br_border_(0),
	bl_border_(0),
	t_border_(0),
	b_border_(0),
	l_border_(0),
	r_border_(0),
	center_component_(0),
	client_rect_(0),
	caption_(0),
	border_(false),
	border_width_(0),
	body_color_(192, 192, 192),
	border_style_(0) {
	client_rect_ = new RectComponent(image_id, layout);
	//client_rect_->SetPreferredSize(0, 0, true);
	center_component_ = new RectComponent(new GridLayout(2, 1));
	center_component_->AddChild(client_rect_, 1, 0);
	Parent::AddChild(center_component_);
	//InitBorder();
}

Window::~Window() {
	delete tr_border_;
	delete br_border_;
	delete bl_border_;
	delete b_border_;
	delete r_border_;
	delete tl_border_;
	delete t_border_;
	delete l_border_;
	delete client_rect_;

	Parent::RemoveChild(center_component_, 0);
	delete center_component_;
}

void Window::Init() {
	Parent::AddChild(center_component_, 0, 0);

	SetColor(body_color_);
	SetNeedsRepaint(true);
}

void Window::InitBorder() {
	BorderComponent::BorderShadeFunc func = BorderComponent::kZigzag;
	if (Check(border_style_, kBorderLinearshading) == true) {
		func = BorderComponent::kLinear;
	}

	if (Check(border_style_, kBorderHalf) == false) {
		tr_border_ = new BorderComponent(BorderComponent::kToprightCorner,    body_color_, func);
		br_border_ = new BorderComponent(BorderComponent::kBottomrightCorner, body_color_, func);
		bl_border_ = new BorderComponent(BorderComponent::kBottomleftCorner,  body_color_, func);
		b_border_  = new BorderComponent(BorderComponent::kBottomBorder,      body_color_, func);
		r_border_  = new BorderComponent(BorderComponent::kRightBorder,       body_color_, func);
	}
	tl_border_ = new BorderComponent(BorderComponent::kTopleftCorner,     body_color_, func);
	t_border_  = new BorderComponent(BorderComponent::kTopBorder,         body_color_, func);
	l_border_  = new BorderComponent(BorderComponent::kLeftBorder,        body_color_, func);

	if (Check(border_style_, kBorderSunken) == true) {
		if (Check(border_style_, kBorderHalf) == false) {
			tr_border_->SetSunken(true);
			br_border_->SetSunken(true);
			bl_border_->SetSunken(true);
			b_border_->SetSunken(true);
			r_border_->SetSunken(true);
		}
		tl_border_->SetSunken(true);
		t_border_->SetSunken(true);
		l_border_->SetSunken(true);
	}

	if (Check(border_style_, kBorderHalf) == false) {
		tr_border_->SetMinSize(border_width_, border_width_);
		br_border_->SetMinSize(border_width_, border_width_);
		bl_border_->SetMinSize(border_width_, border_width_);
		b_border_->SetMinSize(0, border_width_);
		r_border_->SetMinSize(border_width_, 0);
	}
	tl_border_->SetMinSize(border_width_, border_width_);
	t_border_->SetMinSize(0, border_width_);
	l_border_->SetMinSize(border_width_, 0);

	if (Check(border_style_, kBorderHalf) == false) {
		tr_border_->SetPreferredSize(border_width_, border_width_);
		br_border_->SetPreferredSize(border_width_, border_width_);
		bl_border_->SetPreferredSize(border_width_, border_width_);
		b_border_->SetPreferredSize(0, border_width_);
		r_border_->SetPreferredSize(border_width_, 0);
	}
	tl_border_->SetPreferredSize(border_width_, border_width_);
	t_border_->SetPreferredSize(0, border_width_);
	l_border_->SetPreferredSize(border_width_, 0);

	if (Check(border_style_, kBorderResizable) == true) {
		if (Check(border_style_, kBorderHalf) == false) {
			tr_border_->SetResizable(true);
			br_border_->SetResizable(true);
			bl_border_->SetResizable(true);
			b_border_->SetResizable(true);
			r_border_->SetResizable(true);
		}
		tl_border_->SetResizable(true);
		t_border_->SetResizable(true);
		l_border_->SetResizable(true);
	}

//	center_component_->SetPreferredSize(0, 0);
	if (Check(border_style_, kBorderHalf) == false) {
		Parent::AddChild(tr_border_, 0, 2);
		Parent::AddChild(br_border_, 2, 2);
		Parent::AddChild(bl_border_, 2, 0);
		Parent::AddChild(b_border_, 2, 1);
		Parent::AddChild(r_border_, 1, 2);
	}

	Parent::AddChild(tl_border_, 0, 0);
	Parent::AddChild(t_border_, 0, 1);
	Parent::AddChild(l_border_, 1, 0);

	SetNeedsRepaint(true);
}

void Window::SetBorder(unsigned border_style, int width) {
	if (border_ == false || (border_style == border_style_ && width == border_width_)) {
		return;
	}

	border_style_ = border_style;

	BorderComponent::BorderShadeFunc func = BorderComponent::kZigzag;
	if (Check(border_style_, kBorderLinearshading) == true) {
		func = BorderComponent::kLinear;
	}

	border_width_ = width;

	if (tl_border_) {
		tl_border_->SetPreferredSize(width, width);
		tr_border_->SetPreferredSize(width, width);
		br_border_->SetPreferredSize(width, width);
		bl_border_->SetPreferredSize(width, width);
		t_border_->SetPreferredSize(0, width);
		b_border_->SetPreferredSize(0, width);
		l_border_->SetPreferredSize(width, 0);
		r_border_->SetPreferredSize(width, 0);

		tl_border_->Set(body_color_, func);
		tr_border_->Set(body_color_, func);
		br_border_->Set(body_color_, func);
		bl_border_->Set(body_color_, func);
		t_border_->Set(body_color_, func);
		b_border_->Set(body_color_, func);
		l_border_->Set(body_color_, func);
		r_border_->Set(body_color_, func);

		if (Check(border_style_, kBorderSunken) == true) {
			tl_border_->SetSunken(true);
			tr_border_->SetSunken(true);
			br_border_->SetSunken(true);
			bl_border_->SetSunken(true);
			t_border_->SetSunken(true);
			b_border_->SetSunken(true);
			l_border_->SetSunken(true);
			r_border_->SetSunken(true);
		}
	}

	SetNeedsRepaint(true);
}

void Window::SetBorder(unsigned border_style,
		       Painter::ImageID top_left_id,
		       Painter::ImageID top_right_id,
		       Painter::ImageID bottom_left_id,
		       Painter::ImageID bottom_right_id,
		       Painter::ImageID top_id,
		       Painter::ImageID bottom_id,
		       Painter::ImageID left_id,
		       Painter::ImageID right_id) {
	if (border_ == false) {
		return;
	}

	border_style_ = border_style;

	GUIImageManager* i_man = GetImageManager();

	tl_border_->SetPreferredSize(i_man->GetImageSize(top_left_id));
	tr_border_->SetPreferredSize(i_man->GetImageSize(top_right_id));
	bl_border_->SetPreferredSize(i_man->GetImageSize(bottom_left_id));
	br_border_->SetPreferredSize(i_man->GetImageSize(bottom_right_id));
	t_border_->SetPreferredSize(0, i_man->GetImageSize(top_id).y);
	b_border_->SetPreferredSize(0, i_man->GetImageSize(bottom_id).y);
	l_border_->SetPreferredSize(i_man->GetImageSize(left_id).x, 0);
	r_border_->SetPreferredSize(i_man->GetImageSize(right_id).x, 0);

	tl_border_->Set(top_left_id);
	tr_border_->Set(top_right_id);
	br_border_->Set(bottom_right_id);
	bl_border_->Set(bottom_left_id);
	t_border_->Set(top_id);
	b_border_->Set(bottom_id);
	l_border_->Set(left_id);
	r_border_->Set(right_id);
}

bool Window::IsOver(int screen_x, int screen_y) {
	return GetScreenRect().IsInside(screen_x, screen_y);
	/*if (GetScreenRect().IsInside(screen_x, screen_y) == true) {
		Layout* _layout = GetLayout();
		if (_layout != 0) {
			Component* _child = _layout->GetFirst();
			while (_child != 0) {
				if (_child->GetScreenRect().IsInside(screen_x, screen_y) == true) {
					return _child->IsOver(screen_x, screen_y);
				}
				_child = _layout->GetNext();
			}
		}
	}
	return false;*/
}

bool Window::OnMouseMove(int mouse_x, int mouse_y, int delta_x, int delta_y) {
	Parent::OnMouseMove(mouse_x, mouse_y, delta_x, delta_y);
	return IsOver(mouse_x, mouse_y);
}

void Window::SetCaption(Caption* caption) {
	if (caption_ != 0) {
		center_component_->RemoveChild(caption_, 0);
	}

	caption_ = caption;

	if (caption_ != 0) {
		center_component_->AddChild(caption_, 0, 0);
	}
}

PixelRect Window::GetClientRect() const {
	return center_component_->GetScreenRect();
}

RectComponent* Window::GetClientRectComponent() const {
	return client_rect_;
}

void Window::AddChild(Component* child, int param1, int param2, int layer) {
	client_rect_->AddChild(child, param1, param2, layer);
}

void Window::RemoveChild(Component* child, int layer) {
	client_rect_->RemoveChild(child, layer);
}

int Window::GetNumChildren() const {
	return client_rect_->GetNumChildren();
}

void Window::SetActive(bool active) {
	if (IsVisible()) {
		if (active) {
			if (caption_ != 0) {
				caption_->SetActive(true);
			}

			Component* parent = Parent::GetParent();

			if (parent != 0) {
				Layout* parent_layout = parent->GetLayout();

				if (parent_layout->GetType() == Layout::kFloatinglayout) {
					((FloatingLayout*)parent_layout)->MoveToTop(this);
				}
			}
		} else {
			if (caption_ != 0) {
				if (HasKeyboardFocus() ==  true) {
					GetTopParent()->ReleaseKeyboardFocus(kRecurseDown);
				}

				caption_->SetActive(false);
			}
		}
	}
}

bool Window::OnChar(wchar_t c) {
	bool ok;
	if (c == '\t') {
		// Focus next component. May wrap.
		const DesktopWindow* desktop_window = (DesktopWindow*)GetParentOfType(Component::kDesktopwindow);
		uilepra::InputManager* input_manager = desktop_window->GetInputManager();
		bool focus_previous = false;
		bool focus_next = false;
		StateComponentList component_list = GetStateList(kStateFocusable);
		StateComponentList::iterator x = component_list.begin();
		for (; x != component_list.end(); ++x) {
			if (focus_next) {
				break;
			}
			if (x->first == 2) {
				return (Parent::OnChar(c));	// RAII simplifies.
			} else if (x->first) {
				if (input_manager->ReadKey(uilepra::InputManager::kInKbdLShift) ||
					input_manager->ReadKey(uilepra::InputManager::kInKbdRShift)) {
					focus_previous = true;
					break;
				}
				focus_next = true;
			}
		}
		if (focus_previous) {
			if (x == component_list.begin()) {
				x = component_list.end();
			}
			--x;
		} else if (!focus_next || x == component_list.end()) {
			x = component_list.begin();
		}
		if (x != component_list.end()) {	// Any available focusable children?
			x->second->SetKeyboardFocus();
		}
		ok = true;
	} else if (c == '\r') {
		StateComponentList component_list = client_rect_->GetStateList(kStateClickable);
		StateComponentList::iterator x = component_list.begin();
		if (x != component_list.end()) {
			((Button*)x->second)->Click(true);
		} else {
			return (Parent::OnChar(c));	// RAII simplifies.
		}
		ok = true;
	} else {
		ok = Parent::OnChar(c);
	}
	return (ok);
}

bool Window::OnLButtonDown(int mouse_x, int mouse_y) {
	bool __return = Parent::OnLButtonDown(mouse_x, mouse_y);
	SetActive(true);
	return __return;
}

bool Window::OnRButtonDown(int mouse_x, int mouse_y) {
	bool __return = Parent::OnRButtonDown(mouse_x, mouse_y);
	SetActive(true);
	return __return;
}

bool Window::OnMButtonDown(int mouse_x, int mouse_y) {
	bool __return = Parent::OnMButtonDown(mouse_x, mouse_y);
	SetActive(true);
	return __return;
}



void Window::DoSetSize(int width, int height) {
	Parent::DoSetSize(width, height);
	center_component_->SetSize(width, height);
	center_component_->UpdateLayout();
}



bool Window::Check(unsigned flags, unsigned pFlag) {
	return ((flags & pFlag) != 0);
}

Caption* Window::GetCaption() {
	return caption_;
}

void Window::SetBackgroundImage(Painter::ImageID image_id) {
	SetNeedsRepaint(image_id != client_rect_->GetImage());
	client_rect_->SetImage(image_id);
}

void Window::SetBaseColor(const Color& color) {
	body_color_ = color;
	SetColor(color);
}

void Window::SetColor(const Color& color) {
	SetNeedsRepaint(color != client_rect_->GetColor());
	client_rect_->SetColor(color);

	if (tl_border_) {
		BorderComponent::BorderShadeFunc func =
			Check(border_style_, kBorderLinearshading)? BorderComponent::kLinear : BorderComponent::kZigzag;
		tl_border_->Set(color, func);
		tr_border_->Set(color, func);
		br_border_->Set(color, func);
		bl_border_->Set(color, func);
		t_border_->Set(color, func);
		b_border_->Set(color, func);
		l_border_->Set(color, func);
		r_border_->Set(color, func);
	}
}

Painter::ImageID Window::GetBackgroundImage() {
	return client_rect_->GetImage();
}

const Color& Window::GetColor() {
	return body_color_;
}

unsigned Window::GetBorderStyle() {
	return border_style_;
}

int Window::GetBorderWidth() {
	return border_width_;
}

int Window::GetTotalBorderWidth() {
	if (Check(border_style_, kBorderHalf) == true)
		return border_width_;
	else
		return border_width_ * 2;
}

int Window::GetRoundedRadius() const {
	return GetClientRectComponent()->GetCornerRadius();
}

void Window::SetRoundedRadius(int radius) {
	GetClientRectComponent()->SetCornerRadius(radius);
	GetClientRectComponent()->SetIsHollow(false);
}

void Window::SetRoundedRadiusMask(int mask) {
	GetClientRectComponent()->SetCornerRadiusMask(mask);
}

Component::Type Window::GetType() const {
	return Component::kWindow;
}



}
