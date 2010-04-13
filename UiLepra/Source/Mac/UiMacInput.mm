
// Author: Jonas Bystršm
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/Mac/UiMacInput.h"
#include "../../../Lepra/Include/Log.h"
#include "../../Include/Mac/UiMacCore.h"
#include "../../Include/Mac/UiMacDisplayManager.h"



namespace UiLepra
{



InputManager* InputManager::CreateInputManager(DisplayManager* pDisplayManager)
{
	return (new MacInputManager((MacDisplayManager*)pDisplayManager));
}



class MacInputElement;	// TODO: REMOVEME!



#if 0



MacInputElement::MacInputElement(Type pType, Interpretation pInterpretation, int pTypeIndex,
	MacInputDevice* pParentDevice, LPCDIDEVICEOBJECTINSTANCE pElement, unsigned pFieldOffset):
	InputElement(pType, pInterpretation, pTypeIndex, pParentDevice),
	mElement(pElement),
	mMin(MAX_INT),
	mMax(MIN_INT)
{
	SetIdentifier(mElement->tszName);

	mDataFormat.dwType  = mElement->dwType;
	mDataFormat.pguid   = 0;
	mDataFormat.dwOfs   = (DWORD)pFieldOffset;
	mDataFormat.dwFlags = 0;
}

MacInputElement::~MacInputElement()
{
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

	if (GetInterpretation() == RELATIVE_AXIS)
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



#endif // 0



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
			//lElement->PollEvents();
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
		switch (lCurrentElement->type)
		{
			case kIOHIDElementTypeInput_Button:
			{
				//CreateButtonElement(lCurrentElement);
			}
			break;
			case kIOHIDElementTypeInput_Axis:
			{
				//CreateAxisElement(lCurrentElement);
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
							//CreateButtonElement(lCurrentElement);
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
							//CreateAxisElement(lCurrentElement);
						}
						break;
						default:
						{
							printf("Misc element:\n");
							printf("-------------\n");
							printf("name: %s\n", lCurrentElement->name);
							printf("usagePage: %i\nusage: %i\n", lCurrentElement->usagePage, lCurrentElement->usage);
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
				printf("min: %i\nmax: %i\nsize: %i\n", lCurrentElement->min, lCurrentElement->max, lCurrentElement->size);
			}
			break;
			default:
			{
				printf("unknown element type: %d\n", lCurrentElement->type);
			}
			break;
		}
		lCurrentElement = HIDGetNextDeviceElement(lCurrentElement, kHIDElementTypeInput);
	}
}



LOG_CLASS_DEFINE(UI_INPUT, MacInputDevice);



MacInputManager::MacInputManager(MacDisplayManager* pDisplayManager):
	mDisplayManager(pDisplayManager),
	//mDirectInput(0),
	mEnumError(false),
	mInitialized(false),
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
		pRecDevice lHIDDevice = HIDGetFirstDevice();
		while (lHIDDevice)
		{
			mDeviceList.push_back(new MacInputDevice(lHIDDevice, this));
			lHIDDevice = HIDGetNextDevice(lHIDDevice);
		}
	}

	Refresh();

	AddObserver();

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

bool MacInputManager::IsInitialized()
{
	return (mInitialized);
}

void MacInputManager::Refresh()
{
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

MacDisplayManager* MacInputManager::GetDisplayManager() const
{
	return (mDisplayManager);
}

bool MacInputManager::OnMessage(NSEvent* pEvent)
{
	bool lConsumed = false;
	/*switch (pMsg)
	{
		case WM_CHAR:
		{
			lConsumed = NotifyOnChar((tchar)pwParam);
		}
		break;
		case WM_LBUTTONDBLCLK:
		{
			lConsumed = NotifyMouseDoubleClick();
		}
		break;
		case WM_MOUSEMOVE:
		case WM_NCMOUSEMOVE:
		{
			int x = GET_X_LPARAM(plParam);
			int y = GET_Y_LPARAM(plParam);
			SetMousePosition(pMsg, x, y);
		}
		break;
		case WM_KEYUP:
		{
			SetKey((KeyCode)pwParam, plParam, false);
			lConsumed = NotifyOnKeyUp((KeyCode)pwParam);
		}
		break;
		case WM_KEYDOWN:
		{
			SetKey((KeyCode)pwParam, plParam, true);
			lConsumed = NotifyOnKeyDown((KeyCode)pwParam);
		}
		break;
		case WM_SYSKEYUP:
		{
			SetKey((KeyCode)pwParam, plParam, false);
			lConsumed = NotifyOnKeyUp((KeyCode)pwParam);
		}
		break;
		case WM_SYSKEYDOWN:
		{
			SetKey((KeyCode)pwParam, plParam, true);
			lConsumed = NotifyOnKeyDown((KeyCode)pwParam);
		}
		break;
	}*/
	return (lConsumed);
}

void MacInputManager::SetKey(KeyCode pWParam, long pLParam, bool pIsDown)
{
	/*if (pLParam&0x1000000)	// Extended key = right Alt, Ctrl...
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
	}*/
	Parent::SetKey(pWParam, pIsDown);
}

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
	mCursorX = 2.0 * (double)x / (double)mScreenWidth  - 1.0;
	mCursorY = 2.0 * (double)y / (double)mScreenHeight - 1.0;
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

void MacInputManager::AddObserver()
{
	if (mDisplayManager)
	{
		// Listen to text input and standard mouse events.
		/*mDisplayManager->AddObserver(WM_CHAR, this);
		mDisplayManager->AddObserver(WM_SYSKEYDOWN, this);
		mDisplayManager->AddObserver(WM_SYSKEYUP, this);
		mDisplayManager->AddObserver(WM_KEYDOWN, this);
		mDisplayManager->AddObserver(WM_KEYUP, this);
		mDisplayManager->AddObserver(WM_MOUSEMOVE, this);
		mDisplayManager->AddObserver(WM_NCMOUSEMOVE, this);
		mDisplayManager->AddObserver(WM_LBUTTONDBLCLK, this);
		mDisplayManager->AddObserver(WM_LBUTTONDOWN, this);
		mDisplayManager->AddObserver(WM_RBUTTONDOWN, this);
		mDisplayManager->AddObserver(WM_MBUTTONDOWN, this);
		mDisplayManager->AddObserver(WM_LBUTTONUP, this);
		mDisplayManager->AddObserver(WM_RBUTTONUP, this);
		mDisplayManager->AddObserver(WM_MBUTTONUP, this);*/
	}
}

void MacInputManager::RemoveObserver()
{
	if (mDisplayManager)
	{
		mDisplayManager->RemoveObserver((MacInputManager*)this);
	}
}



}
