
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/unordered.h"
#include <list>
#include <vector>
#include "../../lepra/include/lepratypes.h"
#include "../../lepra/include/string.h"
#include "../../lepra/include/hashtable.h"
#include "../include/uilepra.h"



namespace uilepra {



class DisplayManager;
class InputDevice;
class InputElement;
class InputManager;



class TextInputObserver {
public:
	virtual bool OnChar(wchar_t c) = 0;
};

class KeyCodeInputObserver;

class MouseInputObserver {
public:
	virtual bool OnDoubleClick() = 0;
};



class InputFunctor {
public:
	virtual void Call(InputElement* element) = 0;
	virtual InputFunctor* CreateCopy() const = 0;
};

template<class _TClass> class TInputFunctor : public InputFunctor {
public:
	TInputFunctor(_TClass* object, void (_TClass::*func)(InputElement* element)):
		object_(object),
		mFunc(func) {
	}

	virtual void Call(InputElement* element) {
		(object_->*mFunc)(element);
	}

	InputFunctor* CreateCopy() const {
		return new TInputFunctor(object_, mFunc);
	}

private:
	_TClass* object_;
	void (_TClass::*mFunc)(InputElement* element);
};



class InputElement {
public:
	typedef std::vector<InputFunctor*> FunctorArray;

	friend class InputDevice;

	enum Type {
		kUnknown = 0,
		kAnalogue,
		kDigital,
	};

	enum Interpretation {
		kAbsoluteAxis,
		kRelativeAxis,
		kButton,
	};

	InputElement(Type type, Interpretation interpretation, int type_index, InputDevice* parent_device);
	virtual ~InputElement();

	Type GetType() const;

	Interpretation GetInterpretation() const;
	int GetTypeIndex() const;

	InputDevice* GetParentDevice() const;

	// If digital:
	//    Returns true if "pressed", false otherwise.
	// If analogue:
	//    Returns true if the analogue value is greater than threshold.
	bool GetBooleanValue(float threshold = 0.5) const;

	float GetValue() const;
	void SetValue(float new_value);
	// (Only useful with analogue elements).
	void SetValue(int value);
	void AddValue(int value);

	void SetIdentifier(const str& identifier);
	const str& GetIdentifier() const;
	str GetFullName() const;
	str GetName() const;

	// Sets the input listener functor. The InputElement will be responsible
	// of deleting it.
	void AddFunctor(InputFunctor* functor);
	void ClearFunctors();
	const FunctorArray& GetFunctorArray() const;
	void TriggerFunctors();

	str GetCalibration() const;
	bool SetCalibration(const str& data);

private:
	float value_;
	int min_;
	int max_;

	Type type_;
	Interpretation interpretation_;
	int type_index_;
	InputDevice* parent_device_;

	str identifier_;

	FunctorArray functor_array_;

	logclass();
};

#define ADD_INPUT_CALLBACK(_e, _func, _class) \
	(_e)->AddFunctor(new uilepra::TInputFunctor<_class>(this, &_class::_func));



class InputDevice {
public:
	typedef std::vector<InputElement*> ElementArray;

	enum Interpretation {
		kTypeMouse = 0,
		kTypeKeyboard,
		kTypeJoystick,
		kTypeGamepad,
		kType1Stperson,
		kTypePedals,
		kTypeWheel,
		kTypeFlight,
		kTypeOther,
		kTypeCount
	};

	typedef std::pair<str, str> CalibrationElement;
	typedef std::vector<CalibrationElement> CalibrationData;

	InputDevice(InputManager* manager);
	virtual ~InputDevice();
	virtual bool IsOwnedByManager() const;

	Interpretation GetInterpretation() const;
	int GetTypeIndex() const;
	void SetInterpretation(Interpretation interpretation, int type_index);

	InputManager* GetManager() const;

	// Check and set if device is active.
	bool IsActive();

	// Since only one window (or application) should be in focus of
	// the input, the display manager is required.
	virtual void Activate() = 0;
	virtual void Release() = 0;

	virtual void PollEvents() = 0;

	const ElementArray& GetElements() const;

	// Returns the first input element identified by identifier.
	// Returns NULL if no element was found.
	const InputElement* GetElement(const str& identifier) const;
	InputElement* GetElement(const str& identifier);
	InputElement* GetElement(unsigned element_index) const;
	unsigned GetElementIndex(InputElement* element) const;

	InputElement* GetButton(unsigned button_index) const;
	InputElement* GetAxis(unsigned axis_index) const;

	unsigned GetNumElements() const;
	unsigned GetNumDigitalElements() const;
	unsigned GetNumAnalogueElements() const;
	void CountElements() const;

	void SetIdentifier(const str& identifier);
	void SetUniqueIdentifier(const str& identifier);
	const str& GetIdentifier() const;
	const str& GetUniqueIdentifier() const;

	// Sets an observer on the entire device. (All elements).
	// The device takes care of deleting the functor.
	void AddFunctor(InputFunctor* functor);
	void ClearFunctors();

	CalibrationData GetCalibration() const;
	bool SetCalibration(const CalibrationData& data);

protected:

	void SetActive(bool active);

	ElementArray element_array_;

private:

	InputManager* manager_;
	Interpretation interpretation_;
	int type_index_;
	mutable int num_digital_elements_;
	mutable int num_analogue_elements_;

	bool active_;

	str identifier_;
	str unique_identifier_;

	logclass();
};



/*
	class InputManager

	This is the main class of the input system. It provides you
	with a list of all input devices (call GetDeviceList()), and
	some default mouse functionality.
*/

class InputManager {
public:

	typedef std::list<InputDevice*> DeviceList;

	enum KeyCode {
		kInKbdBackspace		= 8,
		kInKbdTab		= 9,

		kInKbdCenter		= 12,
		kInKbdEnter		= 13,

		kInKbdLShift		= 16,
		kInKbdLCtrl		= 17,
		kInKbdLAlt		= 18,

		kInKbdPause		= 19,
		kInKbdCapsLock		= 20,

		kInKbdEsc		= 27,

		kInKbdSpace		= 32,
		kInKbdPgUp		= 33,
		kInKbdPgDown		= 34,
		kInKbdEnd		= 35,
		kInKbdHome		= 36,
		kInKbdLeft		= 37,
		kInKbdUp		= 38,
		kInKbdRight		= 39,
		kInKbdDown		= 40,

		kInKbdPrintScreen	= 44,
		kInKbdInsert		= 45,
		kInKbdDel		= 46,

		kInKbd0			= 48,
		kInKbd1			= 49,
		kInKbd2			= 50,
		kInKbd3			= 51,
		kInKbd4			= 52,
		kInKbd5			= 53,
		kInKbd6			= 54,
		kInKbd7			= 55,
		kInKbd8			= 56,
		kInKbd9			= 57,

		kInKbdA			= 65,
		kInKbdB			= 66,
		kInKbdC			= 67,
		kInKbdD			= 68,
		kInKbdE			= 69,
		kInKbdF			= 70,
		kInKbdG			= 71,
		kInKbdH			= 72,
		kInKbdI			= 73,
		kInKbdJ			= 74,
		kInKbdK			= 75,
		kInKbdL			= 76,
		kInKbdM			= 77,
		kInKbdN			= 78,
		kInKbdO			= 79,
		kInKbdP			= 80,
		kInKbdQ			= 81,
		kInKbdR			= 82,
		kInKbdS			= 83,
		kInKbdT			= 84,
		kInKbdU			= 85,
		kInKbdV			= 86,
		kInKbdW			= 87,
		kInKbdX			= 88,
		kInKbdY			= 89,
		kInKbdZ			= 90,
		kInKbdLOs		= 91,
		kInKbdROs		= 92,

		kInKbdContextMenu	= 93,	// The "windows menu" button on Microsoft-compatible keyboards.

		kInKbdNumpad0		= 96,
		kInKbdNumpad1		= 97,
		kInKbdNumpad2		= 98,
		kInKbdNumpad3		= 99,
		kInKbdNumpad4		= 100,
		kInKbdNumpad5		= 101,
		kInKbdNumpad6		= 102,
		kInKbdNumpad7		= 103,
		kInKbdNumpad8		= 104,
		kInKbdNumpad9		= 105,
		kInKbdNumpadMul		= 106,
		kInKbdNumpadPlus	= 107,
		kInKbdNumpadMinus	= 109,
		kInKbdNumpadDot		= 110,
		kInKbdNumpadDiv		= 111,
		kInKbdF1		= 112,
		kInKbdF2		= 113,
		kInKbdF3		= 114,
		kInKbdF4		= 115,
		kInKbdF5		= 116,
		kInKbdF6		= 117,
		kInKbdF7		= 118,
		kInKbdF8		= 119,
		kInKbdF9		= 120,
		kInKbdF10		= 121,
		kInKbdF11		= 122,
		kInKbdF12		= 123,
		kInKbdNumLock		= 144,
		kInKbdScrollLock	= 145,

		kInKbdQuickBack		= 166,
		kInKbdQuickForward	= 167,
		kInKbdQuickRefresh	= 168,
		kInKbdQuickStop		= 169,
		kInKbdQuickSearch	= 170,
		kInKbdQuickFavorites	= 171,
		kInKbdQuickWebHome	= 172,
		kInKbdQuickSoundMute	= 173,
		kInKbdQuickDecrVolume	= 174,
		kInKbdQuickIncrVolume	= 175,
		kInKbdQuickNavRight	= 176,
		kInKbdQuickNavLeft	= 177,
		kInKbdQuickNavStop	= 178,
		kInKbdQuickNavPlaypause	= 179,
		kInKbdQuickMail		= 180,
		kInKbdQuickMedia	= 181,
		kInKbdQuickMyComputer	= 182,
		kInKbdQuickCalculator	= 183,

		kInKbdDiaeresis		= 186,
		kInKbdPlus		= 187,
		kInKbdComma		= 188,
		kInKbdMinus		= 189,
		kInKbdDot		= 190,
		kInKbdApostrophe	= 191,

		kInKbdOe		= 192,

		kInKbdAcute		= 219,
		kInKbdParagraph		= 220,
		kInKbdAa		= 221,
		kInKbdAe		= 222,

		kInKbdCompare		= 226,

		kInKbdRShift		= 227,	// Not defined by Win32.
		kInKbdRCtrl		= 228,	// Not defined by Win32.
		kInKbdRAlt		= 229,	// Not defined by Win32.
	};

	static InputManager* CreateInputManager(DisplayManager* display_manager);

	// Returns a singleton input manager (created if needed).
	// This function is implemented elsewhere (not in Input.cpp),
	// depending on the platform dependent implementation.
	// Returns a null pointer if something went wrong in the creation.
	//static InputManager* GetInputManager();

	// A call to this function must be issued when the application
	// exits, IFF GetInputManager() was previously called.
	//static void ReleaseInputManager();

	virtual ~InputManager();

	virtual void Refresh() = 0;

	// Returns a list of all available input devices.
	const DeviceList& GetDeviceList() const;

	// Returns the keyboard device, if there is one... NULL otherwise.
	virtual const InputDevice* GetKeyboard() const = 0;
	virtual InputDevice* GetKeyboard() = 0;

	// Returns the mouse device, if there is one... NULL otherwise.
	virtual const InputDevice* GetMouse() const = 0;
	virtual InputDevice* GetMouse() = 0;

	// Searches for device number n, with identifier device_identifier.
	InputDevice* FindDevice(const str& device_identifier, int n = 0);

	// Returns the number of devices with the same identifier.
	int QueryIdentifierCount(const str& device_identifier) const;

	// Returns which index the device has in the list of devices with the
	// same identifier. Thus, if there are two gamepads, and we pass one
	// of them as the parameter, the function will return either 0 or 1,
	// depending on if this is the first or second gamepad. This index can
	// be used with FindDevice() later on.
	unsigned GetDeviceIndex(InputDevice* device) const;

	void AddTextInputObserver(TextInputObserver* listener);
	void RemoveTextInputObserver(TextInputObserver* listener);

	void AddKeyCodeInputObserver(KeyCodeInputObserver* listener);
	void RemoveKeyCodeInputObserver(KeyCodeInputObserver* listener);

	void AddMouseInputObserver(MouseInputObserver* listener);
	void RemoveMouseInputObserver(MouseInputObserver* listener);

	virtual void PreProcessEvents();
	virtual void PollEvents();

	// Sets one observer on all input devices, and all elements.
	// The input manager takes care of deleting the functor.
	void AddFunctor(InputFunctor* functor);
	void ClearFunctors();

	void AddInputDevice(InputDevice* device);
	void RemoveInputDevice(InputDevice* device);

	// Activase/release all devices.
	void ActivateAll();
	void ReleaseAll();

	virtual void ActivateKeyboard();
	virtual void ReleaseKeyboard();

	/*
		Default mouse management.
	*/

	// Show/hide system default cursor.
	virtual void SetCursorVisible(bool visible) = 0;
	bool IsCursorVisible() const;

	// Range: [-1, 1] (Left and right, up and down)
	virtual float GetCursorX() = 0;
	virtual float GetCursorY() = 0;
	void GetMousePosition(int& x, int& y) const;
	virtual void SetMousePosition(int x, int y);

	bool NotifyOnChar(wchar_t c);
	bool NotifyOnKeyDown(KeyCode key_code);
	bool NotifyOnKeyUp(KeyCode key_code);
	bool NotifyMouseDoubleClick();

	bool ReadKey(KeyCode key_code);

	static str GetKeyName(KeyCode key_code);

protected:

	DeviceList device_list_;

	InputManager();

	void SetKey(KeyCode key_code, bool value);

	typedef std::unordered_set<TextInputObserver*, LEPRA_VOIDP_HASHER> TextObserverList;
	typedef std::unordered_set<KeyCodeInputObserver*, LEPRA_VOIDP_HASHER> KeyCodeObserverList;
	typedef std::unordered_set<MouseInputObserver*, LEPRA_VOIDP_HASHER> MouseObserverList;
	TextObserverList text_observer_list_;
	KeyCodeObserverList key_code_observer_list_;
	MouseObserverList mouse_observer_list_;

	bool key_down_[256];
	int mouse_x_;
	int mouse_y_;
	bool is_cursor_visible_;

	logclass();
};



class KeyCodeInputObserver {
public:
	virtual bool OnKeyDown(InputManager::KeyCode key_code) = 0;
	virtual bool OnKeyUp(InputManager::KeyCode key_code) = 0;
};



}
