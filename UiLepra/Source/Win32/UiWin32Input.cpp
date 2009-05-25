/*
	Class:  Win32InputElement, Win32InputDevice, Win32InputManager
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "../../../Lepra/Include/Log.h"
#include "../../Include/Win32/UiWin32Input.h"
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

Win32InputElement::Win32InputElement(InputElement::Type pType, Win32InputDevice* pParentDevice,
	LPCDIDEVICEOBJECTINSTANCE pElement, unsigned pFieldOffset):
	InputElement(pType, pParentDevice),
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

	Win32InputDevice* lDevice = (Win32InputDevice*)GetParentDevice();
	Win32InputManager* lManager = (Win32InputManager*)lDevice->GetManager();

//	if ((mElement->dwType & DIDFT_RELAXIS) != 0)
	if (lDevice->HaveRelativeAxes() == true)
	{
		// Treat this as a relative axis. Since we don't know the maximum value
		// of this axis (can probably be infinitly large), we need to scale it down
		// to some kind of common unit. The mouse x- and y-axes fall into this 
		// category, and the mouse is the primary device that we should take into 
		// consideration when finding the appropriate scale factor.
		//
		// To give the relative mouse coordinates the same unit as for absolute ones,
		// we will divide the value by half the screen width and height.

		if (GetInterpretation() == MOUSE_WHEEL)
		{
			InputElement::SetValue((double)pValue / 1000.0);
		}
		else
		{
			InputElement::SetValue(2.0 * (double)pValue / (double)lManager->mScreenWidth);
		}

		if (GetParentDevice() == lManager->GetMouse())
		{
			// Update the mouse delta coordinates in the input manager.
			if (GetInterpretation() == Y_AXIS)
			{
				lManager->mMouseDY = GetValue();
			}
			else if(GetInterpretation() == MOUSE_WHEEL)
			{
				lManager->mMouseDWheel = GetValue();
			}
			else
			{
				lManager->mMouseDX = GetValue();
			}
		}
	}
	else
	{
		double lPrevValue = GetValue();

		// Treat this as an absolute axis, centered about 0.
		if (pValue > 0)
		{
			InputElement::SetValue((double)pValue / (double)mMax);
		}
		else if(pValue < 0)
		{
			InputElement::SetValue(-(double)pValue / (double)mMin);
		}

		if (GetParentDevice() == lManager->GetMouse())
		{
			// Update the mouse delta coordinates in the input manager.
			if (GetInterpretation() == Y_AXIS)
			{
				lManager->mMouseDY = GetValue() - lPrevValue;
			}
			else if(GetInterpretation() == MOUSE_WHEEL)
			{
				lManager->mMouseDWheel = GetValue() - lPrevValue;
			}
			else
			{
				lManager->mMouseDX = GetValue() - lPrevValue;
			}
		}
	}
}

unsigned Win32InputElement::GetCalibrationDataSize()
{
	return sizeof(mMin) + sizeof(mMax);
}

void Win32InputElement::GetCalibrationData(Lepra::uint8* pData)
{
	((unsigned*)pData)[0] = mMin;
	((unsigned*)pData)[1] = mMax;
}

void Win32InputElement::SetCalibrationData(const Lepra::uint8* pData)
{
	mMin = ((unsigned*)pData)[0];
	mMax = ((unsigned*)pData)[1];
}

/*
	class Win32InputDevice
*/

Win32InputDevice::Win32InputDevice(LPDIRECTINPUTDEVICE8 pDIDevice, LPCDIDEVICEINSTANCE pInfo, InputManager* pManager):
	InputDevice(pManager),
	mDIDevice(pDIDevice),
	mButtonCount(0),
	mRelAxisCount(0),
	mAbsAxisCount(0)
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

	if (mAbsAxisCount > mRelAxisCount)
	{
		mDataFormat.dwFlags    = DIDF_ABSAXIS;
	}
	else
	{
		mDataFormat.dwFlags    = DIDF_RELAXIS;
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

	delete[] mDeviceObjectData;
	delete[] mDataFormat.rgodf;
}

BOOL CALLBACK Win32InputDevice::EnumElementsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
	Win32InputDevice* lDevice = (Win32InputDevice*)pvRef;

	// Is this an analogue or digital element?
	if ((lpddoi->dwType & DIDFT_ABSAXIS) != 0 ||
		(lpddoi->dwType & DIDFT_AXIS)    != 0 ||
		(lpddoi->dwType & DIDFT_POV)     != 0 ||
		(lpddoi->dwType & DIDFT_RELAXIS) != 0)
	{
		// Count number of relative and absolute axes.
		// These values are used later on in the constructor to determine 
		// the data format.
		if ((lpddoi->dwType & DIDFT_RELAXIS) != 0)
		{
			lDevice->mRelAxisCount++;
		}
		if ((lpddoi->dwType & DIDFT_ABSAXIS) != 0)
		{
			lDevice->mAbsAxisCount++;
		}

		Win32InputElement* lElement = 
			new Win32InputElement(InputElement::ANALOGUE,
								  lDevice,
								  lpddoi,
								  (unsigned)lDevice->mElementArray.size() * sizeof(unsigned));

		lElement->SetIdentifier(lpddoi->tszName);
		lDevice->mElementArray.push_back(lElement);

		// Set the preferred interpretation of this element.
		if (lpddoi->guidType == GUID_XAxis ||
			lpddoi->guidType == GUID_RxAxis ||
			lpddoi->guidType == GUID_Slider)
		{
			lElement->SetInterpretation(InputElement::X_AXIS);
		}
		else if(lpddoi->guidType == GUID_YAxis ||
				lpddoi->guidType == GUID_RyAxis)
		{
			lElement->SetInterpretation(InputElement::Y_AXIS);
		}
		else if(lpddoi->guidType == GUID_ZAxis ||
				lpddoi->guidType == GUID_RzAxis)
		{
			if (lDevice->GetIdentifier() == _T("Mouse"))
			{
				// This is for sure a mouse wheel.
				lElement->SetInterpretation(InputElement::MOUSE_WHEEL);
			}
			else
			{
				lElement->SetInterpretation(InputElement::Z_AXIS);
			}
		}
	}
	else if((lpddoi->dwType&DIDFT_BUTTON)    != 0 ||
			(lpddoi->dwType&DIDFT_PSHBUTTON) != 0 ||
			(lpddoi->dwType&DIDFT_TGLBUTTON) != 0)
	{
		Win32InputElement* lElement = new Win32InputElement(InputElement::DIGITAL,
			lDevice, lpddoi, (unsigned)lDevice->mElementArray.size() * sizeof(unsigned));

		lElement->SetIdentifier(lpddoi->tszName);
		lDevice->mElementArray.push_back(lElement);

		// Set the preferred interpretation of this element.
		lElement->SetInterpretation(
			(InputElement::Interpretation)((int)InputElement::BUTTON1 + lDevice->mButtonCount));
		lDevice->mButtonCount++;
	}
	else if(lpddoi->dwType&DIDFT_FFACTUATOR)
	{
		// TODO: handle force feedback elements!
	}

	return DIENUM_CONTINUE;
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
		while (lMore == true)
		{
			DWORD lInOut = (DWORD)mElementArray.size();
			mDIDevice->GetDeviceData(sizeof(mDeviceObjectData[0]), mDeviceObjectData, &lInOut, 0);

			for (unsigned i = 0; i < lInOut; i++)
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
					SetElementValue(lElement, lValue == 0 ? 0.0 : 1.0);
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
	mMouseDX(0),
	mMouseDY(0),
	mCursorDX(0),
	mCursorDY(0),
	mCursorX(0),
	mCursorY(0),
	mPrevCursorX(0),
	mPrevCursorY(0),
	mMouse(0),
	mKeyboard(0)
{
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

	mMouseButton[0] = false;
	mMouseButton[1] = false;
	mMouseButton[2] = false;

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
	switch(pMsg)
	{
		case WM_CHAR:
		{
			lConsumed = InputManager::NotifyOnChar((Lepra::tchar)pwParam);
		} break;
		case WM_LBUTTONDBLCLK:
		{
			lConsumed = NotifyMouseDoubleClick();
		} break;
		case WM_LBUTTONDOWN:
		{
			mMouseButton[0] = true;
		} break;
		case WM_RBUTTONDOWN:
		{
			mMouseButton[1] = true;
		} break;
		case WM_MBUTTONDOWN:
		{
			mMouseButton[2] = true;
		} break;
		case WM_LBUTTONUP:
		{
			mMouseButton[0] = false;
		} break;
		case WM_RBUTTONUP:
		{
			mMouseButton[1] = false;
		} break;
		case WM_MBUTTONUP:
		{
			mMouseButton[2] = false;
		} break;
		case WM_MOUSEMOVE:
		{
			int lX = LOWORD(plParam);
			int lY = HIWORD(plParam);

			mPrevCursorX = mCursorX;
			mPrevCursorY = mCursorY;

			mCursorX = 2.0 * (double)lX / (double)mScreenWidth  - 1.0;
			mCursorY = 2.0 * (double)lY / (double)mScreenHeight - 1.0;

			mCursorX = mCursorX < -1.0 ? -1.0 : (mCursorX > 1.0 ? 1.0 : mCursorX);
			mCursorY = mCursorY < -1.0 ? -1.0 : (mCursorY > 1.0 ? 1.0 : mCursorY);

			mCursorDX = mCursorX - mPrevCursorX;
			mCursorDY = mCursorY - mPrevCursorY;
		} break;
		case WM_KEYUP:
		{
			SetKey((KeyCode)pwParam, plParam, false);
			lConsumed = NotifyOnKeyUp((KeyCode)pwParam);
		}break;
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
		} break;
		case WM_SYSKEYDOWN:
		{
			SetKey((KeyCode)pwParam, plParam, true);
			lConsumed = NotifyOnKeyDown((KeyCode)pwParam);
		} break;
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
	InputManager::SetKey(pWParam, pIsDown);
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

	// Create a device and add it to the list of devices.
	Win32InputDevice* lDevice = new Win32InputDevice(lDIDevice, lpddi, lInputManager);
	lInputManager->mDeviceList.push_back(lDevice);

	// Since we can't detect the mouse and keyboard by string comparisons 
	// due to the language of the OS, we have to make a qualified guess.
	if (lInputManager->mMouse == 0 &&
	   lDevice->GetNumAnalogueElements() >= 2 &&
	   lDevice->HaveRelativeAxes() == true &&
	   lDevice->GetNumDigitalElements() >= 1)
	{
		lInputManager->mMouse = lDevice;
	}

	if (lInputManager->mKeyboard == 0 &&
	   lDevice->GetNumDigitalElements() >= 70)
	{
		lInputManager->mKeyboard = lDevice;
	}

	return DIENUM_CONTINUE;
}

void Win32InputManager::PollEvents()
{
	// Reset mouse delta coordinates every frame.
	mMouseDX = 0;
	mMouseDY = 0;
	mMouseDWheel = 0;

	InputManager::PollEvents();
}

void Win32InputManager::ShowCursor()
{
	::ShowCursor(TRUE);
}

void Win32InputManager::HideCursor()
{
	::ShowCursor(FALSE);
}

bool Win32InputManager::GetMouseButtonState(unsigned pButtonIndex)
{
	if (pButtonIndex < 3)
	{
		return mMouseButton[pButtonIndex];
	}
	else
	{
		return false;
	}
}

double Win32InputManager::GetMouseDeltaX()
{
	return mMouseDX;
}

double Win32InputManager::GetMouseDeltaY()
{
	return mMouseDY;
}

double Win32InputManager::GetCursorDeltaX()
{
	return mCursorDX;
}

double Win32InputManager::GetCursorDeltaY()
{
	return mCursorDY;
}

double Win32InputManager::GetMouseDeltaUnit()
{
	return 2.0 / (double)mScreenWidth;
}

double Win32InputManager::GetMouseDeltaWheel()
{
	return mMouseDWheel;
}

double Win32InputManager::GetCursorX()
{
	return mCursorX;
}

double Win32InputManager::GetCursorY()
{
	return mCursorY;
}

void Win32InputManager::SetCursorX(double x)
{
	mCursorX = x < -1.0 ? -1.0 : (x > 1.0 ? 1.0 : x);

	int lX = (int)((mCursorX + 1.0) * 0.5 * (double)mScreenWidth);
	int lY = (int)((mCursorY + 1.0) * 0.5 * (double)mScreenHeight);

	RECT lWinRect;
	::GetWindowRect(mDisplayManager->GetHWND(), &lWinRect);
	::SetCursorPos(lWinRect.left + lX, lWinRect.top + lY);
}

void Win32InputManager::SetCursorY(double y)
{
	mCursorY = y < -1.0 ? -1.0 : (y > 1.0 ? 1.0 : y);

	int lX = (int)((mCursorX + 1.0) * 0.5 * (double)mScreenWidth);
	int lY = (int)((mCursorY + 1.0) * 0.5 * (double)mScreenHeight);

	RECT lWinRect;
	::GetWindowRect(mDisplayManager->GetHWND(), &lWinRect);
	::SetCursorPos(lWinRect.left + lX, lWinRect.top + lY);
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



}
