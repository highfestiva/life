
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../../uilepra/include/uiinput.h"
#include "../../../lepra/include/log.h"
#include "../../../lepra/include/listutil.h"
#include "../../include/gui/uidesktopwindow.h"



namespace uitbc {



DesktopWindow::DesktopWindow(uilepra::InputManager* input_manager, Painter* painter,
	Layout* layout, const char* image_definition_file,
	const char* archive):
	RectComponent(layout),
	input_manager_(input_manager),
	mouse_enabled_(true),
	keyboard_enabled_(true),
	mouse_area_(0, 0, 0, 0),
	mouse_x_(0),
	mouse_y_(0),
	mouse_prev_x_(0),
	mouse_prev_y_(0),
	mouse_button_flags_(0),
	painter_(painter) {
	Init(image_definition_file, archive);
}

DesktopWindow::DesktopWindow(uilepra::InputManager* input_manager, Painter* painter, const Color& color,
	Layout* layout, const char* image_definition_file, const char* archive):
	RectComponent(color, layout),
	input_manager_(input_manager),
	mouse_enabled_(true),
	keyboard_enabled_(true),
	mouse_area_(0, 0, 0, 0),
	mouse_x_(0),
	mouse_y_(0),
	mouse_prev_x_(0),
	mouse_prev_y_(0),
	painter_(painter) {
	Init(image_definition_file, archive);
}

DesktopWindow::DesktopWindow(uilepra::InputManager* input_manager, Painter* painter,
	const Color& top_left_color, const Color& top_right_color,
	const Color& bottom_right_color, const Color& bottom_left_color,
	Layout* layout, const char* image_definition_file,
	const char* archive):
	RectComponent(top_left_color, top_right_color, bottom_right_color, bottom_left_color, layout),
	input_manager_(input_manager),
	mouse_enabled_(true),
	keyboard_enabled_(true),
	mouse_area_(0, 0, 0, 0),
	mouse_x_(0),
	mouse_y_(0),
	mouse_prev_x_(0),
	mouse_prev_y_(0),
	painter_(painter) {
	Init(image_definition_file, archive);
}

DesktopWindow::DesktopWindow(uilepra::InputManager* input_manager, Painter* painter, Painter::ImageID image_id,
	Layout* layout, const char* image_definition_file, const char* archive):
	RectComponent(image_id, layout),
	input_manager_(input_manager),
	mouse_enabled_(true),
	keyboard_enabled_(true),
	mouse_area_(0, 0, 0, 0),
	mouse_x_(0),
	mouse_y_(0),
	mouse_prev_x_(0),
	mouse_prev_y_(0),
	painter_(painter) {
	Init(image_definition_file, archive);
}

DesktopWindow::~DesktopWindow() {
	PurgeDeleted();

	// TRICKY: this has gotta be done here, too. Component parent
	// does this, but only after the image manager is deleted, which
	// causes the component still holding images to blow up when they
	// get killed.
	DeleteAllLayers();

	ListUtil::DeleteAll(cleaner_list_);

	input_manager_->RemoveTextInputObserver(this);
	input_manager_->RemoveKeyCodeInputObserver(this);
	input_manager_->RemoveMouseInputObserver(this);
	input_manager_ = 0;
}

uilepra::InputManager* DesktopWindow::GetInputManager() const {
	return (input_manager_);
}

void DesktopWindow::Init(const char* /*image_definition_file*/, const char* /*archive*/) {
	// Let all components access the image manager.
	SetImageManager(&image_manager_);

	image_manager_.SetPainter(painter_);

	uilepra::InputDevice* mouse = 0;
	if (input_manager_) {
		input_manager_->AddTextInputObserver(this);
		input_manager_->AddKeyCodeInputObserver(this);
		input_manager_->AddMouseInputObserver(this);
		mouse = input_manager_->GetMouse();
	}

	if (mouse != 0) {
		uilepra::InputElement* button1 = mouse->GetButton(0);
		uilepra::InputElement* button2 = mouse->GetButton(1);
		uilepra::InputElement* button3 = mouse->GetButton(2);

		if (button1 != 0) {
			ADD_INPUT_CALLBACK(button1, OnButton1, DesktopWindow);
		}
		if (button2 != 0) {
			ADD_INPUT_CALLBACK(button2, OnButton2, DesktopWindow);
		}
		if (button3 != 0) {
			ADD_INPUT_CALLBACK(button3, OnButton3, DesktopWindow);
		}
	}

//	This code is left as a comment to remind you that reading the keyboard
//	shouldn't be done the same way as the mouse. It is possible of course,
//	but it would require huge amounts of code and isn't practial.
//
//	uilepra::InputDevice* lKeyboard = input_manager_->GetKeyboard();
//
//	if (lKeyboard != 0)
//	{
//
//	}

}

void DesktopWindow::PurgeDeleted() {
	// Delete all prerendered images from the painter.
	ComponentList::iterator iter;
	for (iter = delete_queue_.begin(); iter != delete_queue_.end(); ++iter) {
		Component* _child = *iter;
		if (_child->image_id_ != Painter::kInvalidImageid) {
			painter_->RemoveImage(_child->image_id_);
			_child->ReleaseKeyboardFocus();
		}
		RemoveChild(_child, 0);
	}
	// Delete all queued components.
	ListUtil::DeleteAll(delete_queue_);
}

void DesktopWindow::DoSetSize(int width, int height) {
	Parent::DoSetSize(width, height);
	PixelCoord pos(GetScreenPos());
	mouse_area_.Set(pos.x, pos.y, pos.x + width, pos.y + height);
}

void DesktopWindow::SetMouseEnabled(bool enabled) {
	mouse_enabled_ = enabled;
}

void DesktopWindow::ClampMouse(int& x, int& y) const {
	bool update_mouse = false;

	if (x < mouse_area_.left_) {
		x = mouse_area_.left_;
		update_mouse = true;
	}

	if (x >= mouse_area_.right_) {
		x = mouse_area_.right_ - 1;
		update_mouse = true;
	}

	if (y < mouse_area_.top_) {
		y = mouse_area_.top_;
		update_mouse = true;
	}

	if (y >= mouse_area_.bottom_) {
		y = mouse_area_.bottom_ - 1;
		update_mouse = true;
	}
}

void DesktopWindow::AddChild(Component* child, int param1, int param2, int layer) {
	Parent::AddChild(child, param1, param2, layer);
	child->OnConnectedToDesktopWindow();
}

void DesktopWindow::Repaint(Painter* /*painter*/) {
	PurgeDeleted();

	// Call OnIdle() on all subscribers.
	ComponentList::iterator iter;
	for (iter = idle_subscribers_.begin();
		iter != idle_subscribers_.end(); ++iter) {
		(*iter)->OnIdle();
	}

	// Handle mouse...
	GetCursorPosition(mouse_x_, mouse_y_);
	DispatchMouseMove(mouse_x_, mouse_y_);

	if (NeedsRepaint() == true || update_layout_) {
		UpdateLayout();
		update_layout_ = false;
	}
	Parent::Repaint(painter_);
}

void DesktopWindow::RepaintChild(Component* child, Painter* painter) {
	child->Repaint(painter);
}

void DesktopWindow::OnButton1(uilepra::InputElement* element) {
	if (mouse_enabled_ == true) {
		int _mouse_x;
		int _mouse_y;
		GetCursorPosition(_mouse_x, _mouse_y);
		if (element->GetBooleanValue() == true) {
			if (OnLButtonDown(_mouse_x, _mouse_y)) {
				mouse_button_flags_ |= kConsumedMouseButton1;
			}
		} else {
			mouse_button_flags_ &= ~kConsumedMouseButton1;
			DispatchMouseMove(_mouse_x, _mouse_y);
			OnLButtonUp(_mouse_x, _mouse_y);
		}
	}
}

void DesktopWindow::OnButton2(uilepra::InputElement* element) {
	if (mouse_enabled_ == true) {
		int _mouse_x;
		int _mouse_y;
		GetCursorPosition(_mouse_x, _mouse_y);
		if (element->GetBooleanValue() == true) {
			if (OnRButtonDown(_mouse_x, _mouse_y)) {
				mouse_button_flags_ |= kConsumedMouseButton2;
			}
		} else {
			mouse_button_flags_ &= ~kConsumedMouseButton2;
			DispatchMouseMove(_mouse_x, _mouse_y);
			OnRButtonUp(_mouse_x, _mouse_y);
		}
	}
}

void DesktopWindow::OnButton3(uilepra::InputElement* element) {
	if (mouse_enabled_ == true) {
		int _mouse_x;
		int _mouse_y;
		GetCursorPosition(_mouse_x, _mouse_y);
		if (element->GetBooleanValue() == true) {
			if (OnMButtonDown(_mouse_x, _mouse_y)) {
				mouse_button_flags_ |= kConsumedMouseButton3;
			}
		} else {
			mouse_button_flags_ &= ~kConsumedMouseButton3;
			DispatchMouseMove(_mouse_x, _mouse_y);
			OnMButtonUp(_mouse_x, _mouse_y);
		}
	}
}

void DesktopWindow::GetCursorPosition(int& mouse_x, int& mouse_y) const {
	if (!input_manager_) {
		return;
	}

	float64 _width  = (float64)mouse_area_.GetWidth();
	float64 _height = (float64)mouse_area_.GetHeight();

	mouse_x = (int)((input_manager_->GetCursorX() + 1.0) * _width / 2.0);
	mouse_y = (int)((input_manager_->GetCursorY() + 1.0) * _height / 2.0);

	ClampMouse(mouse_x, mouse_y);
}

void DesktopWindow::DispatchMouseMove(int mouse_x, int mouse_y) {
	if (!mouse_enabled_) {
		return;
	}

	if (mouse_prev_x_ != mouse_x || mouse_prev_y_ != mouse_y) {
		OnMouseMove(mouse_x, mouse_y, mouse_x - mouse_prev_x_, mouse_y - mouse_prev_y_);
		mouse_prev_x_ = mouse_x;
		mouse_prev_y_ = mouse_y;
	}
}

bool DesktopWindow::OnChar(wchar_t c) {
	return (Parent::OnChar(c));
}

bool DesktopWindow::OnKeyDown(uilepra::InputManager::KeyCode key_code) {
	return (Parent::OnKeyDown(key_code));
}

bool DesktopWindow::OnKeyUp(uilepra::InputManager::KeyCode key_code) {
	return (Parent::OnKeyUp(key_code));
}

bool DesktopWindow::OnDoubleClick() {
	Parent::OnDoubleClick();

	if (mouse_enabled_ == true) {
		int _mouse_x;
		int _mouse_y;
		GetCursorPosition(_mouse_x, _mouse_y);
		Parent::OnDoubleClick(_mouse_x, _mouse_y);
	}
	return (false);
}

DesktopWindow::MouseButtonFlags DesktopWindow::GetMouseButtonFlags() const {
	return (MouseButtonFlags)mouse_button_flags_;
}

void DesktopWindow::PostDeleteComponent(Component* component, int /*layer*/) {
	delete_queue_.push_back(component);
}

Painter* DesktopWindow::GetPainter() {
	deb_assert(this);
	return (painter_);
}

void DesktopWindow::SetKeyboardEnabled(bool enabled) {
	keyboard_enabled_ = enabled;
}

void DesktopWindow::SetUpdateLayout(bool update_layout) {
	update_layout_ = update_layout;
}

void DesktopWindow::AddIdleSubscriber(Component* component) {
	idle_subscribers_.push_back(component);
	idle_subscribers_.unique();
}

void DesktopWindow::RemoveIdleSubscriber(Component* component) {
	idle_subscribers_.remove(component);
}

void DesktopWindow::ActivateKeyboard() {
	input_manager_->ActivateKeyboard();
}

void DesktopWindow::DeactivateKeyboard() {
	input_manager_->ReleaseKeyboard();
}

void DesktopWindow::AddCleaner(Cleaner* cleaner) {
	cleaner_list_.push_back(cleaner);
	cleaner_list_.unique();
}

Component::Type DesktopWindow::GetType() const {
	return Component::kDesktopwindow;
}



loginstance(kUiGfx2D, DesktopWindow);



}
