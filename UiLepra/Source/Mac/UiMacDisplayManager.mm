
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/Mac/UiMacDisplayManager.h"
#include <stdexcept>
#include "../../../Lepra/Include/Log.h"
#include "../../../Lepra/Include/String.h"
#include "../../../Lepra/Include/SystemManager.h"
#include "../../Include/UiLepra.h"
#include "../../Include/Mac/UiMacInput.h"
#include "../../Include/Mac/UiMacOpenGLDisplay.h"



@interface NativeWindow: NSWindow
{
}
@end

@implementation NativeWindow
/*- (void) mouseDown: (NSEvent*)theEvent
{
	//printf("Mouse down i min favoritvy\n");
	//[self setAcceptsMouseMovedEvents: YES];
	//[self setIgnoresMouseEvents: NO];
}*/

- (void) mouseDragged: (NSEvent*)theEvent
{
	//printf("Mouse dragged i min favoritvy\n");
	//[self setAcceptsMouseMovedEvents: YES];
	//[self setIgnoresMouseEvents: NO];
	[self mouseMoved: theEvent];
}

- (void)mouseMoved: (NSEvent*)theEvent
{
	//printf("mouseMoved!\n");
	//[super mouseMoved:theEvent];

	UiLepra::MacInputManager* lInput = UiLepra::MacInputManager::GetSingleton();
	if (lInput)
	{
		NSPoint lPoint = [theEvent locationInWindow];
		lInput->SetMousePosition(lPoint.x, lPoint.y);
	}
}
- (void)keyDown: (NSEvent*)theEvent
{
	NSString *characters = [theEvent characters];
	unichar character = [characters characterAtIndex: 0];
	UiLepra::MacInputManager* lInput = UiLepra::MacInputManager::GetSingleton();
	if (lInput)
	{
		// TODO: handle stuff like NSRightArrowFunctionKey, NSLeftArrowFunctionKey.
		lInput->NotifyOnChar(character);
	}
}
- (void)keyUp: (NSEvent*)theEvent
{
	//printf("view keyup!\n");
}
/*- (void)mouseDown: (NSEvent*)theEvent
{
	//((UiLepra::MacInputManager*)InputManager::GetInputManager())->OSXMouseDownEvent(0);
}
- (void)rightMouseDown: (NSEvent*)theEvent
{
	//((UiLepra::MacInputManager*)InputManager::GetInputManager())->OSXMouseDownEvent(1);
}
- (void)otherMouseDown: (NSEvent*)theEvent
{
	//((UiLepra::MacInputManager*)InputManager::GetInputManager())->OSXMouseDownEvent(2);
}
- (void)mouseUp: (NSEvent*)theEvent
{
	//((UiLepra::MacInputManager*)InputManager::GetInputManager())->OSXMouseUpEvent(0);
}
- (void)rightMouseUp: (NSEvent*)theEvent
{
	//((UiLepra::MacInputManager*)InputManager::GetInputManager())->OSXMouseUpEvent(1);
}
- (void)otherMouseUp: (NSEvent*)theEvent
{
	//((UiLepra::MacInputManager*)InputManager::GetInputManager())->OSXMouseUpEvent(2);
}
- (void)scrollWheel: (NSEvent*)theEvent
{
	//((MacInputManager*)InputManager::GetInputManager())->OSXScrollWheelEvent([theEvent deltaY]/10.0f);
}*/
- (BOOL)acceptsFirstResponder
{
	return YES;
}
- (BOOL)becomeFirstResponder
{
	return YES;
}
- (BOOL)resignFirstResponder
{
	return YES;
}
@end



namespace UiLepra
{



DisplayManager* DisplayManager::CreateDisplayManager(ContextType pCT)
{
	DisplayManager* lDisplayManager = 0;
	switch(pCT)
	{
		case DisplayManager::OPENGL_CONTEXT:	lDisplayManager = new MacOpenGLDisplay;				break;
		default:				mLog.AError("Invalid context type in CreateDisplayManager().");	break;
	}
	return (lDisplayManager);
}

void DisplayManager::EnableScreensaver(bool /*pEnable*/)
{
	// TODO: implement!
}



MacDisplayManager::MacDisplayManager():
	mScreenMode(DisplayManager::WINDOWED),
	mWnd(0),
	mIsOpen(false),
	mMinimized(false),
	mMaximized(false),
	mNormalWidth(0),
	mNormalHeight(0),
	mCaptionSet(false),
	mConsumeChar(true)
{
	// Obtain number of available displays.
	CGDisplayCount lDisplayCount = 0;
	CGDisplayErr lError = CGGetActiveDisplayList(0, 0, &lDisplayCount);
	assert(lError == CGDisplayNoErr);
	assert(lDisplayCount >= 1);
	if (lError != CGDisplayNoErr || lDisplayCount < 1)
	{
		throw std::runtime_error("CoreGraphics error: fetch of number of screens failed.");
	}

	// Allocate and convert.
	CGDirectDisplayID* lDisplays = (CGDirectDisplayID*)new char[sizeof(CGDirectDisplayID) * lDisplayCount];
	lError = CGGetActiveDisplayList(lDisplayCount, lDisplays, &lDisplayCount);
	assert(lError == CGDisplayNoErr);
	assert(lDisplayCount >= 1);
	if (lError != CGDisplayNoErr || lDisplayCount < 1)
	{
		throw std::runtime_error("CoreGraphics error: fetch of number of screens failed 2.");
	}
	if(lDisplayCount >= 1)
	{
		CFArrayRef lModeList = CGDisplayAvailableModes(lDisplays[0]);
		if (!lModeList)
		{
			throw std::runtime_error("CoreGraphics error: fetch of display modes failed.");
		}
		CFIndex lModeCount = CFArrayGetCount(lModeList);
		mEnumeratedDisplayModeCount = lModeCount;
		mEnumeratedDisplayMode = new DisplayMode[mEnumeratedDisplayModeCount];
		for (CFIndex x = 0; x < lModeCount; ++x)
		{
			CFDictionaryRef lMode = (CFDictionaryRef)CFArrayGetValueAtIndex(lModeList, x);
			mEnumeratedDisplayMode[x] = ConvertNativeDisplayMode(lMode);
		}
	}
	
	delete[] (lDisplays);
}

MacDisplayManager::~MacDisplayManager()
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

unsigned MacDisplayManager::GetWidth() 
{
	return mDisplayMode.mWidth;
}

unsigned MacDisplayManager::GetHeight() 
{
	return mDisplayMode.mHeight;
}

unsigned MacDisplayManager::GetBitDepth() 
{
	return mDisplayMode.mBitDepth;
}

unsigned MacDisplayManager::GetRefreshRate()
{
	return mDisplayMode.mRefreshRate;
}

bool MacDisplayManager::IsFullScreen() 
{
	return (mScreenMode == DisplayManager::FULLSCREEN);
}

void MacDisplayManager::SetCaption(const str& pCaption)
{
	SetCaption(pCaption, false);
}

void MacDisplayManager::SetCaption(const str& pCaption, bool pInternalCall)
{
	if (mIsOpen)
	{
		if (pInternalCall == false)
		{
			mCaptionSet = true;
		}

		if (pInternalCall == false || mCaptionSet == false)
		{
			[mWnd setTitle: [NSString stringWithCString: astrutil::Encode(pCaption).c_str()]];
		}
	}
}

bool MacDisplayManager::OpenScreen(const DisplayMode& pDisplayMode, ScreenMode pScreenMode)
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

void MacDisplayManager::CloseScreen()
{
	if (mIsOpen)
	{
		mIsOpen = false;
		MacCore::RemoveDisplayManager(this);
		RemoveObserver(this);

		if (mWnd)
		{
			[mWnd close];
			mWnd = 0;
		}
	}
}

bool MacDisplayManager::IsVisible() const
{
	return (!IsMinimized());
}

bool MacDisplayManager::InitWindow()
{
	CloseScreen();

	bool lOk = mIsOpen = true;

	mWnd = [NativeWindow alloc];
	[mWnd	initWithContentRect:	NSMakeRect(0, 0, mDisplayMode.mWidth, mDisplayMode.mHeight)
		styleMask:		NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask
		backing:		NSBackingStoreBuffered
		defer:			NO];
	[mWnd setAcceptsMouseMovedEvents: YES];
	[mWnd setIgnoresMouseEvents: NO];
        [mWnd setReleasedWhenClosed: YES];
	[mWnd makeKeyAndOrderFront: nil];
	[mWnd	setFrame:	NSMakeRect(0, 0, mDisplayMode.mWidth, mDisplayMode.mHeight)
		display:	YES];

	++mWindowCount;

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
								_T("LepraMacClass"), _T("Lepra"),
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

					mWnd = ::CreateWindowEx(0, _T("LepraMacClass"), _T("Lepra"),
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
		MacCore::AddDisplayManager(this);
	}

	return lOk;
}

void MacDisplayManager::GetBorderSize(int& pSizeX, int& pSizeY)
{
	//if (mScreenMode == FULLSCREEN ||
	//   mScreenMode == SPLASH_WINDOW)
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

int MacDisplayManager::GetWindowWidth(int pClientWidth)
{
	int lBorderSizeX;
	int lBorderSizeY;
	GetBorderSize(lBorderSizeX, lBorderSizeY);
	return pClientWidth + lBorderSizeX;
}

int MacDisplayManager::GetWindowHeight(int pClientHeight)
{
	int lBorderSizeX;
	int lBorderSizeY;
	GetBorderSize(lBorderSizeX, lBorderSizeY);
	return pClientHeight + lBorderSizeY;
}

int MacDisplayManager::GetClientWidth(int pWindowWidth)
{
	int lBorderSizeX;
	int lBorderSizeY;
	GetBorderSize(lBorderSizeX, lBorderSizeY);
	return pWindowWidth - lBorderSizeX;
}

int MacDisplayManager::GetClientHeight(int pWindowHeight)
{
	int lBorderSizeX;
	int lBorderSizeY;
	GetBorderSize(lBorderSizeX, lBorderSizeY);
	return pWindowHeight - lBorderSizeY;
}

NSWindow* MacDisplayManager::GetWindow() const
{
	return mWnd;
}

bool MacDisplayManager::DispatchMessage(NSEvent* e)
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
	return (false);
}

void MacDisplayManager::ProcessMessages()
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

void MacDisplayManager::AddObserver(int pMessage, MacObserver* pObserver)
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

void MacDisplayManager::RemoveObserver(int pMessage, MacObserver* pObserver)
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

void MacDisplayManager::RemoveObserver(MacObserver* pObserver)
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

void MacDisplayManager::ShowMessageBox(const str& pMsg, const str& pCaption)
{
	// StandardAlert() I googled, whatever that might be.
}

bool MacDisplayManager::OnMessage(NSEvent* e)
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



DisplayMode MacDisplayManager::ConvertNativeDisplayMode(CFDictionaryRef pMode)
{
	CFNumberRef lResWidth = (CFNumberRef)CFDictionaryGetValue(pMode, kCGDisplayWidth);
	CFNumberRef lResHeight = (CFNumberRef)CFDictionaryGetValue(pMode, kCGDisplayHeight);
	CFNumberRef lResRefreshRate = (CFNumberRef)CFDictionaryGetValue(pMode, kCGDisplayRefreshRate);
	CFNumberRef lResBitsPerPixel = (CFNumberRef)CFDictionaryGetValue(pMode, kCGDisplayBitsPerPixel);

	if (!lResWidth || !lResHeight || !lResRefreshRate || !lResBitsPerPixel)
	{
		throw std::runtime_error("Unknown error while inspecting display mode.");
	}

	int lWidth;
	CFNumberGetValue(lResWidth, kCFNumberSInt32Type, &lWidth);
	int lHeight;
	CFNumberGetValue(lResHeight, kCFNumberSInt32Type, &lHeight);
	int lBpp;
	CFNumberGetValue(lResBitsPerPixel, kCFNumberSInt32Type, &lBpp);
	float lRefreshRate;
	CFNumberGetValue(lResRefreshRate, kCFNumberFloat32Type, &lRefreshRate);

	DisplayMode lDisplayMode;
	lDisplayMode.mWidth = lWidth;
	lDisplayMode.mHeight = lHeight;
	lDisplayMode.mBitDepth = lBpp;
	lDisplayMode.mRefreshRate = lRefreshRate;
	return (lDisplayMode);
}



int MacDisplayManager::mWindowCount = 0;
LOG_CLASS_DEFINE(UI_GFX, MacDisplayManager);



}
