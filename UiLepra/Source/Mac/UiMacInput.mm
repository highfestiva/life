
// Author: Jonas Bystršm
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/Mac/UiMacInput.h"
#include "../../../Lepra/Include/CyclicArray.h"
#include "../../../Lepra/Include/Log.h"
#include "../../Include/Mac/UiMacCore.h"
#include "../../Include/Mac/UiMacDisplayManager.h"



namespace UiLepra
{



InputManager* InputManager::CreateInputManager(DisplayManager* pDisplayManager)
{
	return (new MacInputManager((MacDisplayManager*)pDisplayManager));
}



MacInputElement::MacInputElement(Type pType, Interpretation pInterpretation, int pTypeIndex,
	MacInputDevice* pParentDevice, pRecElement pElement):
	InputElement(pType, pInterpretation, pTypeIndex, pParentDevice),
	mElement(pElement),
	mMin(MAX_INT),
	mMax(MIN_INT)
{
	SetIdentifier(strutil::Encode((const char*)mElement->name));
}

MacInputElement::~MacInputElement()
{
}

pRecElement MacInputElement::GetNativeElement() const
{
	return (mElement);
}

void MacInputElement::SetValue(int pValue)
{
	// Calibrate...
	if (pValue < mMin)
	{
		mMin = pValue;
	}
	if (pValue > mMax)
	{
		mMax = pValue;
	}

	MacInputDevice* lDevice = (MacInputDevice*)GetParentDevice();
	MacInputManager* lManager = (MacInputManager*)lDevice->GetManager();

	if (GetType() == DIGITAL)
	{
		Parent::SetValue(pValue);
	}
	else if (GetInterpretation() == RELATIVE_AXIS)
	{
		// Treat this as a relative axis. Since we don't know the maximum value
		// of this axis (can probably be infinitly large), we need to scale it down
		// to some kind of common unit. The mouse x- and y-axes fall into this 
		// category, and the mouse is the primary device that we should take into 
		// consideration when finding the appropriate scale factor.
		//
		// To give the relative mouse coordinates the same unit as for absolute ones,
		// we will divide the value by half the screen width and height.

		if (GetTypeIndex() <= 1)	// Mouse of sorts.
		{
			InputElement::SetValue(2.0 * (double)pValue / (double)lManager->mScreenWidth);
		}
		else
		{
			InputElement::SetValue((double)pValue / 1000.0);
		}
	}
	else if (mMin < mMax)
	{
		// Scale to +-1.
		InputElement::SetValue((pValue*2.0-(mMax+mMin)) / (double)(mMax-mMin));
	}
}

str MacInputElement::GetCalibration() const
{
	str lData;
	lData += strutil::IntToString(mMin, 10);
	lData += _T(", ");
	lData += strutil::IntToString(mMax, 10);
	return (lData);
}

bool MacInputElement::SetCalibration(const str& pData)
{
	bool lOk = false;
	strutil::strvec lData = strutil::Split(pData, _T(", "));
	if (lData.size() >= 2)
	{
		lOk = true;
		lOk &= strutil::StringToInt(lData[0], mMin, 10);
		lOk &= strutil::StringToInt(lData[1], mMax, 10);
	}
	return (lOk);
}



LOG_CLASS_DEFINE(UI_INPUT, MacInputElement);



/*
	class MacInputDevice
*/

MacInputDevice::MacInputDevice(pRecDevice pNativeDevice, InputManager* pManager):
	InputDevice(pManager),
	mNativeDevice(pNativeDevice),
	mRelAxisCount(0),
	mAbsAxisCount(0),
	mAnalogueCount(0),
	mButtonCount(0)
{
	SetIdentifier(strutil::Encode((const char*)pNativeDevice->manufacturer) + _T(":") + strutil::Encode((const char*)pNativeDevice->product));
	EnumElements();
}

MacInputDevice::~MacInputDevice()
{
	if (IsActive() == true)
	{
		Release();
	}
	//mNativeDevice->Release();

	ElementArray::iterator x;
	for (x = mElementArray.begin(); x != mElementArray.end(); ++x)
	{
		InputElement* lElement = *x;
		delete lElement;
	}
	mElementArray.clear();
}

void MacInputDevice::Activate()
{
	if (IsActive() == false)
	{
		//mNativeDevice->Acquire();
		SetActive(true);
	}
}

void MacInputDevice::Release()
{
	//mNativeDevice->Unacquire();
	SetActive(false);
}

void MacInputDevice::PollEvents()
{
	if (IsActive() == true)
	{
		ElementArray::iterator x;
		for (x = mElementArray.begin(); x != mElementArray.end(); ++x)
		{
			MacInputElement* lElement = (MacInputElement*)*x;
			pRecElement lNativeElement = lElement->GetNativeElement();
			if (lElement->GetType() == InputElement::ANALOGUE)
			{
				const int32 lValue = HIDGetElementValue(mNativeDevice, lNativeElement);
				const int32 lValueCal = HIDCalibrateValue(lValue, lNativeElement);
				const int32 lValueScale = HIDScaleValue(lValueCal, lNativeElement);
				lElement->SetValue(lValueScale / 127.5f - 1);
			}
			else if (lElement->GetType() == InputElement::DIGITAL)
			{
				const int32 lValue = HIDGetElementValue(mNativeDevice, lNativeElement);
				lElement->SetValue((lValue > 0)? 1 : 0);
			}
		}
	}
}

bool MacInputDevice::HaveRelativeAxes()
{
	return (mRelAxisCount > mAbsAxisCount);
}

void MacInputDevice::EnumElements()
{
	pRecElement lCurrentElement = HIDGetFirstDeviceElement(mNativeDevice, kHIDElementTypeInput);
	while (lCurrentElement)
	{
		MacInputElement* lElement = 0;
		switch (lCurrentElement->type)
		{
			case kIOHIDElementTypeInput_Button:
			{
				InputElement::Interpretation lInterpretation = (InputElement::Interpretation)((int)InputElement::BUTTON1 + mButtonCount);
				lElement = new MacInputElement(InputElement::DIGITAL, lInterpretation, mButtonCount, this, lCurrentElement);
				++mButtonCount;
			}
			break;
			case kIOHIDElementTypeInput_Axis:
			{
				InputElement::Interpretation lInterpretation = InputElement::ABSOLUTE_AXIS;	// TODO: ...
				lElement = new MacInputElement(InputElement::ANALOGUE, lInterpretation, mAnalogueCount, this, lCurrentElement);
				++mAnalogueCount;
			}
			break;
			case kIOHIDElementTypeInput_Misc:
			{
				char name[256];
				HIDGetUsageName(lCurrentElement->usagePage, lCurrentElement->usage, name);
				if (lCurrentElement->usagePage == kHIDPage_GenericDesktop)
				{
					switch(lCurrentElement->usage)
					{
						case kHIDUsage_GD_DPadUp:
						case kHIDUsage_GD_DPadDown:
						case kHIDUsage_GD_DPadRight:
						case kHIDUsage_GD_DPadLeft:
						{
							InputElement::Interpretation lInterpretation = (InputElement::Interpretation)((int)InputElement::BUTTON1 + mButtonCount);
							lElement = new MacInputElement(InputElement::DIGITAL, lInterpretation, mButtonCount, this, lCurrentElement);
							++mButtonCount;
						}
						break;
						case kHIDUsage_GD_X:
						case kHIDUsage_GD_Y:
						case kHIDUsage_GD_Z:
						case kHIDUsage_GD_Rx:
						case kHIDUsage_GD_Ry:
						case kHIDUsage_GD_Rz:
						case kHIDUsage_GD_Slider:
						case kHIDUsage_GD_Wheel:
						{
							InputElement::Interpretation lInterpretation = InputElement::ABSOLUTE_AXIS;	// TODO: ...
							lElement = new MacInputElement(InputElement::ANALOGUE, lInterpretation, mAnalogueCount, this, lCurrentElement);
							++mAnalogueCount;
						}
						break;
						default:
						{
							printf("Misc element:\n");
							printf("-------------\n");
							printf("name: %s\n", lCurrentElement->name);
							printf("usagePage: %li\nusage: %li\n", lCurrentElement->usagePage, lCurrentElement->usage);
							printf("usagename: %s\n", name);
						}
						break;
					}
				}
			}
			break;
			case kIOHIDElementTypeInput_ScanCodes:
			{
				printf("warning: kIOHIDElementTypeInput_ScanCodes support not implemented yet!\n");
				printf("min: %li\nmax: %li\nsize: %li\n", lCurrentElement->min, lCurrentElement->max, lCurrentElement->size);
			}
			break;
			default:
			{
				printf("unknown element type: %d\n", lCurrentElement->type);
			}
			break;
		}
		if (lElement)
		{
			mElementArray.push_back(lElement);
		}
		lCurrentElement = HIDGetNextDeviceElement(lCurrentElement, kHIDElementTypeInput);
	}
}



LOG_CLASS_DEFINE(UI_INPUT, MacInputDevice);



MacInputManager::MacInputManager(MacDisplayManager* pDisplayManager):
	mDisplayManager(pDisplayManager),
	mEnumError(false),
	mInitialized(false),
	mKeyModifiers(0x100),
	mScreenWidth(0),
	mScreenHeight(0),
	mCursorX(0),
	mCursorY(0),
	mKeyboard(0),
	mMouse(0)
{
	HIDBuildDeviceList(0, 0);
	if (HIDHaveDeviceList())
	{
		EnumDevices();
	}

	Refresh();

	AddObserver();

	mInputManagerSingleton = this;

	mInitialized = true;
}

MacInputManager::~MacInputManager()
{
	if (mInitialized == true)
	{
		HIDReleaseDeviceList();
	}

	RemoveObserver();

	DeviceList::iterator lDIter;
	for (lDIter = mDeviceList.begin(); lDIter != mDeviceList.end(); ++lDIter)
	{
		InputDevice* lDevice = *lDIter;
		delete lDevice;
	}

	mDisplayManager = 0;
}

MacInputManager* MacInputManager::GetSingleton()
{
	return (mInputManagerSingleton);
}

bool MacInputManager::IsInitialized()
{
	return (mInitialized);
}

void MacInputManager::Refresh()
{
	mScreenWidth  = 640;
	mScreenHeight = 480;
	/*if (mDisplayManager != 0 && mDisplayManager->GetHWND() != 0)
	{
		RECT lRect;
		::GetClientRect(mDisplayManager->GetHWND(), &lRect);
		
		mScreenWidth  = lRect.right - lRect.left;
		mScreenHeight = lRect.bottom - lRect.top;
	}
	else
	{
		// Get the entire screen area.
		mScreenWidth  = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
		mScreenHeight = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);

		if (mScreenWidth == 0 || mScreenHeight == 0)
		{
			// Virtual screen not supported, use the primary display.
			mScreenWidth  = ::GetSystemMetrics(SM_CXSCREEN);
			mScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);
		}
	}*/
}

unichar MacInputManager::ConvertChar(unichar pChar)
{
	bool lIsChar;
	ConvertCharToKeyCode(pChar, lIsChar);
	if (!lIsChar)
	{
		return 0;
	}
	switch (pChar)
	{
		case 0x7F:	return '\b';
	}
	return pChar;
}

MacInputManager::KeyCode MacInputManager::ConvertCharToKeyCode(unichar pChar, bool& pIsChar)
{
	pIsChar = true;
#define NC()	pIsChar = false;
	mLog.Infof(_T("Got key: %x (%u, '%c')"), (unsigned)pChar, (unsigned)pChar, pChar);
	switch (pChar)
	{
		case NSUpArrowFunctionKey:	NC();	return IN_KBD_UP;
		case NSDownArrowFunctionKey:	NC();	return IN_KBD_DOWN;
		case NSLeftArrowFunctionKey:	NC();	return IN_KBD_LEFT;
		case NSRightArrowFunctionKey:	NC();	return IN_KBD_RIGHT;
		case NSF1FunctionKey:		NC();	return IN_KBD_F1;
		case NSF2FunctionKey:		NC();	return IN_KBD_F2;
		case NSF3FunctionKey:		NC();	return IN_KBD_F3;
		case NSF4FunctionKey:		NC();	return IN_KBD_F4;
		case NSF5FunctionKey:		NC();	return IN_KBD_F5;
		case NSF6FunctionKey:		NC();	return IN_KBD_F6;
		case NSF7FunctionKey:		NC();	return IN_KBD_F7;
		case NSF8FunctionKey:		NC();	return IN_KBD_F8;
		case NSF9FunctionKey:		NC();	return IN_KBD_F9;
		case NSF10FunctionKey:		NC();	return IN_KBD_F10;
		case NSF11FunctionKey:		NC();	return IN_KBD_F11;
		case NSF12FunctionKey:		NC();	return IN_KBD_F12;
		case 0xF746:	// Fall thru.
		case NSInsertFunctionKey:	NC();	return IN_KBD_INSERT;
		case NSDeleteFunctionKey:	NC();	return IN_KBD_DEL;
		case NSHomeFunctionKey:		NC();	return IN_KBD_HOME;
		case NSEndFunctionKey:		NC();	return IN_KBD_END;
		case NSPageUpFunctionKey:	NC();	return IN_KBD_PGUP;
		case NSPageDownFunctionKey:	NC();	return IN_KBD_PGDOWN;
		case NSSysReqFunctionKey:	// Fall thru.
		case NSPrintScreenFunctionKey:	NC();	return IN_KBD_PRINT_SCREEN;
		case NSScrollLockFunctionKey:	NC();	return IN_KBD_SCROLL_LOCK;
		case NSBreakFunctionKey:	// Fall thru.
		case NSPauseFunctionKey:	NC();	return IN_KBD_PAUSE;
		case NSMenuFunctionKey:		NC();	return IN_KBD_CONTEXT_MENU;
	}
	if (pChar >= 'a' && pChar <= 'z')
	{
		pChar -= 'a'-'A';
	}
	return (KeyCode)pChar;
}

MacDisplayManager* MacInputManager::GetDisplayManager() const
{
	return (mDisplayManager);
}

void MacInputManager::OnEvent(NSEvent* pEvent)
{
	bool lIsKeyDown = false;
	switch ([pEvent type])
	{
		case NSKeyDown:
		{
			{
				const NSString* lCharacters = [pEvent charactersIgnoringModifiers];
				const unsigned lCharacterCount = [lCharacters length];
				for (unsigned x = 0; x < lCharacterCount; ++x)
				{
					unichar lChar = [lCharacters characterAtIndex:x];
					lChar = ConvertChar(lChar);
					if (lChar)
					{
						NotifyOnChar(lChar);
					}
				}
			}
			lIsKeyDown = true;
		}
		// TRICKY: fall thru!
		case NSKeyUp:
		{
			const NSString* lCharacters = [pEvent characters];
			const unsigned lCharacterCount = [lCharacters length];
			for (unsigned x = 0; x < lCharacterCount; ++x)
			{
				const unichar lChar = [lCharacters characterAtIndex:x];
				bool lIsChar;
				const KeyCode lKey = ConvertCharToKeyCode(lChar, lIsChar);
				SetKey(lKey, lIsKeyDown);
				if (lIsKeyDown)
				{
					NotifyOnKeyDown(lKey);
				}
				else
				{
					NotifyOnKeyUp(lKey);
				}

			}
		}
		break;
		case NSFlagsChanged:
		{
			const NSUInteger lKeyModifiers = [pEvent modifierFlags];
			const NSUInteger lDeltaModifiers = mKeyModifiers ^ lKeyModifiers;
			const int lTranslationTable[][2] =
			{
				{ UiLepra::InputManager::IN_KBD_LCTRL,	1<<0, },
				{ UiLepra::InputManager::IN_KBD_LSHIFT,	1<<1, },
				{ UiLepra::InputManager::IN_KBD_RSHIFT,	1<<2, },
				{ UiLepra::InputManager::IN_KBD_LALT,	1<<3, },
				{ UiLepra::InputManager::IN_KBD_RALT,	1<<4, },
				{ UiLepra::InputManager::IN_KBD_LWIN,	1<<5, },
				{ UiLepra::InputManager::IN_KBD_RWIN,	1<<6, },
				{ UiLepra::InputManager::IN_KBD_RCTRL,	1<<13, },
			};
			for (unsigned x = 0; x < LEPRA_ARRAY_COUNT(lTranslationTable); ++x)
			{
				if (!(lDeltaModifiers & lTranslationTable[x][1]))
				{
					continue;
				}
				const bool lIsKeyDown = (lKeyModifiers & lTranslationTable[x][1]);
				SetKey((KeyCode)lTranslationTable[x][0], lIsKeyDown);
				if (lIsKeyDown)
				{
					NotifyOnKeyDown((KeyCode)lTranslationTable[x][0]);
				}
				else
				{
					NotifyOnKeyUp((KeyCode)lTranslationTable[x][0]);
				}
			}
			mKeyModifiers = lKeyModifiers;
		}
		break;
		case NSMouseMoved:
		case NSLeftMouseDragged:
		case NSRightMouseDragged:
		{
			NSPoint lPoint = [pEvent locationInWindow];
			SetMousePosition(lPoint.x, lPoint.y);
		}
		break;
	}
}

/*void MacInputManager::SetKey(KeyCode pWParam, bool pIsDown)
{
	printf("Key: %i", pWParam);
	if (pLParam&0x1000000)	// Extended key = right Alt, Ctrl...
	{
		switch (pWParam)
		{
			case IN_KBD_LCTRL:		pWParam = IN_KBD_RCTRL;	break;
			case IN_KBD_LALT:		pWParam = IN_KBD_RALT;	break;
		}
	}
	else if (pWParam == IN_KBD_LSHIFT && (pLParam&0xFF0000) == 0x360000)
	{
		pWParam = IN_KBD_RSHIFT;
	}
	Parent::SetKey(pWParam, pIsDown);
}*/

/*BOOL CALLBACK MacInputManager::EnumDeviceCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	MacInputManager* lInputManager = (MacInputManager*)pvRef;

	HRESULT lHR;
	LPDIRECTINPUTDEVICE8 lDIDevice;
	
	lHR = lInputManager->mDirectInput->CreateDevice(lpddi->guidInstance, &lDIDevice, 0);

	if (lHR != DI_OK)
	{
		lInputManager->mEnumError = true;
		return DIENUM_STOP;
	}

	MacInputDevice* lDevice = new MacInputDevice(lDIDevice, lpddi, lInputManager);
	InputDevice::Interpretation lInterpretation = InputDevice::TYPE_OTHER;
	switch (lpddi->dwDevType & 0xFF)
	{
		case DI8DEVTYPE_MOUSE:		lInterpretation = InputDevice::TYPE_MOUSE;	break;
		case DI8DEVTYPE_KEYBOARD:	lInterpretation = InputDevice::TYPE_KEYBOARD;	break;
		case DI8DEVTYPE_JOYSTICK:	lInterpretation = InputDevice::TYPE_JOYSTICK;	break;
		case DI8DEVTYPE_GAMEPAD:	lInterpretation = InputDevice::TYPE_GAMEPAD;	break;
		case DI8DEVTYPE_1STPERSON:	lInterpretation = InputDevice::TYPE_1STPERSON;	break;
		case DI8DEVTYPE_DRIVING:
		{
			switch ((lpddi->dwDevType>>8) & 0xFF)
			{
				case DI8DEVTYPEDRIVING_COMBINEDPEDALS:
				case DI8DEVTYPEDRIVING_DUALPEDALS:
				case DI8DEVTYPEDRIVING_THREEPEDALS:	lInterpretation = InputDevice::TYPE_PEDALS;	break;
				default:				lInterpretation = InputDevice::TYPE_WHEEL;	break;
			}
		}
		break;
		case DI8DEVTYPE_FLIGHT:
		{
			switch ((lpddi->dwDevType>>8) & 0xFF)
			{
				case DI8DEVTYPEFLIGHT_RC:	lInterpretation = InputDevice::TYPE_GAMEPAD;	break;
				case DI8DEVTYPEFLIGHT_STICK:	lInterpretation = InputDevice::TYPE_JOYSTICK;	break;
				default:			lInterpretation = InputDevice::TYPE_FLIGHT;	break;
			}
		}
		break;
		case DI8DEVTYPE_SUPPLEMENTAL:
		{
			switch ((lpddi->dwDevType>>8) & 0xFF)
			{
				case DI8DEVTYPESUPPLEMENTAL_COMBINEDPEDALS:
				case DI8DEVTYPESUPPLEMENTAL_DUALPEDALS:
				case DI8DEVTYPESUPPLEMENTAL_RUDDERPEDALS:
				case DI8DEVTYPESUPPLEMENTAL_THREEPEDALS:	lInterpretation = InputDevice::TYPE_PEDALS;	break;
			}
		}
		break;
	}
	lDevice->SetInterpretation(lInterpretation, lInputManager->mTypeCount[lInterpretation]);
	++lInputManager->mTypeCount[lInterpretation];
	if (lInterpretation == InputDevice::TYPE_MOUSE && lInputManager->mMouse == 0)
	{
		lInputManager->mMouse = lDevice;
	}
	else if (lInterpretation == InputDevice::TYPE_KEYBOARD && lInputManager->mKeyboard == 0)
	{
		lInputManager->mKeyboard = lDevice;
	}
	lInputManager->mDeviceList.push_back(lDevice);

	return (DIENUM_CONTINUE);
}*/

void MacInputManager::ShowCursor()
{
	//::ShowCursor(TRUE);
}

void MacInputManager::HideCursor()
{
	//::ShowCursor(FALSE);
}

double MacInputManager::GetCursorX()
{
	return mCursorX;
}

double MacInputManager::GetCursorY()
{
	return mCursorY;
}

void MacInputManager::SetMousePosition(int x, int y)
{
	mCursorX = 2.0 * x / mScreenWidth  - 1.0;
	mCursorY = 2.0 * (mScreenHeight-y) / mScreenHeight - 1.0;
}

const InputDevice* MacInputManager::GetKeyboard() const
{
	return mKeyboard;
}

InputDevice* MacInputManager::GetKeyboard()
{
	return mKeyboard;
}

const InputDevice* MacInputManager::GetMouse() const
{
	return mMouse;
}

InputDevice* MacInputManager::GetMouse()
{
	return mMouse;
}

void MacInputManager::EnumDevices()
{
	pRecDevice lHIDDevice = HIDGetFirstDevice();
	while (lHIDDevice)
	{
		MacInputDevice* lDevice = new MacInputDevice(lHIDDevice, this);
		mDeviceList.push_back(lDevice);
		
		InputDevice::Interpretation lInterpretation = InputDevice::TYPE_OTHER;
		if (lDevice->GetNumDigitalElements() < 15 && lDevice->GetNumAnalogueElements() >= 2)
		{
			lInterpretation = InputDevice::TYPE_MOUSE;
		}
		else if (lDevice->GetNumDigitalElements() >= 40)
		{
			lInterpretation = InputDevice::TYPE_KEYBOARD;
		}
		lDevice->SetInterpretation(lInterpretation, mTypeCount[lInterpretation]);
		++mTypeCount[lInterpretation];
		
		if (lInterpretation == InputDevice::TYPE_MOUSE)
		{
			mMouse = lDevice;
			//printf("GOT MOUSE!!!\n");
		}
		else if (lInterpretation == InputDevice::TYPE_KEYBOARD)
		{
			mKeyboard = lDevice;
			//printf("GOT KEYBOARD!!!\n");
		}
		
		lHIDDevice = HIDGetNextDevice(lHIDDevice);
	}
}
	
void MacInputManager::AddObserver()
{
	if (mDisplayManager)
	{
		// Listen to text input and standard mouse events.
		mDisplayManager->AddObserver(NSKeyDown, this);
		mDisplayManager->AddObserver(NSKeyUp, this);
		mDisplayManager->AddObserver(NSFlagsChanged, this);
		mDisplayManager->AddObserver(NSMouseMoved, this);
		mDisplayManager->AddObserver(NSLeftMouseDragged, this);
		mDisplayManager->AddObserver(NSRightMouseDragged, this);
	}
}

void MacInputManager::RemoveObserver()
{
	if (mDisplayManager)
	{
		mDisplayManager->RemoveObserver((MacInputManager*)this);
	}
}



MacInputManager* MacInputManager::mInputManagerSingleton = 0;

LOG_CLASS_DEFINE(UI_INPUT, MacInputManager);



}
