
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Include/Win32/UiWin32DisplayManager.h"
#include "../../../Lepra/Include/Log.h"
#include "../../../Lepra/Include/String.h"
#include "../../../Lepra/Include/SystemManager.h"
#include "../../Include/UiLepra.h"
#include "../../Include/Win32/UiWin32DirectXDisplay.h"
#include "../../Include/Win32/UiWin32OpenGLDisplay.h"

#define IDI_MAIN_ICON 1001	// This must be identical to resource definition in the .rc file.



namespace UiLepra
{



DisplayManager* DisplayManager::CreateDisplayManager(ContextType pCT)
{
	DisplayManager* lDisplayManager = 0;
	switch(pCT)
	{
		case DisplayManager::OPENGL_CONTEXT:	lDisplayManager = new Win32OpenGLDisplay;				break;
		case DisplayManager::DIRECTX_CONTEXT:	lDisplayManager = new Win32DirectXDisplay;				break;
		default:				mLog.AError("Invalid context type in CreateDisplayManager().");		break;
	}
	return (lDisplayManager);
}

void DisplayManager::EnableScreensaver(bool pEnable)
{
	BOOL lOldValue;
	::SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, pEnable, &lOldValue, 0);
}



Win32DisplayManager::Win32DisplayManager() :
	mWnd(0),
	mInitialized(false),
	mScreenOpened(false),
	mMinimized(false),
	mMaximized(false),
	mNormalWidth(0),
	mNormalHeight(0),
	mCaptionSet(false),
	mConsumeChar(true)
{
	// Count display modes.
	DEVMODE lDevMode;
	while (EnumDisplaySettings(NULL, mEnumeratedDisplayModeCount, &lDevMode) != 0)
	{
		mEnumeratedDisplayModeCount++;
	}
	mEnumeratedDisplayMode = new DisplayMode[mEnumeratedDisplayModeCount];

	int lCount = 0;
	while (EnumDisplaySettings(NULL, lCount, &lDevMode) != 0)
	{
		mEnumeratedDisplayMode[lCount].mWidth = lDevMode.dmPelsWidth;
		mEnumeratedDisplayMode[lCount].mHeight = lDevMode.dmPelsHeight;
		mEnumeratedDisplayMode[lCount].mBitDepth = lDevMode.dmBitsPerPel;
		mEnumeratedDisplayMode[lCount].mRefreshRate = lDevMode.dmDisplayFrequency;

		lCount++;
	}

	Register();
}

Win32DisplayManager::~Win32DisplayManager()
{
	CloseScreen();

	Unregister();

	ObserverSetTable::Iterator lTIter;
	for (lTIter = mObserverSetTable.First();
		lTIter != mObserverSetTable.End();
		++lTIter)
	{
		ObserverSet* lSet = *lTIter;
		delete (lSet);
	}
}

void Win32DisplayManager::SetFocus(bool pFocus)
{
	if (pFocus)
	{
		//::BringWindowToTop(mWnd);
		//::SwitchToThisWindow(mWnd, FALSE);
		::SetWindowPos(mWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
		::SetWindowPos(mWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
		::SetForegroundWindow(mWnd);
		//HWND_NOTOPMOST
	}
}

bool Win32DisplayManager::Register() 
{
	bool lOk = true;
	if (msRegisterCount == 0)
	{
		++msRegisterCount;

		mshThisInstance = Win32Core::GetAppInstance();

		// Register window class.
		msWindowClass.cbSize		= sizeof(msWindowClass);
		msWindowClass.cbClsExtra	= 0;
		msWindowClass.cbWndExtra	= 0;
		msWindowClass.hbrBackground	= GetStockBrush(BLACK_BRUSH);
		msWindowClass.style		= CS_DBLCLKS;
		msWindowClass.lpfnWndProc	= WndProc;
		msWindowClass.hInstance		= (HINSTANCE)mshThisInstance;
		msWindowClass.hIcon		= ::LoadIcon((HINSTANCE)mshThisInstance, MAKEINTRESOURCE(IDI_MAIN_ICON));
		msWindowClass.hIconSm		= ::LoadIcon((HINSTANCE)mshThisInstance, MAKEINTRESOURCE(IDI_MAIN_ICON));
		msWindowClass.hCursor		= ::LoadCursor(0, IDC_ARROW);
		msWindowClass.lpszMenuName	= NULL;
		msWindowClass.lpszClassName	= _T("LepraWin32Class");

		lOk = (RegisterClassEx(&msWindowClass) != 0);
	}
	if (lOk) 
	{
		mDisplayMode.mWidth	= 0;
		mDisplayMode.mHeight	= 0;
		mDisplayMode.mBitDepth	= 0;
		mInitialized		= true;
	}
	else
	{
		mLog.AError("Register() - failed to register window class.");
	}
	return lOk;
}

void Win32DisplayManager::Unregister()
{
	--msRegisterCount;
	if (msRegisterCount == 0)
	{
		::UnregisterClass(_T("LepraWin32Class"), (HINSTANCE)mshThisInstance);
	}
}

unsigned Win32DisplayManager::GetWidth() const
{
	return mDisplayMode.mWidth;
}

unsigned Win32DisplayManager::GetHeight() const
{
	return mDisplayMode.mHeight;
}

unsigned Win32DisplayManager::GetBitDepth() const
{
	return mDisplayMode.mBitDepth;
}

unsigned Win32DisplayManager::GetRefreshRate() const
{
	return mDisplayMode.mRefreshRate;
}

bool Win32DisplayManager::IsFullScreen() const
{
	return (mScreenMode == DisplayManager::FULLSCREEN);
}

double Win32DisplayManager::GetPhysicalScreenSize() const
{
	return 23.000;	// Estimated average user's screen size at time of playing any one of my games. Extremely accurate.
}

void Win32DisplayManager::SetCaption(const str& pCaption)
{
	SetCaption(pCaption, false);
}

void Win32DisplayManager::SetCaption(const str& pCaption, bool pInternalCall)
{
	if (mInitialized)
	{
		if (pInternalCall == false)
		{
			mCaptionSet = true;
		}

		if (pInternalCall == false || mCaptionSet == false)
		{
			::SetWindowText(mWnd, pCaption.c_str());
		}
	}
}

bool Win32DisplayManager::OpenScreen(const DisplayMode& pDisplayMode, ScreenMode pScreenMode, Orientation pOrientation)
{
	bool lOk = true;

	if (mInitialized == false)
	{
		mLog.AWarning("OpenScreen() - DisplayManager not initialized.");
		lOk = false;
	}
	else if(mScreenOpened == true)
	{
		mLog.AWarning("OpenScreen() - Screen already opened.");
		lOk = false;
	}
	else if(pDisplayMode.IsValid() == false && pScreenMode == FULLSCREEN)
	{
		mLog.AError("OpenScreen() - Invalid display mode.");
		lOk = false;
	}

	if (lOk)
	{
		mScreenMode = pScreenMode;
		mOrientation = pOrientation;

		if (mScreenMode == DisplayManager::FULLSCREEN)
		{
			bool lSupportedMode = false;

			// We can't trust that the user actually knows what he or she is doing, 
			// so check if the mode is actually supported.
			if (pDisplayMode.mBitDepth == 0)
			{
				lSupportedMode = FindDisplayMode(mDisplayMode,
								   pDisplayMode.mWidth, 
								   pDisplayMode.mHeight);
			}
			else if(pDisplayMode.mRefreshRate == 0)
			{
				lSupportedMode = FindDisplayMode(mDisplayMode,
								   pDisplayMode.mWidth, 
								   pDisplayMode.mHeight, 
								   pDisplayMode.mBitDepth);
			}
			else
			{
				lSupportedMode = FindDisplayMode(mDisplayMode,
								   pDisplayMode.mWidth, 
								   pDisplayMode.mHeight, 
								   pDisplayMode.mBitDepth,
								   pDisplayMode.mRefreshRate);
			}

			if (lSupportedMode == false)
			{
				str lErr(strutil::Format(_T("OpenScreen() - Display mode %i-bit %ix%i at %i Hz is not supported!"),
						 pDisplayMode.mBitDepth, 
						 pDisplayMode.mWidth, 
						 pDisplayMode.mHeight, 
						 pDisplayMode.mRefreshRate));

				mLog.Error(lErr);
				lOk = false;
			}
		}
		else
		{
			mDisplayMode = pDisplayMode;
		}
	}

	if (lOk)
	{
		lOk = InitWindow();
	}

	if (lOk)
	{
		mScreenOpened = InitScreen();

		if (mScreenOpened == true)
		{
			AddObserver(WM_SIZE, this);
			AddObserver(WM_SIZING, this);
		}
		else
		{
			lOk = false;
		}
	}

	return lOk;
}

void Win32DisplayManager::CloseScreen()
{
	if (mScreenOpened)
	{
		mScreenOpened = false;
		Win32Core::RemoveDisplayManager(this);
		RemoveObserver(this);

		::DestroyWindow(mWnd);
		mWnd = 0;

		// Repaint the desktop to restore background.
		RECT lRect;
		::GetWindowRect(GetDesktopWindow(), &lRect);
		::MoveWindow(GetDesktopWindow(), 
			lRect.left, 
			lRect.top, 
			lRect.right - lRect.left, 
			lRect.bottom - lRect.top, 
			TRUE);
	}
}

bool Win32DisplayManager::IsVisible() const
{
	return (!IsMinimized());
}

bool Win32DisplayManager::InitWindow()
{
	bool lOk = mInitialized;

	if (lOk)
	{
		if (mDisplayMode.mWidth == 0 || mDisplayMode.mHeight == 0) 
		{
			mDisplayMode.mWidth  = GetSystemMetrics(SM_CXSCREEN);
			mDisplayMode.mHeight = GetSystemMetrics(SM_CYSCREEN);
		}

		int lWindowWidth  = mDisplayMode.mWidth;
		int lWindowHeight = mDisplayMode.mHeight;

		// Create window
		if (mWnd == 0)
		{
			switch(mScreenMode)
			{
				case DisplayManager::FULLSCREEN:
				case DisplayManager::SPLASH_WINDOW:
				{
					mWnd = ::CreateWindowEx(WS_EX_APPWINDOW | WS_EX_TOPMOST,
								_T("LepraWin32Class"), _T("Lepra"),
								WS_POPUP | WS_CLIPSIBLINGS | WS_VISIBLE,
								GetSystemMetrics(SM_CXSCREEN) / 2 - lWindowWidth / 2,
								GetSystemMetrics(SM_CYSCREEN) / 2 - lWindowHeight / 2,
								lWindowWidth, lWindowHeight,
								GetDesktopWindow(), NULL, (HINSTANCE)mshThisInstance, NULL);
				}
				break;
				case DisplayManager::WINDOWED:
				case DisplayManager::STATIC_WINDOW:
				{
					lWindowWidth = GetWindowWidth(lWindowWidth);
					lWindowHeight = GetWindowHeight(lWindowHeight);
					DWORD lStyle = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;
					if (mScreenMode == DisplayManager::WINDOWED)
					{
						lStyle |= (WS_SIZEBOX | WS_MAXIMIZEBOX);
					}
					mWnd = ::CreateWindowEx(WS_EX_APPWINDOW,
						_T("LepraWin32Class"), _T("Lepra"),
						lStyle,
						GetSystemMetrics(SM_CXSCREEN) / 2 - lWindowHeight / 2,
						GetSystemMetrics(SM_CYSCREEN) / 2 - lWindowHeight / 2,
						lWindowWidth, lWindowHeight,
						NULL, NULL, (HINSTANCE)mshThisInstance, NULL);
				}
				break;
				default:
				break;
			}

			::ShowWindow(mWnd, SW_SHOW);
			::UpdateWindow(mWnd);
			mMinimized = false;
			mMaximized = false;
		}

		lOk = (mWnd != 0);

		if (lOk)
		{
			++msWindowCount;
		}
		else
		{
			mLog.AError("InitWindow() - Failed to create window.");
		}
	}

	if (lOk)
	{
		Win32Core::AddDisplayManager(this);
	}

	return lOk;
}

void Win32DisplayManager::GetBorderSize(int& pSizeX, int& pSizeY)
{
	if (mScreenMode == FULLSCREEN ||
	   mScreenMode == SPLASH_WINDOW)
	{
		pSizeX = 0;
		pSizeY = 0;
	}
	else if(mWnd != 0)
	{
		// Use the safest way there is... Taking the difference between
		// the size of the window and the size of the client area.
		// These numbers can't lie.
		RECT lClientRect;
		RECT lWindowRect;
		::GetClientRect(mWnd, &lClientRect);
		::GetWindowRect(mWnd, &lWindowRect);

		pSizeX = ((lWindowRect.right - lWindowRect.left) -
					(lClientRect.right - lClientRect.left));

		pSizeY = ((lWindowRect.bottom - lWindowRect.top) -
					(lClientRect.bottom - lClientRect.top));
	}
	else
	{
		// We have to use the awful system function GetSystemMetrics().
		// I don't trust that function at all, or any of the values it returns.
		if (mScreenMode == WINDOWED)
		{
			pSizeX = ::GetSystemMetrics(SM_CXSIZEFRAME) * 2;
			pSizeY = ::GetSystemMetrics(SM_CYSIZEFRAME) * 2 +
					   ::GetSystemMetrics(SM_CYCAPTION);
		}
		else
		{
			// Static window.
			pSizeX = ::GetSystemMetrics(SM_CXFIXEDFRAME) * 2;
			pSizeY = ::GetSystemMetrics(SM_CYFIXEDFRAME) * 2 +
					   ::GetSystemMetrics(SM_CYCAPTION);
		}
	}
}

int Win32DisplayManager::GetWindowWidth(int pClientWidth)
{
	int lBorderSizeX;
	int lBorderSizeY;
	GetBorderSize(lBorderSizeX, lBorderSizeY);

	return pClientWidth + lBorderSizeX;
}

int Win32DisplayManager::GetWindowHeight(int pClientHeight)
{
	int lBorderSizeX;
	int lBorderSizeY;
	GetBorderSize(lBorderSizeX, lBorderSizeY);

	return pClientHeight + lBorderSizeY;
}

int Win32DisplayManager::GetClientWidth(int pWindowWidth)
{
	int lBorderSizeX;
	int lBorderSizeY;
	GetBorderSize(lBorderSizeX, lBorderSizeY);

	return pWindowWidth - lBorderSizeX;
}

int Win32DisplayManager::GetClientHeight(int pWindowHeight)
{
	int lBorderSizeX;
	int lBorderSizeY;
	GetBorderSize(lBorderSizeX, lBorderSizeY);

	return pWindowHeight - lBorderSizeY;
}

HWND Win32DisplayManager::GetHWND()
{
	return mWnd;
}

LRESULT CALLBACK Win32DisplayManager::WndProc(HWND pWnd, unsigned int pMessage, unsigned int pwParam, LONG plParam) 
{
	if (pMessage  == WM_QUIT ||
		pMessage == WM_DESTROY ||
		pMessage == WM_CLOSE)
	{
		if (msWindowCount > 0)
		{
			--msWindowCount;
		}
		if (msWindowCount == 0)
		{
			SystemManager::AddQuitRequest(+1);
			// First close attempt.
			if (pMessage == WM_CLOSE && SystemManager::GetQuitRequest() <= 1)
			{
				return (TRUE);
			}
		}
	}

	bool lMessageWasConsumed = false;
	Win32DisplayManager* lDisplayManager = Win32Core::GetDisplayManager(pWnd);
	if (lDisplayManager)
	{
		lMessageWasConsumed = lDisplayManager->InternalDispatchMessage(pMessage, pwParam, plParam);
	}
	LRESULT lResult = 0;
	if (!lMessageWasConsumed)
	{
		lResult = DefWindowProc(pWnd, pMessage, pwParam, plParam);
	}
	return (lResult);
}

bool Win32DisplayManager::InternalDispatchMessage(int pMessage, int pwParam, long plParam) 
{
	if (pMessage == WM_CHAR && mConsumeChar)	// Consume all follow-up WM_CHAR's when we already dispatched the WM_KEYDOWN.
	{
		return (true);
	}

	bool lConsumed = false;
	ObserverSetTable::Iterator lTIter = mObserverSetTable.Find(pMessage);
	if (lTIter != mObserverSetTable.End())
	{
		ObserverSet* lSet = *lTIter;
		ObserverSet::iterator lLIter;
		for (lLIter = lSet->begin(); lLIter != lSet->end(); ++lLIter)
		{
			lConsumed |= (*lLIter)->OnMessage(pMessage, pwParam, plParam);
		}
	}
	if (pMessage == WM_KEYDOWN)
	{
		mConsumeChar = lConsumed;
	}
	return (lConsumed);
}

void Win32DisplayManager::ProcessMessages()
{
	if (mWnd != 0)
	{
		MSG lMsg;
		while (::PeekMessage(&lMsg, mWnd, 0, 0, PM_REMOVE) == TRUE)
		{
			::TranslateMessage(&lMsg);
			::DispatchMessage(&lMsg);
		}
	}
}

void Win32DisplayManager::AddObserver(unsigned int pMessage, Win32Observer* pObserver)
{
	ObserverSetTable::Iterator lTIter = mObserverSetTable.Find(pMessage);
	ObserverSet* lSet = 0;

	if (lTIter == mObserverSetTable.End())
	{
		lSet = new ObserverSet;
		mObserverSetTable.Insert(pMessage, lSet);
	}
	else
	{
		lSet = *lTIter;
	}

	ObserverSet::iterator lLIter = lSet->find(pObserver);
	if (lLIter == lSet->end())
	{
		lSet->insert(pObserver);
	}
}

void Win32DisplayManager::RemoveObserver(unsigned int pMessage, Win32Observer* pObserver)
{
	ObserverSetTable::Iterator lTIter = mObserverSetTable.Find(pMessage);

	if (lTIter != mObserverSetTable.End())
	{
		ObserverSet* lSet = *lTIter;
		lSet->erase(pObserver);
		if (lSet->empty())
		{
			mObserverSetTable.Remove(lTIter);
			delete (lSet);
		}
	}
}

void Win32DisplayManager::RemoveObserver(Win32Observer* pObserver)
{
	ObserverSetTable::Iterator lTIter = mObserverSetTable.First();

	while (lTIter != mObserverSetTable.End())
	{
		ObserverSet* lSet = *lTIter;
		ObserverSet::iterator lLIter = lSet->find(pObserver);
		if (lLIter != lSet->end())
		{
			lSet->erase(lLIter);
			if (lSet->empty())
			{
				mObserverSetTable.Remove(lTIter++);
				delete (lSet);
			}
			else
			{
				++lTIter;
			}
		}
		else
		{
			++lTIter;
		}
	}
}

void Win32DisplayManager::ShowMessageBox(const str& pMsg, const str& pCaption)
{
	::MessageBox(mWnd, pMsg.c_str(), pCaption.c_str(), MB_OK);
}

bool Win32DisplayManager::OnMessage(int pMsg, int pwParam, long plParam)
{
	switch(pMsg)
	{
		case WM_SIZING:
		{
			LPRECT lRect = (LPRECT)(intptr_t)plParam;

			int lClientWidth  = GetClientWidth(lRect->right - lRect->left);
			int lClientHeight = GetClientHeight(lRect->bottom - lRect->top);

			DispatchResize(lClientWidth, lClientHeight);
			mMinimized = false;
			mMaximized = false;
		}
		break;
		case WM_SIZE:
		{
			switch(pwParam)
			{
				case SIZE_MINIMIZED:
				{
					DispatchMinimize();
					mMinimized = true;
					mMaximized = false;
				}
				break;
				case SIZE_MAXIMIZED:
				{
					mNormalWidth  = mDisplayMode.mWidth;
					mNormalHeight = mDisplayMode.mHeight;
					DispatchMaximize((int)LOWORD(plParam), (int)HIWORD(plParam));
					mMaximized = true;
					mMinimized = false;
				}
				break;
				case SIZE_RESTORED:
				{
					DispatchResize((int)LOWORD(plParam), (int)HIWORD(plParam));
					mMinimized = false;
					mMaximized = false;
				}
				break;
			}
		}
	}

	return (false);
}



int Win32DisplayManager::msWindowCount = 0;
int Win32DisplayManager::msRegisterCount = 0;
HANDLE Win32DisplayManager::mshThisInstance = 0;
WNDCLASSEX Win32DisplayManager::msWindowClass;
loginstance(UI_GFX, Win32DisplayManager);



}
