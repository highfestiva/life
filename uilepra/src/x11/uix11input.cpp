
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/x11/uix11input.h"
#include "../../../lepra/include/log.h"
#include "../../include/x11/uix11core.h"
#include "../../include/x11/uix11displaymanager.h"
#include <X11/cursorfont.h>

#define kMouseButtonCount	10
#define kMouseAxisCount	10


namespace uilepra {



InputManager* InputManager::CreateInputManager(DisplayManager* display_manager) {
	return (new X11InputManager((X11DisplayManager*)display_manager));
}



X11InputElement::X11InputElement(Type _type, Interpretation interpretation, int type_index, X11InputDevice* parent_device):
	InputElement(_type, interpretation, type_index, parent_device) {
	/*SetIdentifier(element_->tszName);

	data_format_.dwType  = element_->dwType;
	data_format_.pguid   = 0;
	data_format_.dwOfs   = (DWORD)field_offset;
	data_format_.dwFlags = 0;*/
}

X11InputElement::~X11InputElement() {
}



loginstance(kUiInput, X11InputElement);



/*
	class X11InputDevice
*/

X11InputDevice::X11InputDevice(InputManager* manager):
	InputDevice(manager) {
	/*SetIdentifier(info->tszInstanceName);

	di_device_->EnumObjects(EnumElementsCallback, this, DIDFT_ALL);

	int num_elements = (int)element_array_.size();

	// Input data to use in buffered mode.
	device_object_data_ = new DIDEVICEOBJECTDATA[num_elements];
	memset(device_object_data_, 0, num_elements * sizeof(DIDEVICEOBJECTDATA));

	// Create the DirectInput data format description.
	memset(&data_format_, 0, sizeof(data_format_));
	data_format_.dwSize     = sizeof(data_format_);
	data_format_.dwObjSize  = sizeof(DIOBJECTDATAFORMAT);
	data_format_.dwDataSize = num_elements * sizeof(unsigned);
	data_format_.dwNumObjs  = num_elements;
	data_format_.rgodf      = new DIOBJECTDATAFORMAT[num_elements];

	data_format_.dwFlags = 0;
	if (abs_axis_count_ > 0) {
		data_format_.dwFlags |= DIDF_ABSAXIS;
	}
	if (rel_axis_count_ > 0) {
		data_format_.dwFlags |= DIDF_RELAXIS;
	}

	memset(data_format_.rgodf, 0, data_format_.dwObjSize * num_elements);

	// Fill the data format description with the correct data.
	ElementArray::iterator iter;
	int i;
	for (i = 0, iter = element_array_.begin();
		iter != element_array_.end();
		++i, ++iter) {
		device_object_data_[i].dwOfs = i * sizeof(unsigned);

		X11InputElement* _element = (X11InputElement*)(*iter);

		memcpy(&data_format_.rgodf[i], _element->GetDataFormat(), sizeof(DIOBJECTDATAFORMAT));
	}*/
}

X11InputDevice::~X11InputDevice() {
	if (IsActive() == true) {
		//di_device_->Unacquire();
	}
	//di_device_->Release();
}

void X11InputDevice::AddElement(X11InputElement* element) {
	element_array_.push_back(element);
}

void X11InputDevice::Activate() {
	if (IsActive() == false) {
		/*HRESULT hr;
		DWORD cooperative_flags = 0;
		if (this == GetManager()->GetKeyboard()) {
			cooperative_flags |= DISCL_NOWINKEY;
		}
		hr = di_device_->SetCooperativeLevel(((X11InputManager*)GetManager())->GetDisplayManager()->GetHWND(), DISCL_NONEXCLUSIVE|DISCL_FOREGROUND|cooperative_flags);
		hr = di_device_->SetDataFormat(&data_format_);

		DIPROPDWORD prop;
		prop.diph.dwSize = sizeof(DIPROPDWORD);
		prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		prop.diph.dwHow = DIPH_DEVICE;
		prop.diph.dwObj = 0;
		prop.dwData = 1024;
		di_device_->SetProperty(DIPROP_BUFFERSIZE, &prop.diph);*/

		//reacquire_ = true;
		SetActive(true);
	}
}

void X11InputDevice::Release() {
	//di_device_->Unacquire();
	SetActive(false);
}

void X11InputDevice::PollEvents() {
}



loginstance(kUiInput, X11InputDevice);



X11InputManager::X11InputManager(X11DisplayManager* display_manager):
	display_manager_(display_manager),
	//direct_input_(0),
	initialized_(false),
	screen_width_(1024),
	screen_height_(768),
	cursor_x_(0),
	cursor_y_(0),
	ignore_next_mouse_move_(true),
	grab_cursor_(false),
	mouse_grab_x_(0),
	mouse_grab_y_(0),
	mouse_grab_delta_x_(0),
	mouse_grab_delta_y_(0),
	keyboard_(0),
	mouse_(0) {
	int mx = 0;
	int my = 0;
	if (display_manager_) {
		XIM input_method = XOpenIM(display_manager_->GetDisplay(), NULL, NULL, NULL);
		input_context_ = XCreateIC(input_method, XNInputStyle, XIMPreeditNothing|XIMStatusNothing, XNClientWindow, display_manager_->GetWindow(), NULL);;

		Window rw, cw;
		int _;
		unsigned mask;
		XQueryPointer(display_manager_->GetDisplay(), display_manager_->GetWindow(), &rw, &cw, &_, &_, &mx, &my, &mask);
	}
	SetMousePosition(mx, my);
	mouse_grab_x_ = mx;
	mouse_grab_y_ = my;

	::memset(&type_count_, 0, sizeof(type_count_));
	++type_count_[InputDevice::kTypeKeyboard];
	++type_count_[InputDevice::kTypeMouse];
	keyboard_ = new X11InputDevice(this);
	keyboard_->SetInterpretation(InputDevice::kTypeKeyboard, 0);
	X11InputDevice* mouse = new X11InputDevice(this);
	mouse_ = mouse;
	mouse_->SetInterpretation(InputDevice::kTypeMouse, 0);
	for (int x = 0; x < kMouseButtonCount; ++x) {
		mouse->AddElement(new X11InputElement(InputElement::kDigital, InputElement::kButton, x, mouse));
	}
	for (int x = 0; x < kMouseAxisCount; ++x) {
		mouse->AddElement(new X11InputElement(InputElement::kAnalogue, InputElement::kRelativeAxis, x, mouse));
	}
	device_list_.push_back(keyboard_);
	device_list_.push_back(mouse_);

	Refresh();

	AddObserver();

	initialized_ = true;
}

X11InputManager::~X11InputManager() {
	if (initialized_ == true) {
		//direct_input_->Release();
	}

	RemoveObserver();

	display_manager_ = 0;
}

void X11InputManager::PreProcessEvents() {
	mouse_->GetAxis(0)->SetValue(0);
	mouse_->GetAxis(1)->SetValue(0);
	mouse_->GetAxis(2)->SetValue(0);

	// Warp pointer.
	if (grab_cursor_) {
		const int sx = screen_width_/2;
		const int sy = screen_height_/2;
		Window rw, cw;
		int _, x, y;
		unsigned mask;
		XQueryPointer(display_manager_->GetDisplay(), display_manager_->GetWindow(), &rw, &cw, &_, &_, &x, &y, &mask);
		if (abs(x-sx) >= 100 || abs(y-sy) >= 100) {
			//mLog.Infof("mwarp: (%i; %i"), sx-x, sy-y);
			mouse_grab_delta_x_ += sx - x;
			mouse_grab_delta_y_ += sy - y;
			XWarpPointer(display_manager_->GetDisplay(), None, display_manager_->GetWindow(),
					0, 0, 0, 0, sx, sy);
		}
	}
}

void X11InputManager::PollEvents() {
	Parent::PollEvents();

	if (mouse_grab_delta_x_ || mouse_grab_delta_y_) {
		mouse_grab_x_ += mouse_grab_delta_x_;
		mouse_grab_y_ += mouse_grab_delta_y_;
		mouse_grab_delta_x_ = mouse_grab_delta_y_ = 0;
	}
}

void X11InputManager::Refresh() {
	if (display_manager_ != 0) {
		screen_width_  = display_manager_->GetWidth();
		screen_height_ = display_manager_->GetHeight();
	}
}

X11DisplayManager* X11InputManager::GetDisplayManager() const {
	return (display_manager_);
}

bool X11InputManager::OnMessage(const XEvent& event) {
	bool consumed = false;
	switch (event.type) {
		case KeyPress:
		case KeyRelease: {
			XKeyEvent& key_event = (XKeyEvent&)event;
			char key[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
			KeySym sym;
			if (event.type == KeyPress) {
				::Xutf8LookupString(input_context_, &key_event, key, sizeof(key), &sym, NULL);
			} else {
				::XLookupString(&key_event, key, sizeof(key), &sym, NULL);
			}
			bool _is_special_key;
			KeyCode key_code = TranslateKey(key_event.state, sym, _is_special_key);
			//mLog.Infof("Key event %i: keycode=%i, state=%i, lookup=%s, keysym=%i, keycode=%i",
			//		lKeyEvent.type, lKeyEvent.keycode, lKeyEvent.state, lKey, lSym, lKeyCode);
			if (event.type == KeyPress) {
				SetKey(key_code, true);
				consumed = NotifyOnKeyDown(key_code);

				if (!_is_special_key && !consumed && key[0]) {
					wstr wide_key = wstrutil::Encode(key);
					consumed |= NotifyOnChar(wide_key[0]);
				}
			} else {
				if (XEventsQueued(display_manager_->GetDisplay(), QueuedAfterReading)) {
					XEvent e;
					XPeekEvent(display_manager_->GetDisplay(), &e);
					if (e.type == KeyPress &&
						e.xkey.time == key_event.time &&
						e.xkey.keycode == key_event.keycode) {
						consumed = true;
						break;
					}
				}
				SetKey(key_code, false);
				consumed = NotifyOnKeyUp(key_code);
			}
		} break;
		case ButtonPress:
		case ButtonRelease: {
			const XButtonEvent& __button = (const XButtonEvent&)event;
			int button_index = __button.button-1;
			if (button_index == 1) {
				button_index = 2;
			} else if (button_index == 2) {
				button_index = 1;
			}
			mouse_->GetButton(button_index)->SetValue((event.type == ButtonPress)? 1.0f : 0.0f);
			if (button_index == 3) {
				mouse_->GetAxis(2)->AddValue(+1);
			}
			if (button_index == 4) {
				mouse_->GetAxis(2)->AddValue(-1);
			}
		} break;
		case MotionNotify: {
			const XMotionEvent& motion = (const XMotionEvent&)event;
			SetMousePosition(motion.x, motion.y);
			const int x = motion.x;
			const int y = motion.y;
			const int dx = x - mouse_grab_x_;
			const int dy = y - mouse_grab_y_;
			mouse_grab_x_ = x;
			mouse_grab_y_ = y;
			if (ignore_next_mouse_move_) {
				ignore_next_mouse_move_ = false;
				break;
			}
			//mLog.Infof("mmove: (%i; %i, cnt=%i"), dx, dy, mWarpCount);
			mouse_->GetAxis(0)->AddValue(dx);
			mouse_->GetAxis(1)->AddValue(dy);
		} break;
		case FocusIn: {
			if (!is_cursor_visible_) {
				is_cursor_visible_ = true;
				SetCursorVisible(false);
			}
			ignore_next_mouse_move_ = true;
		} break;
		case FocusOut: {
			if (!is_cursor_visible_) {
				SetCursorVisible(true);
				is_cursor_visible_ = false;
			}
			ignore_next_mouse_move_ = true;
		} break;
		case EnterNotify: {
			ignore_next_mouse_move_ = true;
		} break;
	}
	return (consumed);
}

X11InputManager::KeyCode X11InputManager::TranslateKey(int _state, KeySym key_sym, bool& is_special_key) {
	switch (key_sym) {
		case 65505:	// LSHIFT;
		case 65507:	// LCTRL;
		case 65513:	// LALT;
		case 65506:	// RSHIFT;
		case 65508:	// RCTRL;
		case 65027:	// RALT;
		case 65361:	// LEFT;
		case 65362:	// UP;
		case 65363:	// RIGHT;
		case 65364:	// DOWN;
		case 65365:	// PGUP;
		case 65366:	// PGDOWN;
		case 65367:	// END;
		case 65360:	// HOME;
		case 65535:	// DEL;
			is_special_key = true;
			break;
		default:
			is_special_key = false;
			break;
	}
	switch (key_sym) {
		case 65288:	return kInKbdBackspace;
		case 65289:	return kInKbdTab;
		case 65293:	return kInKbdEnter;
		case 65421:	return kInKbdEnter;	// Numpad.
		case 65437:	return kInKbdCenter;
		case 65505:	return kInKbdLShift;
		case 65507:	return kInKbdLCtrl;
		case 65513:	return kInKbdLAlt;
		case 65299:	return kInKbdPause;
		case 65509:	return kInKbdCapsLock;

		case 65307:	return kInKbdEsc;

		case 65365:	return kInKbdPgUp;
		case 65366:	return kInKbdPgDown;
		case 65367:	return kInKbdEnd;
		case 65360:	return kInKbdHome;
		case 65361:	return kInKbdLeft;
		case 65362:	return kInKbdUp;
		case 65363:	return kInKbdRight;
		case 65364:	return kInKbdDown;
		case 65379:	return kInKbdInsert;
		case 65535:	return kInKbdDel;

		case 65516:	return kInKbdROs;
		case 65383:	return kInKbdContextMenu;

		case 65456:	return kInKbdNumpad0;
		case 65457:	return kInKbdNumpad1;
		case 65458:	return kInKbdNumpad2;
		case 65459:	return kInKbdNumpad3;
		case 65460:	return kInKbdNumpad4;
		case 65461:	return kInKbdNumpad5;
		case 65462:	return kInKbdNumpad6;
		case 65463:	return kInKbdNumpad7;
		case 65464:	return kInKbdNumpad8;
		case 65465:	return kInKbdNumpad9;

		case 65455:	return kInKbdNumpadDiv;
		case 65450:	return kInKbdNumpadMul;
		case 65453:	return kInKbdNumpadMinus;
		case 65451:	return kInKbdNumpadPlus;
		case 65452:	return kInKbdNumpadDot;

		case 65470:	return kInKbdF1;
		case 65471:	return kInKbdF2;
		case 65472:	return kInKbdF3;
		case 65473:	return kInKbdF4;
		case 65474:	return kInKbdF5;
		case 65475:	return kInKbdF6;
		case 65476:	return kInKbdF7;
		case 65477:	return kInKbdF8;
		case 65478:	return kInKbdF9;
		case 65479:	return kInKbdF10;
		case 65480:	return kInKbdF11;
		case 65481:	return kInKbdF12;

		case 65407:	return kInKbdNumLock;
		case 65300:	return kInKbdScrollLock;

		case 65111:	return kInKbdDiaeresis;
		case '+':	return kInKbdPlus;
		case ',':	return kInKbdComma;
		case '.':	return kInKbdDot;
		case '-':	return kInKbdMinus;
		case 65104:	return kInKbdApostrophe;
		case 65105:	return kInKbdApostrophe;

		case 246:	return kInKbdOe;
		case 167:	return kInKbdParagraph;
		case 229:	return kInKbdAa;
		case 228:	return kInKbdAe;

		case '<':	return kInKbdCompare;
		case '>':	return kInKbdCompare;

		case 65506:	return kInKbdRShift;
		case 65508:	return kInKbdRCtrl;
		case 65027:	return kInKbdRAlt;
	}
	if (key_sym >= 'a' && key_sym <= 'z') {
		return KeyCode(key_sym-'a'+'A');
	}
	return (KeyCode)key_sym;
}

void X11InputManager::SetCursorVisible(bool visible) {
	if (is_cursor_visible_ != visible) {
		if (visible) {
			log_volatile(log_.Debug("Showing cursor."));
			Cursor cursor = XCreateFontCursor(display_manager_->GetDisplay(), XC_X_cursor);
			XDefineCursor(display_manager_->GetDisplay(), display_manager_->GetWindow(), cursor);
			XFreeCursor(display_manager_->GetDisplay(), cursor);

			XUngrabPointer(display_manager_->GetDisplay(), CurrentTime);
			grab_cursor_ = false;
		} else {
			log_volatile(log_.Debug("Hiding cursor."));
			XColor black;
			black.red = black.green = black.blue = 0;
			static char noData[] = { 0,0,0,0,0,0,0,0 };
			Pixmap bitmap_no_data = XCreateBitmapFromData(display_manager_->GetDisplay(), display_manager_->GetWindow(), noData, 8, 8);
			Cursor invisible_cursor = XCreatePixmapCursor(display_manager_->GetDisplay(), bitmap_no_data, bitmap_no_data,
								&black, &black, 0, 0);
			XDefineCursor(display_manager_->GetDisplay(), display_manager_->GetWindow(), invisible_cursor);
			XFreeCursor(display_manager_->GetDisplay(), invisible_cursor);
			XFreePixmap(display_manager_->GetDisplay(), bitmap_no_data);

			XGrabPointer(display_manager_->GetDisplay(), display_manager_->GetWindow(), True, 0, GrabModeAsync, GrabModeAsync, display_manager_->GetWindow(), None, CurrentTime);
			grab_cursor_ = true;
		}
		is_cursor_visible_ = visible;
	}
}

float X11InputManager::GetCursorX() {
	return cursor_x_;
}

float X11InputManager::GetCursorY() {
	return cursor_y_;
}

void X11InputManager::SetMousePosition(int x, int y) {
	Parent::SetMousePosition(x, y);
	cursor_x_ = 2.0f * x / screen_width_  - 1.0f;
	cursor_y_ = 2.0f * y / screen_height_ - 1.0f;
}

const InputDevice* X11InputManager::GetKeyboard() const {
	return keyboard_;
}

InputDevice* X11InputManager::GetKeyboard() {
	return keyboard_;
}

const InputDevice* X11InputManager::GetMouse() const {
	return mouse_;
}

InputDevice* X11InputManager::GetMouse() {
	return mouse_;
}

void X11InputManager::AddObserver() {
	if (display_manager_) {
		// Listen to text input and standard mouse events.
		display_manager_->AddObserver(KeyPress, this);
		display_manager_->AddObserver(KeyRelease, this);
		display_manager_->AddObserver(ButtonPress, this);
		display_manager_->AddObserver(ButtonRelease, this);
		display_manager_->AddObserver(MotionNotify, this);
		display_manager_->AddObserver(FocusIn, this);
		display_manager_->AddObserver(FocusOut, this);
		display_manager_->AddObserver(EnterNotify, this);
	}
}

void X11InputManager::RemoveObserver() {
	if (display_manager_) {
		display_manager_->RemoveObserver(this);
	}
}

bool X11InputManager::IsInitialized() {
	return initialized_;
}



loginstance(kUiInput, X11InputManager);



}
