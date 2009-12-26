
// Author: Alexander Hugestrand, Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiInput.h"
#include <math.h>



namespace UiLepra
{



InputElement::InputElement(Type pType, Interpretation pInterpretation, int pTypeIndex, InputDevice* pParentDevice):
	mPrevValue(0),
	mValue(0),
	mType(pType),
	mInterpretation(pInterpretation),
	mTypeIndex(pTypeIndex),
	mParentDevice(pParentDevice)
{
}

InputElement::~InputElement()
{
	ClearFunctors();
}

InputElement::Type InputElement::GetType() const
{
	return (mType);
}

InputElement::Interpretation InputElement::GetInterpretation() const
{
	return (mInterpretation);
}

int InputElement::GetTypeIndex() const
{
	return (mTypeIndex);
}

InputDevice* InputElement::GetParentDevice() const
{
	return (mParentDevice);
}

bool InputElement::GetBooleanValue(float64 pThreshold) const
{
	return (mValue >= pThreshold);
}

float64 InputElement::GetValue() const
{
	return mValue;
}

float64 InputElement::GetDeltaValue() const
{
	return (mValue - mPrevValue);
}

const str& InputElement::GetIdentifier() const
{
	return mIdentifier;
}

str InputElement::GetFullName() const
{
	str lName;
	switch (GetParentDevice()->GetInterpretation())
	{
		case InputDevice::TYPE_MOUSE:		lName += _T("Mouse");		break;
		case InputDevice::TYPE_KEYBOARD:	lName += _T("Keyboard");	break;
		case InputDevice::TYPE_JOYSTICK:	lName += _T("Joystick");	break;
		case InputDevice::TYPE_GAMEPAD:		lName += _T("GamePad");		break;
		case InputDevice::TYPE_1STPERSON:	lName += _T("1stPerson");	break;
		case InputDevice::TYPE_PEDALS:		lName += _T("Pedals");		break;
		case InputDevice::TYPE_WHEEL:		lName += _T("Wheel");		break;
		case InputDevice::TYPE_FLIGHT:		lName += _T("Flight");		break;
		default:				lName += _T("Device");		break;
	}
	lName += strutil::IntToString(GetParentDevice()->GetTypeIndex(), 10)+_T(".");
	lName += GetName();
	return (lName);
}

str InputElement::GetName() const
{
	str lName;
	switch (GetInterpretation())
	{
		case InputElement::ABSOLUTE_AXIS:	lName += _T("AbsoluteAxis");	break;
		case InputElement::RELATIVE_AXIS:	lName += _T("RelativeAxis");	break;
		default:				lName += _T("Button");		break;
	}
	lName += strutil::IntToString(GetTypeIndex(), 10);
	return (lName);
}

void InputElement::SetIdentifier(const str& pIdentifier)
{
	mIdentifier = pIdentifier;
}

void InputElement::AddFunctor(InputFunctor* pFunctor)
{
	mFunctorArray.push_back(pFunctor);
}

void InputElement::ClearFunctors()
{
	for (FunctorArray::iterator x = mFunctorArray.begin(); x != mFunctorArray.end(); ++x)
	{
		delete (*x);
	}
	mFunctorArray.clear();
}

void InputElement::SetValue(float64 pNewValue)
{
	static const float64 lInputEpsilon = 1e-8;
	if (fabs(pNewValue - mValue) > lInputEpsilon)
	{
		//::printf("%s(%i) = %f", GetIdentifier().c_str(), GetTypeIndex(), pNewValue);

		mPrevValue = mValue;
		mValue = pNewValue;

		// Notify our observers.
		for (FunctorArray::iterator x = mFunctorArray.begin(); x != mFunctorArray.end(); ++x)
		{
			(*x)->Call(this);
		}
	}
}



InputDevice::InputDevice(InputManager* pManager):
	mManager(pManager),
	mInterpretation(TYPE_OTHER),
	mTypeIndex(-1),
	mNumDigitalElements(-1),
	mNumAnalogueElements(-1),
	mActive(false)
{
}

InputDevice::~InputDevice()
{
}

InputDevice::Interpretation InputDevice::GetInterpretation() const
{
	return (mInterpretation);
}

int InputDevice::GetTypeIndex() const
{
	return (mTypeIndex);
}

void InputDevice::SetInterpretation(Interpretation pInterpretation, int pTypeIndex)
{
	mInterpretation = pInterpretation;
	mTypeIndex = pTypeIndex;
}

InputManager* InputDevice::GetManager() const
{
	return (mManager);
}

bool InputDevice::IsActive()
{
	return mActive;
}

void InputDevice::SetActive(bool pActive)
{
	mActive = pActive;
}

void InputDevice::SetElementValue(InputElement* pElement, float64 pValue)
{
	pElement->SetValue(pValue);
}

const InputDevice::ElementArray& InputDevice::GetElements() const
{
	return mElementArray;
}

InputElement* InputDevice::GetElement(unsigned pElementIndex) const
{
	return mElementArray[pElementIndex];
}

unsigned InputDevice::GetNumElements() const
{
	return ((unsigned)mElementArray.size());
}

unsigned InputDevice::GetNumDigitalElements()
{
	if (mNumDigitalElements == -1)
	{
		CountElements();
	}
	return mNumDigitalElements;
}

unsigned InputDevice::GetNumAnalogueElements()
{
	if (mNumAnalogueElements == -1)
	{
		CountElements();
	}
	return mNumAnalogueElements;
}

void InputDevice::SetIdentifier(const str& pIdentifier)
{
	mIdentifier = pIdentifier;
}

const str& InputDevice::GetIdentifier() const
{
	return mIdentifier;
}

const InputElement* InputDevice::GetElement(const str& pIdentifier) const
{
	ElementArray::const_iterator x;
	for (x = mElementArray.begin();
		x != mElementArray.end();
		++x)
	{
		InputElement* lElement = *x;
		if (lElement->GetIdentifier() == pIdentifier)
		{
			return lElement;
		}
	}

	return 0;
}


InputElement* InputDevice::GetElement(const str& pIdentifier)
{
	ElementArray::iterator x;

	for (x = mElementArray.begin();
		x != mElementArray.end();
		++x)
	{
		InputElement* lElement = *x;
		if (lElement->GetIdentifier() == pIdentifier)
		{
			return lElement;
		}
	}

	return 0;
}

unsigned InputDevice::GetElementIndex(InputElement* pElement) const
{
	ElementArray::const_iterator x;

	int i = 0;
	for (x = mElementArray.begin();
		x != mElementArray.end();
		++x)
	{
		InputElement* lElement = *x;
		if (lElement == pElement)
		{
			return i;
		}
		i++;
	}

	return ((unsigned)-1);
}

InputElement* InputDevice::GetButton(unsigned pButtonIndex) const
{
	ElementArray::const_iterator x;

	unsigned lCurrentButton = 0;
	for (x = mElementArray.begin();
		x != mElementArray.end();
		++x)
	{
		InputElement* lElement = *x;
		if (lElement->GetType() == InputElement::DIGITAL)
		{
			if (lCurrentButton == pButtonIndex)
			{
				return lElement;
			}
			lCurrentButton++;
		}
	}

	return 0;
}

InputElement* InputDevice::GetAxis(unsigned pAxisIndex) const
{
	ElementArray::const_iterator x;

	unsigned lCurrentAxis = 0;
	for (x = mElementArray.begin();
		x != mElementArray.end();
		++x)
	{
		InputElement* lElement = *x;
		if (lElement->GetType() == InputElement::ANALOGUE)
		{
			if (lCurrentAxis == pAxisIndex)
			{
				return lElement;
			}
			lCurrentAxis++;
		}
	}

	return 0;
}



void InputDevice::CountElements()
{
	mNumDigitalElements  = 0;
	mNumAnalogueElements = 0;

	ElementArray::iterator x;
	for (x = mElementArray.begin(); x != mElementArray.end(); ++x)
	{
		InputElement* lElement = *x;

		if (lElement->GetType() == InputElement::DIGITAL)
		{
			mNumDigitalElements++;
		}
		else if(lElement->GetType() == InputElement::ANALOGUE)
		{
			mNumAnalogueElements++;
		}
	}
}

void InputDevice::AddFunctor(InputFunctor* pFunctor)
{
	ElementArray::iterator x;
	for (x = mElementArray.begin(); x != mElementArray.end(); ++x)
	{
		InputElement* lElement = *x;
		lElement->AddFunctor(pFunctor->CreateCopy());
	}
	delete (pFunctor);
}

void InputDevice::ClearFunctors()
{
	ElementArray::iterator x;
	for (x = mElementArray.begin(); x != mElementArray.end(); ++x)
	{
		InputElement* lElement = *x;
		lElement->ClearFunctors();
	}
}

InputDevice::CalibrationData InputDevice::GetCalibration() const
{
	CalibrationData lData;
	ElementArray::const_iterator x = mElementArray.begin();
	for (; x != mElementArray.end(); ++x)
	{
		InputElement* lElement = *x;
		if (lElement->GetInterpretation() == InputElement::ABSOLUTE_AXIS)
		{
			lData.push_back(CalibrationElement(lElement->GetName(), lElement->GetCalibration()));
		}
	}
	return (lData);
}

bool InputDevice::SetCalibration(const CalibrationData& pData)
{
	bool lOk = true;
	CalibrationData::const_iterator y = pData.begin();
	for (; y != pData.end(); ++y)
	{
		const CalibrationElement& lCalibration = *y;
		ElementArray::iterator x = mElementArray.begin();
		for (; x != mElementArray.end(); ++x)
		{
			InputElement* lElement = *x;
			if (lElement->GetName() == lCalibration.first)
			{
				lOk &= lElement->SetCalibration(lCalibration.second);
			}
		}
	}
	return (lOk);
}







InputManager::InputManager()
{
	::memset(mKeyDown, 0, sizeof(mKeyDown));
}

InputManager::~InputManager()
{
}

const InputManager::DeviceList& InputManager::GetDeviceList() const
{
	return (mDeviceList);
}

void InputManager::AddTextInputObserver(TextInputObserver* pListener)
{
	mTextObserverList.insert(pListener);
}

void InputManager::RemoveTextInputObserver(TextInputObserver* pListener)
{
	mTextObserverList.erase(pListener);
}

void InputManager::AddKeyCodeInputObserver(KeyCodeInputObserver* pListener)
{
	mKeyCodeObserverList.insert(pListener);
}

void InputManager::RemoveKeyCodeInputObserver(KeyCodeInputObserver* pListener)
{
	mKeyCodeObserverList.erase(pListener);
}

void InputManager::AddMouseInputObserver(MouseInputObserver* pListener)
{
	mMouseObserverList.insert(pListener);
}

void InputManager::RemoveMouseInputObserver(MouseInputObserver* pListener)
{
	mMouseObserverList.erase(pListener);
}

bool InputManager::ReadKey(KeyCode pKeyCode)
{
	return (mKeyDown[(int)pKeyCode]);
}

str InputManager::GetKeyName(KeyCode pKeyCode)
{
	const tchar* lKeyName = 0;
#define X(name)	case IN_KBD_##name:	lKeyName = _T(#name);	break
	switch (pKeyCode)
	{
		X(BACKSPACE);
		X(TAB);
		X(ENTER);
		X(LSHIFT);
		X(LCTRL);
		X(LALT);
		X(PAUSE);
		X(CAPS_LOCK);
		X(ESC);
		X(SPACE);
		X(PGUP);
		X(PGDOWN);
		X(END);
		X(HOME);
		X(LEFT);
		X(UP);
		X(RIGHT);
		X(DOWN);
		X(PRINT_SCREEN);
		X(INSERT);
		X(DEL);
		X(LWIN);
		X(RWIN);
		X(CONTEXT_MENU);
		X(NUMPAD_0);
		X(NUMPAD_1);
		X(NUMPAD_2);
		X(NUMPAD_3);
		X(NUMPAD_4);
		X(NUMPAD_5);
		X(NUMPAD_6);
		X(NUMPAD_7);
		X(NUMPAD_8);
		X(NUMPAD_9);
		X(NUMPAD_MUL);
		X(NUMPAD_PLUS);
		X(NUMPAD_MINUS);
		X(NUMPAD_DOT);
		X(NUMPAD_DIV);
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
		X(NUM_LOCK);
		X(SCROLL_LOCK);
		X(QUICK_BACK);
		X(QUICK_FORWARD);
		X(QUICK_REFRESH);
		X(QUICK_STOP);
		X(QUICK_SEARCH);
		X(QUICK_FAVORITES);
		X(QUICK_WEB_HOME);
		X(QUICK_SOUND_MUTE);
		X(QUICK_DECR_VOLUME);
		X(QUICK_INCR_VOLUME);
		X(QUICK_NAV_RIGHT);
		X(QUICK_NAV_LEFT);
		X(QUICK_NAV_STOP);
		X(QUICK_NAV_PLAYPAUSE);
		X(QUICK_MAIL);
		X(QUICK_MEDIA);
		X(QUICK_MY_COMPUTER);
		X(QUICK_CALCULATOR);
		X(DIAERESIS);
		X(PLUS);
		X(COMMA);
		X(MINUS);
		X(DOT);
		X(APOSTROPHE);
		X(ACUTE);
		X(PARAGRAPH);
		X(COMPARE);
		X(RSHIFT);
		X(RCTRL);
		X(RALT);
		default: break;
	}
#undef X
	if (lKeyName)
	{
		return (lKeyName);
	}
	return (str(1, (tchar)pKeyCode));
}

void InputManager::SetKey(KeyCode pKeyCode, bool pValue)
{
	mKeyDown[(int)pKeyCode] = pValue;
}

bool InputManager::NotifyOnChar(tchar pChar)
{
	bool lConsumed = false;
	TextObserverList::iterator x = mTextObserverList.begin();
	for (; !lConsumed && x != mTextObserverList.end(); ++x)
	{
		lConsumed = (*x)->OnChar(pChar);
	}
	return (lConsumed);
}

bool InputManager::NotifyOnKeyDown(KeyCode pKeyCode)
{
	bool lConsumed = false;
	KeyCodeObserverList::iterator x = mKeyCodeObserverList.begin();
	for (; !lConsumed && x != mKeyCodeObserverList.end(); ++x)
	{
		lConsumed = (*x)->OnKeyDown(pKeyCode);
	}
	return (lConsumed);
}

bool InputManager::NotifyOnKeyUp(KeyCode pKeyCode)
{
	bool lConsumed = false;
	KeyCodeObserverList::iterator x = mKeyCodeObserverList.begin();
	for (; !lConsumed && x != mKeyCodeObserverList.end(); ++x)
	{
		lConsumed = (*x)->OnKeyUp(pKeyCode);
	}
	return (lConsumed);
}

bool InputManager::NotifyMouseDoubleClick()
{
	bool lConsumed = false;
	MouseObserverList::iterator x = mMouseObserverList.begin();
	for (; !lConsumed && x != mMouseObserverList.end(); ++x)
	{
		lConsumed = (*x)->OnDoubleClick();
	}
	return (lConsumed);
}

void InputManager::PollEvents()
{
	DeviceList::iterator x;
	for (x = mDeviceList.begin(); 
		x != mDeviceList.end(); 
		++x)
	{
		InputDevice* lDevice = *x;
		lDevice->PollEvents();
	}
}

void InputManager::AddFunctor(InputFunctor* pFunctor)
{
	DeviceList::iterator x;
	for (x = mDeviceList.begin(); x != mDeviceList.end(); ++x)
	{
		InputDevice* lDevice = *x;
		lDevice->AddFunctor(pFunctor->CreateCopy());
	}
	delete (pFunctor);
}

void InputManager::ClearFunctors()
{
	DeviceList::iterator x;
	for (x = mDeviceList.begin(); x != mDeviceList.end(); ++x)
	{
		InputDevice* lDevice = *x;
		lDevice->ClearFunctors();
	}
}

void InputManager::ActivateAll()
{
	DeviceList::iterator x;
	for (x = mDeviceList.begin(); x != mDeviceList.end(); ++x)
	{
		InputDevice* lDevice = *x;
		lDevice->Activate();
	}
}

void InputManager::ReleaseAll()
{
	DeviceList::iterator x;
	for (x = mDeviceList.begin(); 
		x != mDeviceList.end(); 
		++x)
	{
		InputDevice* lDevice = *x;
		lDevice->Release();
	}
}

InputDevice* InputManager::FindDevice(const str& pDeviceIdentifier, int pN)
{
	int lN = 0;

	DeviceList::iterator x;
	for (x = mDeviceList.begin(); 
		x != mDeviceList.end(); 
		++x)
	{
		InputDevice* lDevice = *x;
		if (pDeviceIdentifier == lDevice->GetIdentifier())
		{
			if (lN == pN)
			{
				return lDevice;
			}
			else
			{
				lN++;
			}
		}
	}

	return 0;
}

int InputManager::GetDeviceCount(const str& pDeviceIdentifier) const
{
	int lCount = 0;

	DeviceList::const_iterator x;
	for (x = mDeviceList.begin(); 
		x != mDeviceList.end(); 
		++x)
	{
		InputDevice* lDevice = *x;
		if (pDeviceIdentifier == lDevice->GetIdentifier())
		{
			lCount++;
		}
	}

	return lCount;
}

unsigned InputManager::GetDeviceIndex(InputDevice* pDevice) const
{
	int lCount = 0;

	DeviceList::const_iterator x;
	for (x = mDeviceList.begin(); 
		x != mDeviceList.end(); 
		++x)
	{
		InputDevice* lDevice = *x;
		if (pDevice->GetIdentifier() == lDevice->GetIdentifier())
		{
			if (pDevice == lDevice)
			{
				return lCount;
			}

			lCount++;
		}
	}

	return 0;
}



}
