
// Author: Jonas Bystrm
// Copyright (c) Pixel Doctrine



#include "../../include/mac/uimacinput.h"
#ifndef LEPRA_IOS
#include "../../../lepra/include/cyclicarray.h"
#include "../../../lepra/include/log.h"
#include "../../include/mac/uimaccore.h"
#include "../../include/mac/uimacdisplaymanager.h"



namespace uilepra {



InputManager* InputManager::CreateInputManager(DisplayManager* display_manager) {
	return (new MacInputManager((MacDisplayManager*)display_manager));
}



MacInputElement::MacInputElement(Type _type, Interpretation interpretation, int type_index,
	MacInputDevice* parent_device, pRecElement element):
	InputElement(_type, interpretation, type_index, parent_device),
	element_(element) {
	parent_device->CountElements();
	str __name;
	if (_type == kAnalogue) {
		__name = "Axis" + strutil::IntToString(parent_device->GetNumAnalogueElements(), 10);
	} else {
		__name = str("Button") + strutil::IntToString(parent_device->GetNumDigitalElements(), 10);
	}
	SetIdentifier(__name);
	//log_.Headlinef("%s got Mac input element %s of type %i w/ index %i.", parent_device->GetIdentifier().c_str(), __name, GetType(), type_index);
}

MacInputElement::~MacInputElement() {
}

pRecElement MacInputElement::GetNativeElement() const {
	return (element_);
}

loginstance(kUiInput, MacInputElement);



/*
	class MacInputDevice
*/

MacInputDevice::MacInputDevice(pRecDevice native_device, InputManager* manager):
	InputDevice(manager),
	native_device_(native_device),
	rel_axis_count_(0),
	abs_axis_count_(0),
	analogue_count_(0),
	button_count_(0) {
	SetIdentifier(str(native_device->product));
	EnumElements();
}

MacInputDevice::~MacInputDevice() {
	if (IsActive() == true) {
		Release();
	}
	//native_device_->Release();
}

void MacInputDevice::Activate() {
	if (IsActive() == false) {
		//native_device_->Acquire();
		SetActive(true);
	}
}

void MacInputDevice::Release() {
	//native_device_->Unacquire();
	SetActive(false);
}

void MacInputDevice::PollEvents() {
	if (IsActive() == true) {
		typedef std::vector<MacInputElement*> MacElementArray;
		MacElementArray relative_axes;
		ElementArray::iterator x;
		for (x = element_array_.begin(); x != element_array_.end(); ++x) {
			MacInputElement* _element = (MacInputElement*)*x;
			if (_element->GetInterpretation() == InputElement::kRelativeAxis) {
				relative_axes.push_back(_element);
				continue;
			}
			pRecElement native_element = _element->GetNativeElement();
			const int32 __value = HIDGetElementValue(native_device_, native_element);
			_element->SetValue(__value);
		}

		if (!relative_axes.empty()) {
			IOHIDEventStruct hid_event;
			for (int z = 0; z < 20 && HIDGetEvent(native_device_, &hid_event); ++z) {
				MacElementArray::iterator y = relative_axes.begin();
				for (; y != relative_axes.end(); ++y) {
					MacInputElement* _element = *y;
					if (_element->GetNativeElement()->cookie == hid_event.elementCookie) {
						const int __value = hid_event.value;
						_element->SetValue(__value);
						break;
					}
				}
			}
		}
	}
}

void MacInputDevice::EnumElements() {
	pRecElement current_element = HIDGetFirstDeviceElement(native_device_, kHIDElementTypeInput);
	while (current_element) {
		MacInputElement* _element = 0;
		switch (current_element->type) {
			case kIOHIDElementTypeInput_Button: {
				_element = new MacInputElement(InputElement::kDigital, InputElement::kButton, button_count_, this, current_element);
				++button_count_;
			} break;
			case kIOHIDElementTypeInput_Axis: {
				InputElement::Interpretation _interpretation = InputElement::kAbsoluteAxis;
				if (current_element->relative) {
					_interpretation = InputElement::kRelativeAxis;
				}
				_element = new MacInputElement(InputElement::kAnalogue, _interpretation, analogue_count_, this, current_element);
				if (current_element->relative) {
					HIDQueueElement(native_device_, current_element);
				}
				++analogue_count_;
			} break;
			case kIOHIDElementTypeInput_Misc: {
				char name[256];
				HIDGetUsageName(current_element->usagePage, current_element->usage, name);
				if (current_element->usagePage == kHIDPage_GenericDesktop) {
					switch(current_element->usage) {
						case kHIDUsage_GD_DPadUp:
						case kHIDUsage_GD_DPadDown:
						case kHIDUsage_GD_DPadRight:
						case kHIDUsage_GD_DPadLeft: {
							_element = new MacInputElement(InputElement::kDigital, InputElement::kButton, button_count_, this, current_element);
							++button_count_;
						} break;
						case kHIDUsage_GD_X:
						case kHIDUsage_GD_Y:
						case kHIDUsage_GD_Z:
						case kHIDUsage_GD_Rx:
						case kHIDUsage_GD_Ry:
						case kHIDUsage_GD_Rz:
						case kHIDUsage_GD_Slider:
						case kHIDUsage_GD_Wheel: {
							InputElement::Interpretation _interpretation = InputElement::kAbsoluteAxis;
							if (current_element->relative) {
								_interpretation = InputElement::kRelativeAxis;
							}
							_element = new MacInputElement(InputElement::kAnalogue, _interpretation, analogue_count_, this, current_element);
							if (current_element->relative) {
								HIDQueueElement(native_device_, current_element);
							}
							++analogue_count_;
						} break;
						default: {
							/*printf("Misc element:\n");
							printf("-------------\n");
							printf("name: %s\n", current_element->name);
							printf("usagePage: %li\nusage: %li\n", current_element->usagePage, current_element->usage);
							printf("usagename: %s\n", name);*/
						} break;
					}
				}
			} break;
			/*case kIOHIDElementTypeInput_ScanCodes: {
				printf("warning: kIOHIDElementTypeInput_ScanCodes support not implemented yet!\n");
				printf("min: %li\nmax: %li\nsize: %li\n", current_element->min, current_element->max, current_element->size);
			} break;
			default: {
				printf("unknown element type: %d\n", current_element->type);
			}*/
			break;
		}
		if (_element) {
			element_array_.push_back(_element);
		}
		current_element = HIDGetNextDeviceElement(current_element, kHIDElementTypeInput);
	}
}



loginstance(kUiInput, MacInputDevice);



MacInputManager::MacInputManager(MacDisplayManager* display_manager):
	display_manager_(display_manager),
	enum_error_(false),
	initialized_(false),
	key_modifiers_(0),
	screen_width_(0),
	screen_height_(0),
	cursor_x_(0),
	cursor_y_(0),
	keyboard_(0),
	mouse_(0) {
	HIDBuildDeviceList(0, 0);
	if (HIDHaveDeviceList()) {
		EnumDevices();
	}

	Refresh();

	AddObserver();

	initialized_ = true;
}

MacInputManager::~MacInputManager() {
	if (initialized_ == true) {
		HIDReleaseDeviceList();
	}

	RemoveObserver();

	display_manager_ = 0;
}

bool MacInputManager::IsInitialized() {
	return (initialized_);
}

void MacInputManager::Refresh() {
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

MacInputManager::KeyCode MacInputManager::ConvertMacKeyCodeToKeyCode(unsigned mac_key_code) {
	switch (mac_key_code) {
		case kVK_ANSI_A:		return kInKbdA;
		case kVK_ANSI_B:		return kInKbdB;
		case kVK_ANSI_C:		return kInKbdC;
		case kVK_ANSI_D:		return kInKbdD;
		case kVK_ANSI_E:		return kInKbdE;
		case kVK_ANSI_F:		return kInKbdF;
		case kVK_ANSI_G:		return kInKbdG;
		case kVK_ANSI_H:		return kInKbdH;
		case kVK_ANSI_I:		return kInKbdI;
		case kVK_ANSI_J:		return kInKbdJ;
		case kVK_ANSI_K:		return kInKbdK;
		case kVK_ANSI_L:		return kInKbdL;
		case kVK_ANSI_M:		return kInKbdM;
		case kVK_ANSI_N:		return kInKbdN;
		case kVK_ANSI_O:		return kInKbdO;
		case kVK_ANSI_P:		return kInKbdP;
		case kVK_ANSI_Q:		return kInKbdQ;
		case kVK_ANSI_R:		return kInKbdR;
		case kVK_ANSI_S:		return kInKbdS;
		case kVK_ANSI_T:		return kInKbdT;
		case kVK_ANSI_U:		return kInKbdU;
		case kVK_ANSI_V:		return kInKbdV;
		case kVK_ANSI_W:		return kInKbdW;
		case kVK_ANSI_X:		return kInKbdX;
		case kVK_ANSI_Y:		return kInKbdY;
		case kVK_ANSI_Z:		return kInKbdZ;
		case kVK_ANSI_1:		return kInKbd1;
		case kVK_ANSI_2:		return kInKbd2;
		case kVK_ANSI_3:		return kInKbd3;
		case kVK_ANSI_4:		return kInKbd4;
		case kVK_ANSI_5:		return kInKbd5;
		case kVK_ANSI_6:		return kInKbd6;
		case kVK_ANSI_7:		return kInKbd7;
		case kVK_ANSI_8:		return kInKbd8;
		case kVK_ANSI_9:		return kInKbd9;
		case kVK_ANSI_0:		return kInKbd0;
		case kVK_ANSI_Equal:		return kInKbdPlus;
		case kVK_ANSI_Minus:		return kInKbdAcute;
		case kVK_ANSI_RightBracket:	return kInKbdAa;
		case kVK_ANSI_LeftBracket:	return kInKbdDiaeresis;
		case kVK_ANSI_Quote:		return kInKbdOe;
		case kVK_ANSI_Semicolon:	return kInKbdAe;
		case kVK_ANSI_Backslash:	return kInKbdApostrophe;
		case kVK_ANSI_Comma:		return kInKbdComma;
		case kVK_ANSI_Slash:		return kInKbdMinus;
		case kVK_ANSI_Period:		return kInKbdDot;
		case kVK_ANSI_Grave:		return kInKbdParagraph;
		case kVK_ANSI_KeypadDecimal:	return kInKbdNumpadDot;
		case kVK_ANSI_KeypadMultiply:	return kInKbdNumpadMul;
		case kVK_ANSI_KeypadPlus:	return kInKbdNumpadPlus;
		case kVK_ANSI_KeypadClear:	return kInKbdNumLock;
		case kVK_ANSI_KeypadDivide:	return kInKbdNumpadDiv;
		case kVK_ANSI_KeypadEnter:	return kInKbdEnter;
		case kVK_ANSI_KeypadMinus:	return kInKbdNumpadMinus;
		case kVK_ANSI_KeypadEquals:	return kInKbdNumLock;
		case kVK_ANSI_Keypad0:		return kInKbdNumpad0;
		case kVK_ANSI_Keypad1:		return kInKbdNumpad1;
		case kVK_ANSI_Keypad2:		return kInKbdNumpad2;
		case kVK_ANSI_Keypad3:		return kInKbdNumpad3;
		case kVK_ANSI_Keypad4:		return kInKbdNumpad4;
		case kVK_ANSI_Keypad5:		return kInKbdNumpad5;
		case kVK_ANSI_Keypad6:		return kInKbdNumpad6;
		case kVK_ANSI_Keypad7:		return kInKbdNumpad7;
		case kVK_ANSI_Keypad8:		return kInKbdNumpad8;
		case kVK_ANSI_Keypad9:		return kInKbdNumpad9;
		case kVK_Return:		return kInKbdEnter;
		case kVK_Tab:			return kInKbdTab;
		case kVK_Space:			return kInKbdSpace;
		case kVK_Delete:		return kInKbdDel;
		case kVK_Escape:		return kInKbdEsc;
		case kVK_Command:		return kInKbdLalt;
		case kVK_Shift:			return kInKbdLshift;
		case kVK_CapsLock:		return kInKbdCapsLock;
		case kVK_Option:		return kInKbdLos;
		case kVK_Control:		return kInKbdLctrl;
		case kVK_RightShift:		return kInKbdRshift;
		case kVK_RightOption:		return kInKbdRos;
		case kVK_RightControl:		return kInKbdRctrl;
		case kVK_Function:		return kInKbdContextMenu;
		case kVK_F17:			return kInKbdF12;
		case kVK_VolumeUp:		return kInKbdQuickIncrVolume;
		case kVK_VolumeDown:		return kInKbdQuickDecrVolume;
		case kVK_Mute:			return kInKbdQuickSoundMute;
		case kVK_F18:			return kInKbdF12;
		case kVK_F19:			return kInKbdF12;
		case kVK_F20:			return kInKbdF12;
		case kVK_F5:			return kInKbdF5;
		case kVK_F6:			return kInKbdF6;
		case kVK_F7:			return kInKbdF7;
		case kVK_F3:			return kInKbdF3;
		case kVK_F8:			return kInKbdF8;
		case kVK_F9:			return kInKbdF9;
		case kVK_F11:			return kInKbdF11;
		case kVK_F13:			return kInKbdF12;
		case kVK_F16:			return kInKbdF12;
		case kVK_F14:			return kInKbdF12;
		case kVK_F10:			return kInKbdF10;
		case kVK_F12:			return kInKbdF12;
		case kVK_F15:			return kInKbdF12;
		case kVK_Help:			return kInKbdF1;
		case kVK_Home:			return kInKbdHome;
		case kVK_PageUp:		return kInKbdPgup;
		case kVK_ForwardDelete:		return kInKbdDel;
		case kVK_F4:			return kInKbdF4;
		case kVK_End:			return kInKbdEnd;
		case kVK_F2:			return kInKbdF2;
		case kVK_PageDown:		return kInKbdPgdown;
		case kVK_F1:			return kInKbdF1;
		case kVK_LeftArrow:		return kInKbdLeft;
		case kVK_RightArrow:		return kInKbdRight;
		case kVK_DownArrow:		return kInKbdDown;
		case kVK_UpArrow:		return kInKbdUp;
		case kVK_ISO_Section:		return kInKbdPause;
		case kVK_JIS_Yen:		return kInKbdPrintScreen;
		case kVK_JIS_Underscore:	return kInKbdPrintScreen;
		case kVK_JIS_KeypadComma:	return kInKbdNumpadDot;
		case kVK_JIS_Eisu:		return kInKbdNumLock;
		case kVK_JIS_Kana:		return kInKbdNumLock;
	}
	return kInKbdContextMenu;
}

unichar MacInputManager::ConvertChar(unichar c) {
	bool _is_char;
	ConvertCharToKeyCode(c, false, _is_char);
	if (!_is_char) {
		return 0;
	}
	switch (c) {
		case 3:		return '\r';	// Numpad-ENTER.
		case 0x7F:	return '\b';
	}
	return c;
}

MacInputManager::KeyCode MacInputManager::ConvertCharToKeyCode(unichar c, bool is_numpad, bool& is_char) {
	is_char = true;
#define NC()	is_char = false
	switch (c) {
		case NSUpArrowFunctionKey:	NC();	return kInKbdUp;
		case NSDownArrowFunctionKey:	NC();	return kInKbdDown;
		case NSLeftArrowFunctionKey:	NC();	return kInKbdLeft;
		case NSRightArrowFunctionKey:	NC();	return kInKbdRight;
		case NSF1FunctionKey:		NC();	return kInKbdF1;
		case NSF2FunctionKey:		NC();	return kInKbdF2;
		case NSF3FunctionKey:		NC();	return kInKbdF3;
		case NSF4FunctionKey:		NC();	return kInKbdF4;
		case NSF5FunctionKey:		NC();	return kInKbdF5;
		case NSF6FunctionKey:		NC();	return kInKbdF6;
		case NSF7FunctionKey:		NC();	return kInKbdF7;
		case NSF8FunctionKey:		NC();	return kInKbdF8;
		case NSF9FunctionKey:		NC();	return kInKbdF9;
		case NSF10FunctionKey:		NC();	return kInKbdF10;
		case NSF11FunctionKey:		NC();	return kInKbdF11;
		case NSF12FunctionKey:		NC();	return kInKbdF12;
		case 0xF746:	// Fall thru.
		case NSInsertFunctionKey:	NC();	return kInKbdInsert;
		case NSDeleteFunctionKey:	NC();	return kInKbdDel;
		case NSHomeFunctionKey:		NC();	return kInKbdHome;
		case NSEndFunctionKey:		NC();	return kInKbdEnd;
		case NSPageUpFunctionKey:	NC();	return kInKbdPgup;
		case NSPageDownFunctionKey:	NC();	return kInKbdPgdown;
		case NSSysReqFunctionKey:	// Fall thru.
		case NSPrintScreenFunctionKey:	NC();	return kInKbdPrintScreen;
		case NSScrollLockFunctionKey:	NC();	return kInKbdScrollLock;
		case NSBreakFunctionKey:	// Fall thru.
		case NSPauseFunctionKey:	NC();	return kInKbdPause;
		case NSMenuFunctionKey:		NC();	return kInKbdContextMenu;
	}
	if (c >= 'a' && c <= 'z') {
		c -= 'a'-'A';
	} else if (is_numpad) {
		if (c >= '0' && c <= '9') {
			return (KeyCode)(kInKbdNumpad0 + c - '0');
		}
		switch (c) {
			case '+':	return kInKbdNumpadPlus;
			case '-':	return kInKbdNumpadMinus;
			case '*':	return kInKbdNumpadMul;
			case '/':	return kInKbdNumpadDiv;
			case '.':	// Fall thru.
			case ',':	return kInKbdNumpadDot;
		}
	}
	return (KeyCode)c;
}

MacDisplayManager* MacInputManager::GetDisplayManager() const {
	return (display_manager_);
}

void MacInputManager::OnEvent(NSEvent* event) {
	bool is_key_down = false;
	const NSEventType __type = [event type];
	switch (__type) {
		case NSKeyDown: {
			is_key_down = true;
		}	// TRICKY: fall thru to handle keyDown notification BEFORE OnChar is dispatched!
		case NSKeyUp: {
			const unsigned _mac_key_code = [event keyCode];	// Hardware independent key code.
			bool consumed = false;
			const KeyCode key = ConvertMacKeyCodeToKeyCode(_mac_key_code);
			SetKey(key, is_key_down);
			//mLog.Infof("Got key %s: %x (%u, '%c'."), lIsKeyDown? "down" : "up", lMacKeyCode, lMacKeyCode, lMacKeyCode);
			if (is_key_down) {
				consumed |= NotifyOnKeyDown(key);
			} else {
				NotifyOnKeyUp(key);
			}

			// Handle OnChar dispatching.
			if (is_key_down && !consumed) {
				const NSString* __characters = [event characters];
				const unsigned character_count = [__characters length];
				for (unsigned x = 0; x < character_count; ++x) {
					unichar _c = [__characters characterAtIndex:x];
					//mLog.Infof("Got char: %x (%u, '%c'"), (unsigned)lChar, (unsigned)lChar, lChar);
					_c = ConvertChar(_c);
					if (_c) {
						NotifyOnChar(_c);
					}
				}
			}
		} break;
		case NSFlagsChanged: {
			const NSUInteger key_modifiers = [event modifierFlags];
			//mLog.Infof("Got key modifiers: %x (%u"), lKeyModifiers, lKeyModifiers);
			const NSUInteger delta_modifiers = key_modifiers_ ^ key_modifiers;
			const int translation_table[][2] =
			{
				{ uilepra::InputManager::kInKbdCapsLock,	NSAlphaShiftKeyMask, },
				{ uilepra::InputManager::kInKbdLshift,		NSShiftKeyMask, },
				{ uilepra::InputManager::kInKbdLctrl,		NSControlKeyMask, },
				{ uilepra::InputManager::kInKbdLalt,		NSAlternateKeyMask, },
				{ uilepra::InputManager::kInKbdLos,		NSCommandKeyMask, },
			};
			for (unsigned x = 0; x < LEPRA_ARRAY_COUNT(translation_table); ++x) {
				if (!(delta_modifiers & translation_table[x][1])) {
					continue;
				}
				const bool is_key_down = (key_modifiers & translation_table[x][1]);
				SetKey((KeyCode)translation_table[x][0], is_key_down);
				if (is_key_down) {
					NotifyOnKeyDown((KeyCode)translation_table[x][0]);
				} else {
					NotifyOnKeyUp((KeyCode)translation_table[x][0]);
				}
			}
			key_modifiers_ = key_modifiers;
		} break;
		case NSMouseMoved:
		case NSLeftMouseDragged:
		case NSRightMouseDragged: {
			NSPoint point = [event locationInWindow];
			SetMousePosition(point.x, point.y);
		} break;
	}
}

void MacInputManager::SetCursorVisible(bool visible) {
	if (visible) {
		CGDisplayShowCursor(kCGDirectMainDisplay);
		CGAssociateMouseAndMouseCursorPosition(true);
	} else {
		CGDisplayHideCursor(kCGDirectMainDisplay);
		CGAssociateMouseAndMouseCursorPosition(false);
	}
}

float MacInputManager::GetCursorX() {
	return cursor_x_;
}

float MacInputManager::GetCursorY() {
	return cursor_y_;
}

void MacInputManager::SetMousePosition(int x, int y) {
	Parent::SetMousePosition(x, screen_height_-y);
	cursor_x_ = 2.0 * x / screen_width_  - 1.0;
	cursor_y_ = 2.0 * (screen_height_-y) / screen_height_ - 1.0;
}

const InputDevice* MacInputManager::GetKeyboard() const {
	return keyboard_;
}

InputDevice* MacInputManager::GetKeyboard() {
	return keyboard_;
}

const InputDevice* MacInputManager::GetMouse() const {
	return mouse_;
}

InputDevice* MacInputManager::GetMouse() {
	return mouse_;
}

void MacInputManager::EnumDevices() {
	pRecDevice hid_device = HIDGetFirstDevice();
	while (hid_device) {
		MacInputDevice* device = new MacInputDevice(hid_device, this);
		device_list_.push_back(device);

		InputDevice::Interpretation _interpretation = InputDevice::kTypeOther;
		if (hid_device->usagePage == kHIDPage_GenericDesktop &&
			(hid_device->usage == kHIDUsage_GD_Pointer ||
			hid_device->usage == kHIDUsage_GD_Mouse)) {
			_interpretation = InputDevice::kTypeMouse;
		} else if (hid_device->usagePage == kHIDPage_GenericDesktop &&
			(hid_device->usage == kHIDUsage_GD_Keypad ||
			hid_device->usage == kHIDUsage_GD_Keyboard)) {
			_interpretation = InputDevice::kTypeKeyboard;
		} else if (hid_device->usagePage == kHIDPage_GenericDesktop &&
			(hid_device->usage == kHIDUsage_GD_Joystick ||
			hid_device->usage == kHIDUsage_GD_MultiAxisController)) {
			_interpretation = InputDevice::kTypeJoystick;
		} else if (hid_device->usagePage == kHIDPage_GenericDesktop &&
			hid_device->usage == kHIDUsage_GD_GamePad) {
			_interpretation = InputDevice::kTypeGamepad;
		} else {
			log_volatile(log_.Debugf("%s is not of known type: usage=%i, usagePage=%i.",
				device->GetIdentifier().c_str(), hid_device->usage, hid_device->usagePage));
		}
		device->SetInterpretation(_interpretation, type_count_[_interpretation]);
		++type_count_[_interpretation];
		if (!mouse_ && _interpretation == InputDevice::kTypeMouse) {
			mouse_ = device;
		} else if (!keyboard_ && _interpretation == InputDevice::kTypeKeyboard) {
			keyboard_ = device;
		}

		hid_device = HIDGetNextDevice(hid_device);
	}
}

void MacInputManager::AddObserver() {
	if (display_manager_) {
		// Listen to text input and standard mouse events.
		display_manager_->AddObserver(NSKeyDown, this);
		display_manager_->AddObserver(NSKeyUp, this);
		display_manager_->AddObserver(NSFlagsChanged, this);
		display_manager_->AddObserver(NSMouseMoved, this);
		display_manager_->AddObserver(NSLeftMouseDragged, this);
		display_manager_->AddObserver(NSRightMouseDragged, this);
	}
}

void MacInputManager::RemoveObserver() {
	if (display_manager_) {
		display_manager_->RemoveObserver((MacInputManager*)this);
	}
}

loginstance(kUiInput, MacInputManager);



}

#endif // !iOS
