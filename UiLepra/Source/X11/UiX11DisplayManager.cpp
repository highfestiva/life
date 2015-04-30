
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Include/X11/UiX11DisplayManager.h"
#include "../../../Lepra/Include/Log.h"
#include "../../../Lepra/Include/String.h"
#include "../../../Lepra/Include/SystemManager.h"
#include "../../Include/UiLepra.h"
#include "../../Include/X11/UiX11OpenGLDisplay.h"



namespace UiLepra
{



DisplayManager* DisplayManager::CreateDisplayManager(ContextType pCT)
{
	DisplayManager* lDisplayManager = 0;
	switch(pCT)
	{
		case DisplayManager::OPENGL_CONTEXT:	lDisplayManager = new X11OpenGLDisplay;				break;
		default:				mLog.AError("Invalid context type in CreateDisplayManager().");	break;
	}
	return (lDisplayManager);
}

void DisplayManager::EnableScreensaver(bool /*pEnable*/)
{
	// TODO: implement!
}



X11DisplayManager::X11DisplayManager() :
	mDisplay(0),
	mWnd(0),
	mIsOpen(false),
	mMinimized(false),
	mMaximized(false),
	mNormalWidth(0),
	mNormalHeight(0),
	mCaptionSet(false),
	mConsumeChar(true)
{

	/* TODO: implement!

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
	}*/
}

X11DisplayManager::~X11DisplayManager()
{
	CloseScreen();

	delete[] mEnumeratedDisplayMode;
	mEnumeratedDisplayMode = 0;
	mEnumeratedDisplayModeCount = 0;

	ObserverSetTable::Iterator lTIter;
	for (lTIter = mObserverSetTable.First();
		lTIter != mObserverSetTable.End();
		++lTIter)
	{
		ObserverSet* lSet = *lTIter;
		delete (lSet);
	}
}

unsigned X11DisplayManager::GetWidth() const
{
	return mDisplayMode.mWidth;
}

unsigned X11DisplayManager::GetHeight() const
{
	return mDisplayMode.mHeight;
}

unsigned X11DisplayManager::GetBitDepth() const
{
	return mDisplayMode.mBitDepth;
}

unsigned X11DisplayManager::GetRefreshRate() const
{
	return mDisplayMode.mRefreshRate;
}

bool X11DisplayManager::IsFullScreen() const
{
	return (mScreenMode == DisplayManager::FULLSCREEN);
}

void X11DisplayManager::SetCaption(const str& pCaption)
{
	SetCaption(pCaption, false);
}

void X11DisplayManager::SetCaption(const str& pCaption, bool pInternalCall)
{
	if (mIsOpen)
	{
		if (pInternalCall == false)
		{
			mCaptionSet = true;
		}

		if (pInternalCall == false || mCaptionSet == false)
		{
			::XStoreName(GetDisplay(), GetWindow(), astrutil::Encode(pCaption).c_str());
		}
	}
}

bool X11DisplayManager::OpenScreen(const DisplayMode& pDisplayMode, ScreenMode pScreenMode, Orientation pOrientation)
{
	bool lOk = true;

	if(mIsOpen == true)
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
		lOk = mIsOpen = InitScreen();
		if (mIsOpen)
		{
			// TODO: fix.
			/*AddObserver(WM_SIZE, this);
			AddObserver(WM_SIZING, this);*/
		}
	}

	return lOk;
}

void X11DisplayManager::CloseScreen()
{
	if (mIsOpen)
	{
		mIsOpen = false;
		X11Core::RemoveDisplayManager(this);
		RemoveObserver(this);

		::XUnmapWindow(GetDisplay(), GetWindow());
		::XDestroyWindow(GetDisplay(), GetWindow());
		mWnd = 0;
		mDisplay = 0;
	}
}

bool X11DisplayManager::IsVisible() const
{
	return (!IsMinimized());
}

bool X11DisplayManager::IsFocused() const
{
	return (!IsMinimized());
}

void X11DisplayManager::HideWindow(bool pHide)
{
	::ShowWindow(mWnd, pHide? SW_HIDE : SW_SHOW);
}

bool X11DisplayManager::InitWindow()
{
	bool lOk = mIsOpen;

	/*if (lOk)
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
								_T("LepraX11Class"), _T("Lepra"),
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

					mWnd = ::CreateWindowEx(0, _T("LepraX11Class"), _T("Lepra"),
						lStyle,
						GetSystemMetrics(SM_CXSCREEN) / 2 - lWindowHeight / 2,
						GetSystemMetrics(SM_CYSCREEN) / 2 - lWindowHeight / 2,
						lWindowWidth, lWindowHeight,
						GetDesktopWindow(), NULL, (HINSTANCE)mshThisInstance, NULL);
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
	}*/

	if (lOk)
	{
		X11Core::AddDisplayManager(this);
	}

	return lOk;
}

void X11DisplayManager::GetBorderSize(int& pSizeX, int& pSizeY)
{
	if (mScreenMode == FULLSCREEN ||
	   mScreenMode == SPLASH_WINDOW)
	{
		pSizeX = 0;
		pSizeY = 0;
	}
	// TODO: implement!
	/*else if(mWnd != 0)
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
	}*/
}

int X11DisplayManager::GetWindowWidth(int pClientWidth)
{
	int lBorderSizeX;
	int lBorderSizeY;
	GetBorderSize(lBorderSizeX, lBorderSizeY);

	return pClientWidth + lBorderSizeX;
}

int X11DisplayManager::GetWindowHeight(int pClientHeight)
{
	int lBorderSizeX;
	int lBorderSizeY;
	GetBorderSize(lBorderSizeX, lBorderSizeY);

	return pClientHeight + lBorderSizeY;
}

int X11DisplayManager::GetClientWidth(int pWindowWidth)
{
	int lBorderSizeX;
	int lBorderSizeY;
	GetBorderSize(lBorderSizeX, lBorderSizeY);

	return pWindowWidth - lBorderSizeX;
}

int X11DisplayManager::GetClientHeight(int pWindowHeight)
{
	int lBorderSizeX;
	int lBorderSizeY;
	GetBorderSize(lBorderSizeX, lBorderSizeY);

	return pWindowHeight - lBorderSizeY;
}

Display* X11DisplayManager::GetDisplay() const
{
	return mDisplay;
}

Window X11DisplayManager::GetWindow() const
{
	return mWnd;
}

Bool X11DisplayManager::WaitForNotify(Display* d, XEvent* e, char* arg)
{
	/*if (pMessage  == WM_QUIT ||
		pMessage == WM_DESTROY)
	{
		--msWindowCount;
		if (msWindowCount == 0)
		{
			SystemManager::AddQuitRequest(+1);
		}
	}

	bool lMessageWasConsumed = false;
	X11DisplayManager* lDisplayManager = X11Core::GetDisplayManager(pWnd);
	if (lDisplayManager)
	{
		lMessageWasConsumed = lDisplayManager->InternalDispatchMessage(pMessage, pwParam, plParam);
	}
	LRESULT lResult = 0;
	if (!lMessageWasConsumed)
	{
		lResult = DefWindowProc(pWnd, pMessage, pwParam, plParam);
	}
	return (lResult);*/
	return((e->type == MapNotify) && (e->xmap.window == (::Window)arg));
}

Bool X11DisplayManager::InternalDispatchMessage(XEvent* e)
{
	/*if (pMessage == WM_CHAR && mConsumeChar)
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
	return (lConsumed);*/
	return (0);
}

void X11DisplayManager::ProcessMessages()
{
	if (mWnd != 0)
	{
		/*MSG lMsg;
		while (::PeekMessage(&lMsg, mWnd, 0, 0, PM_REMOVE) == TRUE)
		{
			::TranslateMessage(&lMsg);
			::DispatchMessage(&lMsg);
		}*/
	}
}

void X11DisplayManager::AddObserver(Window pMessage, X11Observer* pObserver)
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

void X11DisplayManager::RemoveObserver(Window pMessage, X11Observer* pObserver)
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

void X11DisplayManager::RemoveObserver(X11Observer* pObserver)
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

void X11DisplayManager::ShowMessageBox(const str& pMsg, const str& pCaption)
{
	//::MessageBox(mWnd, pMsg.c_str(), pCaption.c_str(), MB_OK);
}

Bool X11DisplayManager::OnMessage(XEvent* e)
{
	/*switch(pMsg)
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
				} break;
				case SIZE_MAXIMIZED:
				{
					mNormalWidth  = mDisplayMode.mWidth;
					mNormalHeight = mDisplayMode.mHeight;
					DispatchMaximize((int)LOWORD(plParam), (int)HIWORD(plParam));
					mMaximized = true;
				} break;
				case SIZE_RESTORED:
				{
					int lWindowWidth;
					int lWindowHeight;

					if (mMaximized == true)
					{
						lWindowWidth  = GetWindowWidth(mNormalWidth);
						lWindowHeight = GetWindowHeight(mNormalHeight);
					}
					else
					{
						lWindowWidth  = GetWindowWidth(mDisplayMode.mWidth);
						lWindowHeight = GetWindowHeight(mDisplayMode.mHeight);
					}

					DispatchResize((int)LOWORD(plParam), (int)HIWORD(plParam));

					mMinimized = false;
					mMaximized = false;
				} break;
			}
		}
		break;
	}*/
	return (0);
}



int X11DisplayManager::msWindowCount = 0;
loginstance(UI_GFX, X11DisplayManager);



}
