
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine




#include "pch.h"
#include "../../../lepra/include/lepraos.h"
#include "../../include/win32/uiwin32input.h"
#include "../../../lepra/include/log.h"
#include "../../include/win32/uiwin32core.h"
#include "../../include/win32/uiwin32displaymanager.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")



namespace uilepra {



InputManager* InputManager::CreateInputManager(DisplayManager* display_manager) {
	return (new Win32InputManager((Win32DisplayManager*)display_manager));
}

Win32InputElement::Win32InputElement(Type type, Interpretation interpretation, int type_index,
	Win32InputDevice* parent_device, LPCDIDEVICEOBJECTINSTANCE element, unsigned field_offset):
	InputElement(type, interpretation, type_index, parent_device),
	element_(element) {
	SetIdentifier(element_->tszName);

	data_format_.dwType  = element_->dwType;
	data_format_.pguid   = 0;
	data_format_.dwOfs   = (DWORD)field_offset;
	data_format_.dwFlags = 0;
}

Win32InputElement::~Win32InputElement() {
}

loginstance(kUiInput, Win32InputElement);



/*
	class Win32InputDevice
*/

Win32InputDevice::Win32InputDevice(LPDIRECTINPUTDEVICE8 di_device, LPCDIDEVICEINSTANCE info, InputManager* manager):
	InputDevice(manager),
	di_device_(di_device),
	rel_axis_count_(0),
	abs_axis_count_(0),
	analogue_count_(0),
	button_count_(0) {
	SetIdentifier(info->tszInstanceName);

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

		Win32InputElement* _element = (Win32InputElement*)(*iter);

		memcpy(&data_format_.rgodf[i], _element->GetDataFormat(), sizeof(DIOBJECTDATAFORMAT));
	}
}

Win32InputDevice::~Win32InputDevice() {
	if (IsActive() == true) {
		di_device_->Unacquire();
	}
	di_device_->Release();

	delete[] device_object_data_;
	delete[] data_format_.rgodf;
}

BOOL CALLBACK Win32InputDevice::EnumElementsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID ref) {
	Win32InputDevice* device = (Win32InputDevice*)ref;

	Win32InputElement* _element = 0;
	// Is this an analogue or digital element?
	if ((lpddoi->dwType & DIDFT_ABSAXIS) != 0 ||
		(lpddoi->dwType & DIDFT_AXIS)    != 0 ||
		(lpddoi->dwType & DIDFT_POV)     != 0 ||
		(lpddoi->dwType & DIDFT_RELAXIS) != 0) {
		InputElement::Interpretation _interpretation = InputElement::kAbsoluteAxis;
		// Count number of relative and absolute axes.
		// These values are used later on in the constructor to determine
		// the data format.
		if ((lpddoi->dwType & DIDFT_RELAXIS) != 0) {
			++device->rel_axis_count_;
			_interpretation = InputElement::kRelativeAxis;
		} else if ((lpddoi->dwType & DIDFT_ABSAXIS) != 0) {
			device->abs_axis_count_++;
		}

		_element = new Win32InputElement(InputElement::kAnalogue, _interpretation, device->analogue_count_,
			device, lpddoi, (unsigned)device->element_array_.size() * sizeof(unsigned));
		++device->analogue_count_;

		// Set absolute axis range.
		if ((lpddoi->dwType & DIDFT_ABSAXIS) != 0) {
			log_volatile(log_.Debugf("Found absolute axis element '%s' = '%s'.",
				_element->GetFullName().c_str(),
				_element->GetIdentifier().c_str()));

			DIPROPRANGE range;
			range.diph.dwSize = sizeof(DIPROPRANGE);
			range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			range.diph.dwHow = DIPH_BYID;
			range.diph.dwObj = lpddoi->dwType;
			if (device->di_device_->GetProperty(DIPROP_RANGE, &range.diph) == DI_OK) {
				const int interval_range = range.lMax-range.lMin;
				const int mid = interval_range / 2 + range.lMin;
				const int min = mid - interval_range/2/8;	// Don't use full range, might not be physically accessible.
				const int max = mid + interval_range/2/8;	// Don't use full range, might not be physically accessible.
				_element->SetValue(min);
				_element->SetValue(max);
				_element->SetValue(mid);
			}
		}
	} else if((lpddoi->dwType&DIDFT_BUTTON)    != 0 ||
			(lpddoi->dwType&DIDFT_PSHBUTTON) != 0 ||
			(lpddoi->dwType&DIDFT_TGLBUTTON) != 0) {
		_element = new Win32InputElement(InputElement::kDigital, InputElement::kButton,
			device->button_count_, device, lpddoi, (unsigned)device->element_array_.size() * sizeof(unsigned));
		++device->button_count_;
	} else if(lpddoi->dwType&DIDFT_FFACTUATOR) {
		// TODO: handle force feedback elements!
	}

	if (_element) {
		_element->SetIdentifier(lpddoi->tszName);
		device->element_array_.push_back(_element);
	}

	return (DIENUM_CONTINUE);
}

void Win32InputDevice::Activate(bool disable_win_mgr) {
	if (IsActive() == false) {
		HRESULT hr;
		DWORD cooperative_flags = 0;
		if (disable_win_mgr && this == GetManager()->GetKeyboard()) {
			cooperative_flags |= DISCL_NOWINKEY;
		}
		hr = di_device_->SetCooperativeLevel(((Win32InputManager*)GetManager())->GetDisplayManager()->GetHWND(), DISCL_NONEXCLUSIVE|DISCL_FOREGROUND|cooperative_flags);
		hr = di_device_->SetDataFormat(&data_format_);

		DIPROPDWORD prop;
		prop.diph.dwSize = sizeof(DIPROPDWORD);
		prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		prop.diph.dwHow = DIPH_DEVICE;
		prop.diph.dwObj = 0;
		prop.dwData = 1024;
		di_device_->SetProperty(DIPROP_BUFFERSIZE, &prop.diph);

		reacquire_ = true;
		SetActive(true);
	}
}

void Win32InputDevice::Release() {
	di_device_->Unacquire();
	SetActive(false);
}

void Win32InputDevice::PollEvents() {
	if (IsActive() == true) {
		if (reacquire_) {
			if (di_device_->Acquire() != DI_OK) {
				// system won't let us in yet. Keep trying.
				return;
			}
			log_debug(GetIdentifier()+": acquired input device.");
		}


		HRESULT hr = di_device_->Poll();
		if (hr == DIERR_INPUTLOST) {
			reacquire_ = true;
			log_debug(GetIdentifier()+": lost input device.");
			return;
		} else if (hr != DI_OK && hr != DI_NOEFFECT) {
			reacquire_ = true;
			log_.Warningf((GetIdentifier() + ": Failed reaquiring device. Error=0x%8.8X.").c_str(), hr);
			return;
		}

		bool more = true;
		while (more) {
			DWORD in_out = (DWORD)element_array_.size();
			hr = di_device_->GetDeviceData(sizeof(device_object_data_[0]), device_object_data_, &in_out, 0);
			if (hr != DI_OK) {
				return;
			}
			if (in_out == 0) {
				break;
			}

			for (unsigned i = 0; i < in_out; i++) {
				// The following is a hack. I don't know if it works as
				// intended on non-Swedish keyboards. The issue is
				// that when pressing the right Alt key (Alt Gr), we will
				// receive one Ctrl-event, and then one "Right Alt"-event
				// at the same time (on Swedish keyboards at least).
				if (i + 1 < in_out &&
					device_object_data_[i].dwTimeStamp ==
					device_object_data_[i + 1].dwTimeStamp &&
					device_object_data_[i + 1].dwOfs == 400) {	// Right Alt at offset 400.
					i++;
				}

				int element_index = device_object_data_[i].dwOfs / sizeof(unsigned);
				InputElement* _element = element_array_[element_index];

				if (_element->GetType() == InputElement::kAnalogue) {
					const int value = device_object_data_[i].dwData;
					_element->SetValue(value);
				} else {
					const int value = device_object_data_[i].dwData;
					_element->SetValue(((value&0x80) && !reacquire_)? 1.0f : 0.0f);	// If reacquiring we release all buttons.
				}
			}
		}

		reacquire_ = false;
	}
}



loginstance(kUiInput, Win32InputDevice);



Win32InputManager::Win32InputManager(Win32DisplayManager* display_manager):
	display_manager_(display_manager),
	direct_input_(0),
	enum_error_(false),
	initialized_(false),
	screen_width_(0),
	screen_height_(0),
	cursor_x_(0),
	cursor_y_(0),
	keyboard_(0),
	mouse_(0) {
	::ShowCursor(FALSE);	// Start by decreasing the mouse show counter to 0, so it can be hidden (-1) or shown again (0).

	Refresh();

	POINT point;
	::GetCursorPos(&point);
	SetMousePosition(WM_NCMOUSEMOVE, point.x, point.y);
	SetCursorVisible(false);

	::memset(&type_count_, 0, sizeof(type_count_));

	HRESULT hr;

	// Create the DirectInput object.
	hr = DirectInput8Create(Win32Core::GetAppInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(LPVOID*)&direct_input_, 0);

	if (hr != DI_OK) {
		display_manager_->ShowMessageBox("DirectX 8 not supported (dinput creation failure)!", "DirectInput error!");
		return;
	}

	// Enumerate all devices.
	hr = direct_input_->EnumDevices(DI8DEVCLASS_ALL, EnumDeviceCallback, this, DIEDFL_ALLDEVICES);

	// enum_error_ will be set if an error has occured.
	if (hr != DI_OK || enum_error_ == true) {
		display_manager_->ShowMessageBox("DirectInput failed enumerating your devices!", "DirectInput error!");
		return;
	}

	AddObserver();

	initialized_ = true;
}

Win32InputManager::~Win32InputManager() {
	SetCursorVisible(true);

	if (initialized_ == true) {
		direct_input_->Release();
	}

	RemoveObserver();

	display_manager_ = 0;
}

void Win32InputManager::Refresh() {
	if (display_manager_ != 0 && display_manager_->GetHWND() != 0) {
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
	}
}

Win32DisplayManager* Win32InputManager::GetDisplayManager() const {
	return (display_manager_);
}

bool Win32InputManager::OnMessage(int msg, int wparam, long lparam) {
	bool consumed = false;
	switch (msg) {
		case WM_CHAR: {
			consumed = NotifyOnChar((wchar_t)wparam);
		} break;
		case WM_LBUTTONDBLCLK: {
			consumed = NotifyMouseDoubleClick();
		} break;
		case WM_MOUSEMOVE:
		case WM_NCMOUSEMOVE: {
			int x = GET_X_LPARAM(lparam);
			int y = GET_Y_LPARAM(lparam);
			SetMousePosition(msg, x, y);
		} break;
		case WM_KEYUP: {
			SetKey((KeyCode)wparam, lparam, false);
			consumed = NotifyOnKeyUp((KeyCode)wparam);
		} break;
		case WM_KEYDOWN: {
			SetKey((KeyCode)wparam, lparam, true);
			consumed = NotifyOnKeyDown((KeyCode)wparam);
		} break;
		case WM_SYSKEYUP: {
			SetKey((KeyCode)wparam, lparam, false);
			consumed = NotifyOnKeyUp((KeyCode)wparam);
		} break;
		case WM_SYSKEYDOWN: {
			SetKey((KeyCode)wparam, lparam, true);
			consumed = NotifyOnKeyDown((KeyCode)wparam);
		} break;
		case WM_ACTIVATE: {
			if (!is_cursor_visible_) {
				if (wparam == WA_INACTIVE) {
					SetCursorVisible(true);
					is_cursor_visible_ = false;
				} else {
					is_cursor_visible_ = true;
					SetCursorVisible(false);
				}
			}
		} break;
		case WM_SETFOCUS: {
			if (!is_cursor_visible_) {
				is_cursor_visible_ = true;
				SetCursorVisible(false);
			}
		} break;
		case WM_KILLFOCUS: {
			if (!is_cursor_visible_) {
				SetCursorVisible(true);
				is_cursor_visible_ = false;
			}
		} break;
	}
	return (consumed);
}

void Win32InputManager::SetKey(KeyCode w_param, long l_param, bool is_down) {
	if (l_param&0x1000000) {	// Extended key = right Alt, Ctrl...
		switch (w_param) {
			case kInKbdLCtrl:		w_param = kInKbdRCtrl;	break;
			case kInKbdLAlt:		w_param = kInKbdRAlt;	break;
		}
	} else if (w_param == kInKbdLShift && (l_param&0xFF0000) == 0x360000) {
		w_param = kInKbdRShift;
	}
	Parent::SetKey(w_param, is_down);
}

BOOL CALLBACK Win32InputManager::EnumDeviceCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID ref) {
	Win32InputManager* input_manager = (Win32InputManager*)ref;

	HRESULT hr;
	LPDIRECTINPUTDEVICE8 _di_device;

	hr = input_manager->direct_input_->CreateDevice(lpddi->guidInstance, &_di_device, 0);

	if (hr != DI_OK) {
		input_manager->enum_error_ = true;
		return DIENUM_STOP;
	}

	Win32InputDevice* device = new Win32InputDevice(_di_device, lpddi, input_manager);
	InputDevice::Interpretation _interpretation = InputDevice::kTypeOther;
	switch (lpddi->dwDevType & 0xFF) {
		case DI8DEVTYPE_MOUSE:		_interpretation = InputDevice::kTypeMouse;	break;
		case DI8DEVTYPE_KEYBOARD:	_interpretation = InputDevice::kTypeKeyboard;	break;
		case DI8DEVTYPE_JOYSTICK:	_interpretation = InputDevice::kTypeJoystick;	break;
		case DI8DEVTYPE_GAMEPAD:	_interpretation = InputDevice::kTypeGamepad;	break;
		case DI8DEVTYPE_1STPERSON:	_interpretation = InputDevice::kType1Stperson;	break;
		case DI8DEVTYPE_DRIVING: {
			switch ((lpddi->dwDevType>>8) & 0xFF) {
				case DI8DEVTYPEDRIVING_COMBINEDPEDALS:
				case DI8DEVTYPEDRIVING_DUALPEDALS:
				case DI8DEVTYPEDRIVING_THREEPEDALS:	_interpretation = InputDevice::kTypePedals;	break;
				default:				_interpretation = InputDevice::kTypeWheel;	break;
			}
		} break;
		case DI8DEVTYPE_FLIGHT: {
			switch ((lpddi->dwDevType>>8) & 0xFF) {
				case DI8DEVTYPEFLIGHT_RC:	_interpretation = InputDevice::kTypeGamepad;	break;
				case DI8DEVTYPEFLIGHT_STICK:	_interpretation = InputDevice::kTypeJoystick;	break;
				default:			_interpretation = InputDevice::kTypeFlight;	break;
			}
		} break;
		case DI8DEVTYPE_SUPPLEMENTAL: {
			switch ((lpddi->dwDevType>>8) & 0xFF) {
				case DI8DEVTYPESUPPLEMENTAL_COMBINEDPEDALS:
				case DI8DEVTYPESUPPLEMENTAL_DUALPEDALS:
				case DI8DEVTYPESUPPLEMENTAL_RUDDERPEDALS:
				case DI8DEVTYPESUPPLEMENTAL_THREEPEDALS:	_interpretation = InputDevice::kTypePedals;	break;
			}
		} break;
	}
	device->SetInterpretation(_interpretation, input_manager->type_count_[_interpretation]);
	++input_manager->type_count_[_interpretation];
	if (_interpretation == InputDevice::kTypeMouse && input_manager->mouse_ == 0) {
		input_manager->mouse_ = device;
	} else if (_interpretation == InputDevice::kTypeKeyboard && input_manager->keyboard_ == 0) {
		input_manager->keyboard_ = device;
	}
	input_manager->device_list_.push_back(device);

	return (DIENUM_CONTINUE);
}

void Win32InputManager::SetCursorVisible(bool visible) {
	deb_assert(Thread::GetCurrentThread()->GetThreadName() == "MainThread");

	if (!visible && GetDisplayManager()) {
		RECT rect;
		::GetClientRect(GetDisplayManager()->GetHWND(), &rect);
		::ClientToScreen(GetDisplayManager()->GetHWND(), (LPPOINT)&rect.left);
		::ClientToScreen(GetDisplayManager()->GetHWND(), (LPPOINT)&rect.right);
		::ClipCursor(&rect);
	} else {
		::ClipCursor(0);
	}
	if (is_cursor_visible_ != visible) {
		if (visible) {
			log_volatile(log_.Debug("Showing cursor."));
			for(int x = 0; x < 10 && ::ShowCursor(visible) < 1; ++x)
				;
		} else {
			log_volatile(log_.Debug("Hiding cursor."));
			for(int x = 0; x < 10 && ::ShowCursor(visible) > -2; ++x)
				;
		}
		is_cursor_visible_ = visible;
	}
}

float Win32InputManager::GetCursorX() {
	return cursor_x_;
}

float Win32InputManager::GetCursorY() {
	return cursor_y_;
}

void Win32InputManager::SetMousePosition(int x, int y) {
	Parent::SetMousePosition(x, y);
	cursor_x_ = 2.0f * x / screen_width_  - 1.0f;
	cursor_y_ = 2.0f * y / screen_height_ - 1.0f;
}

const InputDevice* Win32InputManager::GetKeyboard() const {
	return keyboard_;
}

InputDevice* Win32InputManager::GetKeyboard() {
	return keyboard_;
}

const InputDevice* Win32InputManager::GetMouse() const {
	return mouse_;
}

InputDevice* Win32InputManager::GetMouse() {
	return mouse_;
}

void Win32InputManager::AddObserver() {
	if (display_manager_) {
		// Listen to text input and standard mouse events.
		display_manager_->AddObserver(WM_CHAR, this);
		display_manager_->AddObserver(WM_SYSKEYDOWN, this);
		display_manager_->AddObserver(WM_SYSKEYUP, this);
		display_manager_->AddObserver(WM_KEYDOWN, this);
		display_manager_->AddObserver(WM_KEYUP, this);
		display_manager_->AddObserver(WM_MOUSEMOVE, this);
		display_manager_->AddObserver(WM_NCMOUSEMOVE, this);
		display_manager_->AddObserver(WM_LBUTTONDBLCLK, this);
		display_manager_->AddObserver(WM_LBUTTONDOWN, this);
		display_manager_->AddObserver(WM_RBUTTONDOWN, this);
		display_manager_->AddObserver(WM_MBUTTONDOWN, this);
		display_manager_->AddObserver(WM_LBUTTONUP, this);
		display_manager_->AddObserver(WM_RBUTTONUP, this);
		display_manager_->AddObserver(WM_MBUTTONUP, this);
		display_manager_->AddObserver(WM_ACTIVATE, this);
		display_manager_->AddObserver(WM_SETFOCUS, this);
		display_manager_->AddObserver(WM_KILLFOCUS, this);
	}
}

void Win32InputManager::RemoveObserver() {
	if (display_manager_) {
		display_manager_->RemoveObserver(this);
	}
}

void Win32InputManager::SetMousePosition(int msg, int x, int y) {
	if (msg == WM_NCMOUSEMOVE && display_manager_) {
		POINT point;
		point.x = x;
		point.y = y;
		::ScreenToClient(display_manager_->GetHWND(), &point);
		x = point.x;
		y = point.y;
	}
	SetMousePosition(x, y);
}

bool Win32InputManager::IsInitialized() {
	return initialized_;
}



loginstance(kUiInput, Win32InputManager);



}
