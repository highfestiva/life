
// Author: Jonas Bystrm
// Copyright (c) Pixel Doctrine



#include "../../include/mac/uiiosinput.h"
#ifdef LEPRA_IOS
//#include "../../../lepra/include/cyclicarray.h"
//#include "../../../lepra/include/log.h"
//#include "../../include/mac/uimaccore.h"
//#include "../../Include/Mac/UiMacOpenGLDisplayManager.h"



namespace uilepra {



InputManager* InputManager::CreateInputManager(DisplayManager* display_manager) {
	return (new IosInputManager((MacOpenGLDisplay*)display_manager));
}



IosInputElement::IosInputElement(Type type, Interpretation interpretation, int type_index,
	IosInputDevice* parent_device):
	InputElement(type, interpretation, type_index, parent_device) {
}

IosInputElement::~IosInputElement() {
}

loginstance(kUiInput, IosInputElement);



/*
	class IosInputDevice
*/

IosInputDevice::IosInputDevice(InputManager* manager):
	InputDevice(manager),
	rel_axis_count_(0),
	abs_axis_count_(0),
	analogue_count_(0),
	button_count_(0) {
}

IosInputDevice::~IosInputDevice() {
	if (IsActive() == true) {
		Release();
	}
	//native_device_->Release();

	ElementArray::iterator x;
	for (x = element_array_.begin(); x != element_array_.end(); ++x) {
		InputElement* _element = *x;
		delete _element;
	}
	element_array_.clear();
}

void IosInputDevice::Activate() {
	if (IsActive() == false) {
		//native_device_->Acquire();
		SetActive(true);
	}
}

void IosInputDevice::Release() {
	//native_device_->Unacquire();
	SetActive(false);
}

void IosInputDevice::PollEvents() {
	if (IsActive() == true) {
		ElementArray::iterator x;
		for (x = element_array_.begin(); x != element_array_.end(); ++x) {
			IosInputElement* _element = (IosInputElement*)*x;
			_element->SetValue(_element->GetValue());	// TODO: do some stuff?
		}
	}
}

void IosInputDevice::AddElement(InputElement* element) {
	element_array_.push_back(element);
}



loginstance(kUiInput, IosInputDevice);



IosInputManager::IosInputManager(MacOpenGLDisplay* display_manager):
	display_manager_(display_manager),
	screen_width_(0),
	screen_height_(0),
	cursor_x_(0),
	cursor_y_(0),
	keyboard_(0),
	mouse_(0) {
	Refresh();

	AddObserver();

	IosInputDevice* touch = new IosInputDevice(this);
	IosInputElement* x = new IosInputElement(InputElement::kAnalogue, InputElement::kRelativeAxis, 0, touch);
	IosInputElement* y = new IosInputElement(InputElement::kAnalogue, InputElement::kRelativeAxis, 1, touch);
	IosInputElement* button = new IosInputElement(InputElement::kDigital, InputElement::kButton, 0, touch);
	touch->SetIdentifier("IosVirtualMouse");
	x->SetIdentifier("RelAxisX");
	y->SetIdentifier("RelAxisY");
	button->SetIdentifier("Button");
	touch->AddElement(x);
	touch->AddElement(y);
	touch->AddElement(button);
	x->SetValue(0);
	y->SetValue(0);
	button->SetValue(1);
	x->SetValue(screen_width_);
	y->SetValue(screen_height_);
	button->SetValue(0);
	device_list_.push_back(touch);
	mouse_ = touch;

	initialized_ = true;
}

IosInputManager::~IosInputManager() {
	if (initialized_ == true) {
		//HIDReleaseDeviceList();
	}

	RemoveObserver();

	display_manager_ = 0;
}

bool IosInputManager::IsInitialized() {
	return (initialized_);
}

void IosInputManager::Refresh() {
	screen_width_  = display_manager_->GetWidth();
	screen_height_ = display_manager_->GetHeight();
	/*if (display_manager_ != 0 && display_manager_->GetHWND() != 0) {
		RECT rect;
		::GetClientRect(display_manager_->GetHWND(), &rect);

		screen_width_  = rect.right - rect.left;
		screen_height_ = rect.bottom - rect.top;
	} else {
		// Get the entire screen area.
		screen_width_  = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
		screen_height_ = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);

		if (screen_width_ == 0 || screen_height_ == 0) {
			// Virtual screen not supported, use the primary display.
			screen_width_  = ::GetSystemMetrics(SM_CXSCREEN);
			screen_height_ = ::GetSystemMetrics(SM_CYSCREEN);
		}
	}*/
}

MacDisplayManager* IosInputManager::GetDisplayManager() const {
	return (display_manager_);
}

void IosInputManager::ActivateKeyboard() {
	EAGLView* view = display_manager_->GetGlView();
	[view becomeFirstResponder];
}

void IosInputManager::ReleaseKeyboard() {
	EAGLView* view = display_manager_->GetGlView();
	[view resignFirstResponder];
}

void IosInputManager::SetCursorVisible(bool visible) {
	(void)visible;
}

float IosInputManager::GetCursorX() {
	return cursor_x_;
}

float IosInputManager::GetCursorY() {
	return cursor_y_;
}

void IosInputManager::SetMousePosition(int x, int y) {
	cursor_x_ = 2.0 * x / screen_width_  - 1.0;
	cursor_y_ = 2.0 * y / screen_height_ - 1.0;
}

const InputDevice* IosInputManager::GetKeyboard() const {
	return keyboard_;
}

InputDevice* IosInputManager::GetKeyboard() {
	return keyboard_;
}

const InputDevice* IosInputManager::GetMouse() const {
	return mouse_;
}

InputDevice* IosInputManager::GetMouse() {
	return mouse_;
}

void IosInputManager::OnEvent(LEPRA_APPLE_EVENT* e) {
}

void IosInputManager::AddObserver() {
	if (display_manager_) {
		EAGLView* view = display_manager_->GetGlView();
		view.inputManager = this;
	}
}

void IosInputManager::RemoveObserver() {
	if (display_manager_) {
		display_manager_->RemoveObserver(this);
	}
}

loginstance(kUiInput, IosInputManager);



}

#endif // iOS
