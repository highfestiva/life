
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

void DisplayManager::EnableScreensaver(bool pEnable)
{
	//BOOL lOldValue;
	//::SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, pEnable, &lOldValue, 0);
}



X11DisplayManager::X11DisplayManager() :
	mDisplay(0),
	mWnd(0),
	mIsScreenOpen(false),
	mIsHidden(true),
	mMinimized(false),
	mMaximized(false),
	mNormalWidth(0),
	mNormalHeight(0),
	mCaptionSet(false)
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

void X11DisplayManager::SetFocus(bool pFocus)
{
	if (pFocus)
	{
		::XSetInputFocus(mDisplay, pFocus, RevertToNone, CurrentTime);
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

double X11DisplayManager::GetPhysicalScreenSize() const
{
	return 23.000;	// Estimated average user's screen size at time of playing any one of my games. Extremely accurate.
}

void X11DisplayManager::SetCaption(const str& pCaption)
{
	SetCaption(pCaption, false);
}

void X11DisplayManager::SetCaption(const str& pCaption, bool pInternalCall)
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

bool X11DisplayManager::OpenScreen(const DisplayMode& pDisplayMode, ScreenMode pScreenMode, Orientation pOrientation)
{
	bool lOk = true;

	if (mIsScreenOpen)
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
		lOk = mIsScreenOpen = InitScreen();
		if (lOk)
		{
			AddObserver(ResizeRequest, this);
		}
	}

	return lOk;
}

void X11DisplayManager::CloseScreen()
{
	if (mIsScreenOpen)
	{
		mIsScreenOpen = false;
		X11Core::RemoveDisplayManager(this);
		RemoveObserver(this);

		HideWindow(true);
		::XDestroyWindow(mDisplay, mWnd);
		mWnd = 0;

		::XCloseDisplay(mDisplay);
		mDisplay = 0;
	}
}

bool X11DisplayManager::IsVisible() const
{
	return !IsMinimized() && !mIsHidden;
}

bool X11DisplayManager::IsFocused() const
{
	Window lFocused;
	int lRevertTo;
	::XGetInputFocus(mDisplay, &lFocused, &lRevertTo);
	return IsVisible() && (lFocused == mWnd);
}

void X11DisplayManager::HideWindow(bool pHide)
{
	if (mIsHidden == pHide)
	{
		return;
	}

	if (pHide)
	{
		::XUnmapWindow(mDisplay, mWnd);
	}
	else
	{
		::XMapWindow(mDisplay, mWnd);
	}
	mIsHidden = pHide;
}

bool X11DisplayManager::InitWindow()
{
	bool lOk = true;

	if (lOk && !mDisplay)
	{
		mDisplay = XOpenDisplay(0);
		if(!mDisplay)
		{
			mLog.AError("Display not opened. X started?");
			lOk = false;
		}
	}

	if (lOk && !mWnd)
	{
		XVisualInfo* lVisualInfo = GetVisualInfo();
		Screen* lScreen = DefaultScreenOfDisplay(mDisplay);
		const int lScreenWidth  = WidthOfScreen(lScreen);
		const int lScreenHeight = HeightOfScreen(lScreen);

		if (mDisplayMode.mWidth == 0 || mDisplayMode.mHeight == 0) 
		{
			mDisplayMode.mWidth  = lScreenWidth;
			mDisplayMode.mHeight = lScreenHeight;
		}

		int lWindowWidth  = mDisplayMode.mWidth;
		int lWindowHeight = mDisplayMode.mHeight;

		switch(mScreenMode)
		{
			case DisplayManager::WINDOWED:
			case DisplayManager::STATIC_WINDOW:
			{
				
				lWindowWidth = GetWindowWidth(lWindowWidth);
				lWindowHeight = GetWindowHeight(lWindowHeight);
			}
			// TRICKY: fall through!
			case DisplayManager::FULLSCREEN:
			case DisplayManager::SPLASH_WINDOW:
			{
				Colormap lColMap = ::XCreateColormap(mDisplay, RootWindow(mDisplay, lVisualInfo->screen), lVisualInfo->visual, AllocNone);
				XSetWindowAttributes lWinAttr;
				lWinAttr.colormap = lColMap;
				lWinAttr.border_pixel = 0;
				lWinAttr.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask |
							ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
							Button1MotionMask | Button2MotionMask | Button3MotionMask;
				mWnd = ::XCreateWindow(
					mDisplay,
					RootWindow(mDisplay, lVisualInfo->screen),
					lScreenWidth/2 - lWindowWidth/2,
					lScreenHeight/2 - lWindowHeight/2,
					lWindowWidth,
					lWindowHeight,
					0,
					lVisualInfo->depth,
					InputOutput,
					lVisualInfo->visual,
					CWBorderPixel | CWColormap | CWEventMask,
					&lWinAttr
				);

				if (mWnd)
				{
					HideWindow(false);
					XEvent lEvent;
					::XIfEvent(mDisplay, &lEvent, WaitForNotify, (char*)mWnd);
				}
			}
			break;
			default:
			break;
		}

		mMinimized = false;
		mMaximized = false;

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
		X11Core::AddDisplayManager(this);
	}

	return lOk;
}

void X11DisplayManager::GetBorderSize(int& pSizeX, int& pSizeY)
{
	if (mScreenMode == FULLSCREEN || mScreenMode == SPLASH_WINDOW)
	{
		pSizeX = 0;
		pSizeY = 0;
	}
	else if(mWnd != 0)
	{
		// Use the safest way there is... Taking the difference between
		// the size of the window and the size of the client area.
		// These numbers can't lie.
		XWindowAttributes lWndAttr;
		::XGetWindowAttributes(mDisplay, mWnd, &lWndAttr);
		pSizeX = lWndAttr.border_width * 2;
		pSizeY = lWndAttr.border_width * 3 + 20;	// TODO!
	}
	else
	{
		// TODO
		pSizeX = 5*2;
		pSizeY = 3*3+20;
	}
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

void X11DisplayManager::ProcessMessages()
{
	if (!mWnd)
	{
		return;
	}

	// TODO: add resize and user window shutdown (the X in the corner).
	const int lMessageMask = StructureNotifyMask | KeyPressMask | KeyReleaseMask |
			ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
			Button1MotionMask | Button2MotionMask | Button3MotionMask;
	XEvent lEvent;
	while (::XCheckMaskEvent(mDisplay, lMessageMask, &lEvent))
	{
		DispatchMessage(lEvent);
	}
}

void X11DisplayManager::AddObserver(unsigned pMessage, X11Observer* pObserver)
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

void X11DisplayManager::RemoveObserver(unsigned pMessage, X11Observer* pObserver)
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

bool X11DisplayManager::OnMessage(const XEvent& pEvent)
{
	switch(pEvent.type)
	{
		case ResizeRequest:
		{
			const XResizeRequestEvent& lResizeEvent = (const XResizeRequestEvent&)pEvent;
			DispatchResize(lResizeEvent.width, lResizeEvent.height);

			/*switch(pwParam)
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
			}*/
		}
	}

	return false;
}

bool X11DisplayManager::DispatchMessage(const XEvent& pEvent)
{
	mLog.Infof(_T("X message: %i"), pEvent.type);

	bool lConsumed = false;
	ObserverSetTable::Iterator lTIter = mObserverSetTable.Find(pEvent.type);
	if (lTIter != mObserverSetTable.End())
	{
		ObserverSet* lSet = *lTIter;
		ObserverSet::iterator lLIter;
		for (lLIter = lSet->begin(); lLIter != lSet->end(); ++lLIter)
		{
			lConsumed |= (*lLIter)->OnMessage(pEvent);
		}
	}
	return lConsumed;
}

Bool X11DisplayManager::WaitForNotify(Display* d, XEvent* e, char* arg)
{
	return((e->type == MapNotify) && (e->xmap.window == (::Window)arg));
}

XVisualInfo* X11DisplayManager::GetVisualInfo() const
{
	int lX11AttributeList[] =
	{
			GLX_RGBA, GLX_DOUBLEBUFFER,
			GLX_RED_SIZE, 1,
			GLX_GREEN_SIZE, 1,
			GLX_BLUE_SIZE, 1,
			None
	};
	return(::glXChooseVisual(mDisplay, DefaultScreen(mDisplay), lX11AttributeList));
}



int X11DisplayManager::msWindowCount = 0;
loginstance(UI_GFX, X11DisplayManager);



}
