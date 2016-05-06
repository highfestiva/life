
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uiinput.h"
#include "../../lepra/include/math.h"
#include <math.h>



namespace uilepra {



InputElement::InputElement(Type type, Interpretation interpretation, int type_index, InputDevice* parent_device):
	value_(0),
	type_(type),
	interpretation_(interpretation),
	type_index_(type_index),
	parent_device_(parent_device),
	min_(+1),
	max_(-1) {
}

InputElement::~InputElement() {
	ClearFunctors();
}

InputElement::Type InputElement::GetType() const {
	return (type_);
}

InputElement::Interpretation InputElement::GetInterpretation() const {
	return (interpretation_);
}

int InputElement::GetTypeIndex() const {
	return (type_index_);
}

InputDevice* InputElement::GetParentDevice() const {
	return (parent_device_);
}

bool InputElement::GetBooleanValue(float threshold) const {
	return (value_ >= threshold);
}

float InputElement::GetValue() const {
	return value_;
}


void InputElement::SetValue(float new_value) {
	static const float input_epsilon = 0.02f;
	if (interpretation_ == kRelativeAxis || ::fabs(new_value - value_) > input_epsilon) {
		//::printf("%s(%i) = %f\n", GetIdentifier().c_str(), GetTypeIndex(), new_value);
		//mLog.Infof("%s(%i = %f\n"), GetIdentifier().c_str(), GetTypeIndex(), pNewValue);

		value_ = new_value;

		TriggerFunctors();
	}
}

void InputElement::SetValue(int value) {
	// Calibrate...
	if (value < min_) {
		min_ = value;
	}
	if (value > max_) {
		max_ = value;
	}

	// Center relative axis.
	if (GetInterpretation() == kRelativeAxis) {
		if (max_ > -min_) {
			min_ = -max_;
		}
		if (max_ < -min_) {
			max_ = -min_;
		}
	}

	if (GetType() == kDigital) {
		SetValue((float)value);
	} else if (min_ < max_) {
		// Scale to +-1.
		SetValue((value*2.0f-(max_+min_)) / (float)(max_-min_));
	}
}

void InputElement::AddValue(int value) {
	const int prev_value = (value_*(max_-min_) + (max_+min_)) / 2.0f;
	SetValue(prev_value + value);
}

const str& InputElement::GetIdentifier() const {
	return identifier_;
}

str InputElement::GetFullName() const {
	str id = GetIdentifier();
	id = strutil::ReplaceAll(id, ' ', '_');
	id = strutil::ReplaceAll(id, '-', '_');
	return GetParentDevice()->GetUniqueIdentifier() + "." + id;
}

str InputElement::GetName() const {
	str __name;
	switch (GetInterpretation()) {
		case InputElement::kAbsoluteAxis:	__name += "AbsoluteAxis";	break;
		case InputElement::kRelativeAxis:	__name += "RelativeAxis";	break;
		default:				__name += "Button";		break;
	}
	__name += strutil::IntToString(GetTypeIndex(), 10);
	return (__name);
}

void InputElement::SetIdentifier(const str& identifier) {
	identifier_ = identifier;
}

void InputElement::AddFunctor(InputFunctor* functor) {
	functor_array_.push_back(functor);
}

void InputElement::ClearFunctors() {
	for (FunctorArray::iterator x = functor_array_.begin(); x != functor_array_.end(); ++x) {
		delete (*x);
	}
	functor_array_.clear();
}

const InputElement::FunctorArray& InputElement::GetFunctorArray() const {
	return functor_array_;
}

void InputElement::TriggerFunctors() {
	for (FunctorArray::iterator x = functor_array_.begin(); x != functor_array_.end(); ++x) {
		(*x)->Call(this);
	}
}

str InputElement::GetCalibration() const {
	str _data;
	_data += strutil::IntToString(min_, 10);
	_data += ", ";
	_data += strutil::IntToString(max_, 10);
	return (_data);
}

bool InputElement::SetCalibration(const str& data) {
	bool ok = false;
	strutil::strvec _data = strutil::Split(data, ", ");
	if (_data.size() >= 2) {
		ok = true;
		ok &= strutil::StringToInt(_data[0], min_, 10);
		ok &= strutil::StringToInt(_data[1], max_, 10);
	}
	return (ok);
}

loginstance(kUiInput, InputElement);



// ---------------------------------------------------------------------------



InputDevice::InputDevice(InputManager* manager):
	manager_(manager),
	interpretation_(kTypeOther),
	type_index_(-1),
	num_digital_elements_(-1),
	num_analogue_elements_(-1),
	active_(false) {
}

InputDevice::~InputDevice() {
	ElementArray::const_iterator x;
	for (x = element_array_.begin(); x != element_array_.end(); ++x) {
		InputElement* _element = *x;
		delete _element;
	}
	element_array_.clear();
}

bool InputDevice::IsOwnedByManager() const {
	return true;
}

InputDevice::Interpretation InputDevice::GetInterpretation() const {
	return (interpretation_);
}

int InputDevice::GetTypeIndex() const {
	return (type_index_);
}

void InputDevice::SetInterpretation(Interpretation interpretation, int type_index) {
	interpretation_ = interpretation;
	type_index_ = type_index;
}

InputManager* InputDevice::GetManager() const {
	return (manager_);
}

bool InputDevice::IsActive() {
	return active_;
}

void InputDevice::SetActive(bool active) {
	active_ = active;
}

const InputDevice::ElementArray& InputDevice::GetElements() const {
	return element_array_;
}

InputElement* InputDevice::GetElement(unsigned element_index) const {
	return element_array_[element_index];
}

unsigned InputDevice::GetNumElements() const {
	return ((unsigned)element_array_.size());
}

unsigned InputDevice::GetNumDigitalElements() const {
	if (num_digital_elements_ == -1) {
		CountElements();
	}
	return num_digital_elements_;
}

unsigned InputDevice::GetNumAnalogueElements() const {
	if (num_analogue_elements_ == -1) {
		CountElements();
	}
	return num_analogue_elements_;
}

void InputDevice::CountElements() const {
	num_digital_elements_  = 0;
	num_analogue_elements_ = 0;

	ElementArray::const_iterator x;
	for (x = element_array_.begin(); x != element_array_.end(); ++x) {
		InputElement* _element = *x;

		if (_element->GetType() == InputElement::kDigital) {
			num_digital_elements_++;
		} else if(_element->GetType() == InputElement::kAnalogue) {
			num_analogue_elements_++;
		}
	}
}

void InputDevice::SetIdentifier(const str& identifier) {
	identifier_ = identifier;
	unique_identifier_ = identifier_ + strutil::IntToString(GetManager()->QueryIdentifierCount(identifier_), 10);
	unique_identifier_ = strutil::ReplaceAll(unique_identifier_, ' ', '_');
	unique_identifier_ = strutil::ReplaceAll(unique_identifier_, '-', '_');
}

void InputDevice::SetUniqueIdentifier(const str& identifier) {
	unique_identifier_ = identifier;
}

const str& InputDevice::GetIdentifier() const {
	return identifier_;
}

const str& InputDevice::GetUniqueIdentifier() const {
	return unique_identifier_;
}

const InputElement* InputDevice::GetElement(const str& identifier) const {
	ElementArray::const_iterator x;
	for (x = element_array_.begin(); x != element_array_.end(); ++x) {
		InputElement* _element = *x;
		if (_element->GetIdentifier() == identifier) {
			return _element;
		}
	}

	return 0;
}


InputElement* InputDevice::GetElement(const str& identifier) {
	ElementArray::iterator x;

	for (x = element_array_.begin();
		x != element_array_.end();
		++x) {
		InputElement* _element = *x;
		if (_element->GetIdentifier() == identifier) {
			return _element;
		}
	}

	return 0;
}

unsigned InputDevice::GetElementIndex(InputElement* element) const {
	ElementArray::const_iterator x;

	int i = 0;
	for (x = element_array_.begin();
		x != element_array_.end();
		++x) {
		InputElement* _element = *x;
		if (_element == element) {
			return i;
		}
		i++;
	}

	return ((unsigned)-1);
}

InputElement* InputDevice::GetButton(unsigned button_index) const {
	ElementArray::const_iterator x;

	unsigned current_button = 0;
	for (x = element_array_.begin();
		x != element_array_.end();
		++x) {
		InputElement* _element = *x;
		if (_element->GetType() == InputElement::kDigital) {
			if (current_button == button_index) {
				return _element;
			}
			++current_button;
		}
	}

	return 0;
}

InputElement* InputDevice::GetAxis(unsigned axis_index) const {
	ElementArray::const_iterator x;

	unsigned current_axis = 0;
	for (x = element_array_.begin();
		x != element_array_.end();
		++x) {
		InputElement* _element = *x;
		if (_element->GetType() == InputElement::kAnalogue) {
			if (current_axis == axis_index) {
				return _element;
			}
			++current_axis;
		}
	}

	return 0;
}



void InputDevice::AddFunctor(InputFunctor* functor) {
	ElementArray::iterator x;
	for (x = element_array_.begin(); x != element_array_.end(); ++x) {
		InputElement* _element = *x;
		_element->AddFunctor(functor->CreateCopy());
	}
	delete (functor);
}

void InputDevice::ClearFunctors() {
	ElementArray::iterator x;
	for (x = element_array_.begin(); x != element_array_.end(); ++x) {
		InputElement* _element = *x;
		_element->ClearFunctors();
	}
}

InputDevice::CalibrationData InputDevice::GetCalibration() const {
	CalibrationData _data;
	ElementArray::const_iterator x = element_array_.begin();
	for (; x != element_array_.end(); ++x) {
		InputElement* _element = *x;
		if (_element->GetInterpretation() == InputElement::kAbsoluteAxis) {
			_data.push_back(CalibrationElement(_element->GetName(), _element->GetCalibration()));
		}
	}
	return (_data);
}

bool InputDevice::SetCalibration(const CalibrationData& data) {
	bool ok = true;
	CalibrationData::const_iterator y = data.begin();
	for (; y != data.end(); ++y) {
		const CalibrationElement& calibration = *y;
		ElementArray::iterator x = element_array_.begin();
		for (; x != element_array_.end(); ++x) {
			InputElement* _element = *x;
			if (_element->GetName() == calibration.first) {
				ok &= _element->SetCalibration(calibration.second);
			}
		}
	}
	return (ok);
}

loginstance(kUiInput, InputDevice);



// ---------------------------------------------------------------------------



InputManager::InputManager() {
	::memset(key_down_, 0, sizeof(key_down_));
	mouse_x_ = 0;
	mouse_y_ = 0;
	is_cursor_visible_ = true;
}

InputManager::~InputManager() {
	DeviceList::iterator x;
	for (x = device_list_.begin(); x != device_list_.end(); ++x) {
		InputDevice* _device = *x;
		if (_device->IsOwnedByManager()) {
			delete _device;
		}
	}
	device_list_.clear();
}

const InputManager::DeviceList& InputManager::GetDeviceList() const {
	return (device_list_);
}

void InputManager::AddTextInputObserver(TextInputObserver* listener) {
	text_observer_list_.insert(listener);
}

void InputManager::RemoveTextInputObserver(TextInputObserver* listener) {
	text_observer_list_.erase(listener);
}

void InputManager::AddKeyCodeInputObserver(KeyCodeInputObserver* listener) {
	key_code_observer_list_.insert(listener);
}

void InputManager::RemoveKeyCodeInputObserver(KeyCodeInputObserver* listener) {
	key_code_observer_list_.erase(listener);
}

void InputManager::AddMouseInputObserver(MouseInputObserver* listener) {
	mouse_observer_list_.insert(listener);
}

void InputManager::RemoveMouseInputObserver(MouseInputObserver* listener) {
	mouse_observer_list_.erase(listener);
}

void InputManager::GetMousePosition(int& x, int& y) const {
	x = mouse_x_;
	y = mouse_y_;
}

void InputManager::SetMousePosition(int x, int y) {
	mouse_x_ = x;
	mouse_y_ = y;
}

bool InputManager::NotifyOnChar(wchar_t c) {
	bool consumed = false;
	TextObserverList::iterator x = text_observer_list_.begin();
	for (; !consumed && x != text_observer_list_.end(); ++x) {
		consumed = (*x)->OnChar(c);
	}
	return (consumed);
}

bool InputManager::NotifyOnKeyDown(KeyCode key_code) {
	bool consumed = false;
	KeyCodeObserverList::iterator x = key_code_observer_list_.begin();
	for (; !consumed && x != key_code_observer_list_.end(); ++x) {
		consumed = (*x)->OnKeyDown(key_code);
	}
	return (consumed);
}

bool InputManager::NotifyOnKeyUp(KeyCode key_code) {
	bool consumed = false;
	KeyCodeObserverList::iterator x = key_code_observer_list_.begin();
	for (; !consumed && x != key_code_observer_list_.end(); ++x) {
		consumed = (*x)->OnKeyUp(key_code);
	}
	return (consumed);
}

bool InputManager::NotifyMouseDoubleClick() {
	bool consumed = false;
	MouseObserverList::iterator x = mouse_observer_list_.begin();
	for (; !consumed && x != mouse_observer_list_.end(); ++x) {
		consumed = (*x)->OnDoubleClick();
	}
	return (consumed);
}

bool InputManager::ReadKey(KeyCode key_code) {
	return (key_down_[(int)key_code]);
}

str InputManager::GetKeyName(KeyCode key_code) {
	const char* key_name = 0;
#define X(name)	case kInKbd##name:	key_name = #name;	break
	switch (key_code) {
		X(Backspace);
		X(Tab);
		X(Enter);
		X(LShift);
		X(LCtrl);
		X(LAlt);
		X(Pause);
		X(CapsLock);
		X(Esc);
		X(Space);
		X(PgUp);
		X(PgDown);
		X(End);
		X(Home);
		X(Left);
		X(Up);
		X(Right);
		X(Down);
		X(PrintScreen);
		X(Insert);
		X(Del);
		X(LOs);
		X(ROs);
		X(ContextMenu);
		X(Numpad0);
		X(Numpad1);
		X(Numpad2);
		X(Numpad3);
		X(Numpad4);
		X(Numpad5);
		X(Numpad6);
		X(Numpad7);
		X(Numpad8);
		X(Numpad9);
		X(NumpadMul);
		X(NumpadPlus);
		X(NumpadMinus);
		X(NumpadDot);
		X(NumpadDiv);
		X(F1);
		X(F2);
		X(F3);
		X(F4);
		X(F5);
		X(F6);
		X(F7);
		X(F8);
		X(F9);
		X(F10);
		X(F11);
		X(F12);
		X(NumLock);
		X(ScrollLock);
		X(QuickBack);
		X(QuickForward);
		X(QuickRefresh);
		X(QuickStop);
		X(QuickSearch);
		X(QuickFavorites);
		X(QuickWebHome);
		X(QuickSoundMute);
		X(QuickDecrVolume);
		X(QuickIncrVolume);
		X(QuickNavRight);
		X(QuickNavLeft);
		X(QuickNavStop);
		X(QuickNavPlaypause);
		X(QuickMail);
		X(QuickMedia);
		X(QuickMyComputer);
		X(QuickCalculator);
		X(Diaeresis);
		X(Plus);
		X(Comma);
		X(Minus);
		X(Dot);
		X(Apostrophe);
		X(Acute);
		X(Paragraph);
		X(Compare);
		X(RShift);
		X(RCtrl);
		X(RAlt);
		default: break;
	}
#undef X
	if (key_name) {
		return (key_name);
	}
	return (str(1, (char)key_code));
}

void InputManager::SetKey(KeyCode key_code, bool value) {
	key_down_[(unsigned char)key_code] = value;
}

void InputManager::PreProcessEvents() {
}

void InputManager::PollEvents() {
	DeviceList::iterator x;
	for (x = device_list_.begin();
		x != device_list_.end();
		++x) {
		InputDevice* _device = *x;
		_device->PollEvents();
	}
}

void InputManager::AddFunctor(InputFunctor* functor) {
	DeviceList::iterator x;
	for (x = device_list_.begin(); x != device_list_.end(); ++x) {
		InputDevice* _device = *x;
		_device->AddFunctor(functor->CreateCopy());
	}
	delete (functor);
}

void InputManager::ClearFunctors() {
	DeviceList::iterator x;
	for (x = device_list_.begin(); x != device_list_.end(); ++x) {
		InputDevice* _device = *x;
		_device->ClearFunctors();
	}
}

void InputManager::AddInputDevice(InputDevice* device) {
	// Clone the functors if any are present already.
	DeviceList::iterator x;
	for (x = device_list_.begin(); x != device_list_.end(); ++x) {
		const InputDevice* _device = *x;
		if (_device->GetNumElements() <= 0) {
			continue;
		}
		const InputElement* _element = _device->GetElement(0);
		if (_element) {
			InputElement::FunctorArray::const_iterator z;
			const InputElement::FunctorArray& functor_array = _element->GetFunctorArray();
			for (z = functor_array.begin(); z != functor_array.end(); ++z) {
				device->AddFunctor((*z)->CreateCopy());
			} break;
		}
	}

	device_list_.push_back(device);
}

void InputManager::RemoveInputDevice(InputDevice* device) {
	device_list_.remove(device);
}

void InputManager::ActivateAll() {
	DeviceList::iterator x;
	for (x = device_list_.begin(); x != device_list_.end(); ++x) {
		InputDevice* _device = *x;
		_device->Activate();
	}
}

void InputManager::ReleaseAll() {
	DeviceList::iterator x;
	for (x = device_list_.begin();
		x != device_list_.end();
		++x) {
		InputDevice* _device = *x;
		_device->Release();
	}
}

void InputManager::ActivateKeyboard() {
}

void InputManager::ReleaseKeyboard() {
}

bool InputManager::IsCursorVisible() const {
	return is_cursor_visible_;
}

InputDevice* InputManager::FindDevice(const str& device_identifier, int n) {
	int _n = 0;

	DeviceList::iterator x;
	for (x = device_list_.begin();
		x != device_list_.end();
		++x) {
		InputDevice* _device = *x;
		if (device_identifier == _device->GetIdentifier()) {
			if (_n == n) {
				return _device;
			} else {
				_n++;
			}
		}
	}

	return 0;
}

int InputManager::QueryIdentifierCount(const str& device_identifier) const {
	int count = 0;
	DeviceList::const_iterator x = device_list_.begin();
	for (; x != device_list_.end(); ++x) {
		InputDevice* _device = *x;
		if (device_identifier == _device->GetIdentifier()) {
			++count;
		}
	}

	return count;
}

unsigned InputManager::GetDeviceIndex(InputDevice* device) const {
	int count = 0;

	DeviceList::const_iterator x;
	for (x = device_list_.begin();
		x != device_list_.end();
		++x) {
		InputDevice* _device = *x;
		if (device->GetIdentifier() == _device->GetIdentifier()) {
			if (device == _device) {
				return count;
			}

			count++;
		}
	}

	return 0;
}

loginstance(kUiInput, InputManager);



}
