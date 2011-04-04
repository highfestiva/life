
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games




#include "../../Include/Win32/UiWin32Input.h"
#include "../../../Lepra/Include/Log.h"
#include "../../Include/Win32/UiWin32Core.h"
#include "../../Include/Win32/UiWin32DisplayManager.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")



namespace UiLepra
{



InputManager* InputManager::CreateInputManager(DisplayManager* pDisplayManager)
{
	return (new Win32InputManager((Win32DisplayManager*)pDisplayManager));
}

Win32InputElement::Win32InputElement(Type pType, Interpretation pInterpretation, int pTypeIndex,
	Win32InputDevice* pParentDevice, LPCDIDEVICEOBJECTINSTANCE pElement, unsigned pFieldOffset):
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

Win32InputElement::~Win32InputElement()
{
}

void Win32InputElement::SetValue(int pValue)
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

	//Win32InputDevice* lDevice = (Win32InputDevice*)GetParentDevice();
	//Win32InputManager* lManager = (Win32InputManager*)lDevice->GetManager();

	if (GetType() == DIGITAL)
	{
		Parent::SetValue((float)pValue);
	}
	/*else if (GetInterpretation() == RELATIVE_AXIS)
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
			Parent::SetValue(2.0f * (float)pValue / (float)lManager->mScreenWidth);
		}
		else
		{
			Parent::SetValue((float)pValue / 10.0f);
		}
	}*/
	else if (mMin < mMax)
	{
		// Scale to +-1.
		Parent::SetValue((pValue*2.0f-(mMax+mMin)) / (float)(mMax-mMin));
	}
}

str Win32InputElement::GetCalibration() const
{
	str lData;
	lData += strutil::IntToString(mMin, 10);
	lData += _T(", ");
	lData += strutil::IntToString(mMax, 10);
	return (lData);
}

bool Win32InputElement::SetCalibration(const str& pData)
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



LOG_CLASS_DEFINE(UI_INPUT, Win32InputElement);



/*
	class Win32InputDevice
*/

Win32InputDevice::Win32InputDevice(LPDIRECTINPUTDEVICE8 pDIDevice, LPCDIDEVICEINSTANCE pInfo, InputManager* pManager):
	InputDevice(pManager),
	mDIDevice(pDIDevice),
	mRelAxisCount(0),
	mAbsAxisCount(0),
	mAnalogueCount(0),
	mButtonCount(0)
{
	SetIdentifier(pInfo->tszInstanceName);

	mDIDevice->EnumObjects(EnumElementsCallback, this, DIDFT_ALL);

	int lNumElements = (int)mElementArray.size();

	// Input data to use in buffered mode.
	mDeviceObjectData = new DIDEVICEOBJECTDATA[lNumElements];
	memset(mDeviceObjectData, 0, lNumElements * sizeof(DIDEVICEOBJECTDATA));

	// Create the DirectInput data format description.
	memset(&mDataFormat, 0, sizeof(mDataFormat));
	mDataFormat.dwSize     = sizeof(mDataFormat);
	mDataFormat.dwObjSize  = sizeof(DIOBJECTDATAFORMAT);
	mDataFormat.dwDataSize = lNumElements * sizeof(unsigned);
	mDataFormat.dwNumObjs  = lNumElements;
	mDataFormat.rgodf      = new DIOBJECTDATAFORMAT[lNumElements];

	mDataFormat.dwFlags = 0;
	if (mAbsAxisCount > 0)
	{
		mDataFormat.dwFlags |= DIDF_ABSAXIS;
	}
	if (mRelAxisCount > 0)
	{
		mDataFormat.dwFlags |= DIDF_RELAXIS;
	}

	memset(mDataFormat.rgodf, 0, mDataFormat.dwObjSize * lNumElements);

	// Fill the data format description with the correct data.
	ElementArray::iterator lIter;
	int i;
	for (i = 0, lIter = mElementArray.begin(); 
		lIter != mElementArray.end(); 
		++i, ++lIter)
	{
		mDeviceObjectData[i].dwOfs = i * sizeof(unsigned);

		Win32InputElement* lElement = (Win32InputElement*)(*lIter);

		memcpy(&mDataFormat.rgodf[i], lElement->GetDataFormat(), sizeof(DIOBJECTDATAFORMAT));
	}
}

Win32InputDevice::~Win32InputDevice()
{
	if (IsActive() == true)
	{
		mDIDevice->Unacquire();
	}
	mDIDevice->Release();

	ElementArray::iterator lEIter;
	for (lEIter = mElementArray.begin(); lEIter != mElementArray.end(); ++lEIter)
	{
		InputElement* lElement = *lEIter;
		delete lElement;
	}
	mElementArray.clear();

	delete[] mDeviceObjectData;
	delete[] mDataFormat.rgodf;
}

BOOL CALLBACK Win32InputDevice::EnumElementsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
	Win32InputDevice* lDevice = (Win32InputDevice*)pvRef;

	Win32InputElement* lElement = 0;
	// Is this an analogue or digital element?
	if ((lpddoi->dwType & DIDFT_ABSAXIS) != 0 ||
		(lpddoi->dwType & DIDFT_AXIS)    != 0 ||
		(lpddoi->dwType & DIDFT_POV)     != 0 ||
		(lpddoi->dwType & DIDFT_RELAXIS) != 0)
	{
		InputElement::Interpretation lInterpretation = InputElement::ABSOLUTE_AXIS;
		// Count number of relative and absolute axes.
		// These values are used later on in the constructor to determine
		// the data format.
		if ((lpddoi->dwType & DIDFT_RELAXIS) != 0)
		{
			++lDevice->mRelAxisCount;
			lInterpretation = InputElement::RELATIVE_AXIS;
		}
		else if ((lpddoi->dwType & DIDFT_ABSAXIS) != 0)
		{
			lDevice->mAbsAxisCount++;
		}

		lElement = new Win32InputElement(InputElement::ANALOGUE, lInterpretation, lDevice->mAnalogueCount,
			lDevice, lpddoi, (unsigned)lDevice->mElementArray.size() * sizeof(unsigned));
		++lDevice->mAnalogueCount;

		// Set absolute axis range.
		if ((lpddoi->dwType & DIDFT_ABSAXIS) != 0)
		{
			log_volatile(mLog.Debugf(_T("Found absolute axis element '%s' = '%s'."),
				lElement->GetFullName().c_str(),
				lElement->GetIdentifier().c_str()));

			DIPROPRANGE lRange;
			lRange.diph.dwSize = sizeof(DIPROPRANGE);
			lRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			lRange.diph.dwHow = DIPH_BYID;
			lRange.diph.dwObj = lpddoi->dwType;
			if (lDevice->mDIDevice->GetProperty(DIPROP_RANGE, &lRange.diph) == DI_OK)
			{
				const int lIntervalRange = lRange.lMax-lRange.lMin;
				const int lMid = lIntervalRange / 2 + lRange.lMin;
				const int lMin = lMid - lIntervalRange/2/8;	// Don't use full range, might not be physically accessible.
				const int lMax = lMid + lIntervalRange/2/8;	// Don't use full range, might not be physically accessible.
				lElement->SetValue(lMin);
				lElement->SetValue(lMax);
				lElement->SetValue(lMid);
			}
		}
	}
	else if((lpddoi->dwType&DIDFT_BUTTON)    != 0 ||
			(lpddoi->dwType&DIDFT_PSHBUTTON) != 0 ||
			(lpddoi->dwType&DIDFT_TGLBUTTON) != 0)
	{
		InputElement::Interpretation lInterpretation = (InputElement::Interpretation)((int)InputElement::BUTTON1 + lDevice->mButtonCount);
		lElement = new Win32InputElement(InputElement::DIGITAL, lInterpretation,
			lDevice->mButtonCount, lDevice, lpddoi, (unsigned)lDevice->mElementArray.size() * sizeof(unsigned));
		++lDevice->mButtonCount;
	}
	else if(lpddoi->dwType&DIDFT_FFACTUATOR)
	{
		// TODO: handle force feedback elements!
	}

	if (lElement)
	{
		lElement->SetIdentifier(lpddoi->tszName);
		lDevice->mElementArray.push_back(lElement);
	}

	return (DIENUM_CONTINUE);
}

void Win32InputDevice::Activate()
{
	if (IsActive() == false)
	{
		HRESULT lHR;
		DWORD lCooperativeFlags = 0;
		if (this == GetManager()->GetKeyboard())
		{
			lCooperativeFlags |= DISCL_NOWINKEY;
		}
		lHR = mDIDevice->SetCooperativeLevel(((Win32InputManager*)GetManager())->GetDisplayManager()->GetHWND(), DISCL_NONEXCLUSIVE|DISCL_FOREGROUND|lCooperativeFlags);
		lHR = mDIDevice->SetDataFormat(&mDataFormat);

		DIPROPDWORD lProp;
		lProp.diph.dwSize = sizeof(DIPROPDWORD);
		lProp.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		lProp.diph.dwHow = DIPH_DEVICE;
		lProp.diph.dwObj = 0;
		lProp.dwData = 1024;
		mDIDevice->SetProperty(DIPROP_BUFFERSIZE, &lProp.diph);

		mReacquire = true;
		SetActive(true);
	}
}

void Win32InputDevice::Release()
{
	mDIDevice->Unacquire();
	SetActive(false);
}

void Win32InputDevice::PollEvents()
{
	if (IsActive() == true)
	{
		if (mReacquire)
		{
			if (mDIDevice->Acquire() != DI_OK)
			{
				// System won't let us in yet. Keep trying.
				return;
			}
			mReacquire = false;
			log_debug(GetIdentifier()+_T(": acquired input device."));
		}


		HRESULT lHR = mDIDevice->Poll();
		if (lHR == DIERR_INPUTLOST)
		{
			mReacquire = true;
			log_debug(GetIdentifier()+_T(": lost input device."));
			return;
		}
		else if (lHR != DI_OK && lHR != DI_NOEFFECT)
		{
			mReacquire = true;
			mLog.Warningf((GetIdentifier() + _T(": Failed reaquiring device. Error=0x%8.8X.")).c_str(), lHR);
			return;
		}

		bool lMore = true;
		while (lMore)
		{
			DWORD lInOut = (DWORD)mElementArray.size();
			lHR = mDIDevice->GetDeviceData(sizeof(mDeviceObjectData[0]), mDeviceObjectData, &lInOut, 0);
			lMore = (lHR == DI_OK);

			for (unsigned i = 0; lMore && i < lInOut; i++)
			{
				// The following is a hack. I don't know if it works as 
				// intended on non-Swedish keyboards. The issue is 
				// that when pressing the right Alt key (Alt Gr), we will 
				// receive one Ctrl-event, and then one "Right Alt"-event
				// at the same time (on Swedish keyboards at least).
				if (i + 1 < lInOut && 
					mDeviceObjectData[i].dwTimeStamp == 
					mDeviceObjectData[i + 1].dwTimeStamp &&
					mDeviceObjectData[i + 1].dwOfs == 400)	// Right Alt at offset 400.
				{
					i++;
				}

				int lElementIndex = mDeviceObjectData[i].dwOfs / sizeof(unsigned);
				Win32InputElement* lElement = (Win32InputElement*)mElementArray[lElementIndex];

				if (lElement->GetType() == InputElement::ANALOGUE)
				{
					int lValue = mDeviceObjectData[i].dwData;
					lElement->SetValue(lValue);
				}
				else
				{
					int lValue = mDeviceObjectData[i].dwData;
					SetElementValue(lElement, (lValue&0x80)? 1.0f : 0.0f);
				}
			}

			if (lInOut == 0)
			{
				lMore = false;
			}
		}
	}
}

bool Win32InputDevice::HaveRelativeAxes()
{
	return (mRelAxisCount > mAbsAxisCount);
}



LOG_CLASS_DEFINE(UI_INPUT, Win32InputDevice);



Win32InputManager::Win32InputManager(Win32DisplayManager* pDisplayManager):
	mDisplayManager(pDisplayManager),
	mDirectInput(0),
	mEnumError(false),
	mInitialized(false),
	mScreenWidth(0),
	mScreenHeight(0),
	mCursorX(0),
	mCursorY(0),
	mMouse(0),
	mKeyboard(0)
{
	POINT lPoint;
	::GetCursorPos(&lPoint);
	SetMousePosition(WM_NCMOUSEMOVE, lPoint.x, lPoint.y);

	::memset(&mTypeCount, 0, sizeof(mTypeCount));

	HRESULT lHR;
	
	// Create the DirectInput object.
	lHR = DirectInput8Create(Win32Core::GetAppInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(LPVOID*)&mDirectInput, 0);

	if (lHR != DI_OK)
	{
		mDisplayManager->ShowMessageBox(_T("DirectX 8 not supported (dinput creation failure)!"), _T("DirectInput error!"));
		return;
	}

	// Enumerate all devices.
	lHR = mDirectInput->EnumDevices(DI8DEVCLASS_ALL, EnumDeviceCallback, this, DIEDFL_ALLDEVICES);

	// mEnumError will be set if an error has occured.
	if (lHR != DI_OK || mEnumError == true)
	{
		mDisplayManager->ShowMessageBox(_T("DirectInput failed enumerating your devices!"), _T("DirectInput error!"));
		return;
	}

	Refresh();

	AddObserver();

	mInitialized = true;
}

Win32InputManager::~Win32InputManager()
{
	if (mInitialized == true)
	{
		mDirectInput->Release();
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

void Win32InputManager::Refresh()
{
	if (mDisplayManager != 0 && mDisplayManager->GetHWND() != 0)
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
	}
}

Win32DisplayManager* Win32InputManager::GetDisplayManager() const
{
	return (mDisplayManager);
}

bool Win32InputManager::OnMessage(int pMsg, int pwParam, long plParam)
{
	bool lConsumed = false;
	switch (pMsg)
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
	}
	return (lConsumed);
}

void Win32InputManager::SetKey(KeyCode pWParam, long pLParam, bool pIsDown)
{
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
}

BOOL CALLBACK Win32InputManager::EnumDeviceCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	Win32InputManager* lInputManager = (Win32InputManager*)pvRef;

	HRESULT lHR;
	LPDIRECTINPUTDEVICE8 lDIDevice;
	
	lHR = lInputManager->mDirectInput->CreateDevice(lpddi->guidInstance, &lDIDevice, 0);

	if (lHR != DI_OK)
	{
		lInputManager->mEnumError = true;
		return DIENUM_STOP;
	}

	Win32InputDevice* lDevice = new Win32InputDevice(lDIDevice, lpddi, lInputManager);
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
}

void Win32InputManager::ShowCursor()
{
	::ShowCursor(TRUE);
}

void Win32InputManager::HideCursor()
{
	::ShowCursor(FALSE);
}

float Win32InputManager::GetCursorX()
{
	return mCursorX;
}

float Win32InputManager::GetCursorY()
{
	return mCursorY;
}

const InputDevice* Win32InputManager::GetKeyboard() const
{
	return mKeyboard;
}

InputDevice* Win32InputManager::GetKeyboard()
{
	return mKeyboard;
}

const InputDevice* Win32InputManager::GetMouse() const
{
	return mMouse;
}

InputDevice* Win32InputManager::GetMouse()
{
	return mMouse;
}

void Win32InputManager::AddObserver()
{
	if (mDisplayManager)
	{
		// Listen to text input and standard mouse events.
		mDisplayManager->AddObserver(WM_CHAR, this);
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
		mDisplayManager->AddObserver(WM_MBUTTONUP, this);
	}
}

void Win32InputManager::RemoveObserver()
{
	if (mDisplayManager)
	{
		mDisplayManager->RemoveObserver(this);
	}
}

void Win32InputManager::SetMousePosition(int pMsg, int x, int y)
{
	if (pMsg == WM_NCMOUSEMOVE && mDisplayManager)
	{
		POINT lPoint;
		lPoint.x = x;
		lPoint.y = y;
		::ScreenToClient(mDisplayManager->GetHWND(), &lPoint);
		x = lPoint.x;
		y = lPoint.y;
	}

	mCursorX = 2.0f * (float)x / (float)mScreenWidth  - 1.0f;
	mCursorY = 2.0f * (float)y / (float)mScreenHeight - 1.0f;
}

bool Win32InputManager::IsInitialized()
{
	return mInitialized;
}



}
