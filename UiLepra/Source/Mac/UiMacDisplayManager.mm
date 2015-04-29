
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../Include/Mac/UiMacDisplayManager.h"
#include <stdexcept>
#include "../../../Lepra/Include/Posix/MacLog.h"
#include "../../../Lepra/Include/Log.h"
#include "../../../Lepra/Include/String.h"
#include "../../../Lepra/Include/SystemManager.h"
#include "../../Include/UiLepra.h"
#include "../../Include/Mac/UiIosDef.h"
#include "../../Include/Mac/UiMacOpenGLDisplay.h"



#ifdef LEPRA_IOS
@interface NativeWindow: LEPRA_APPLE_WINDOW
#else // !iOS
@interface NativeWindow: NSWindow <NSWindowDelegate>
#endif // iOS/!iOS
{
	@public UiLepra::MacDisplayManager* mDisplayManager;
}
@end

@implementation NativeWindow
-(void) keyDown:(LEPRA_APPLE_EVENT*)theEvent
{
	if (mDisplayManager) mDisplayManager->DispatchEvent(theEvent);
}
-(void) keyUp:(LEPRA_APPLE_EVENT*)theEvent
{
	if (mDisplayManager) mDisplayManager->DispatchEvent(theEvent);
}
-(void) flagsChanged:(LEPRA_APPLE_EVENT*)theEvent
{
	if (mDisplayManager) mDisplayManager->DispatchEvent(theEvent);
}
- (void) mouseDragged: (LEPRA_APPLE_EVENT*)theEvent
{
	if (mDisplayManager) mDisplayManager->DispatchEvent(theEvent);
}
- (void)mouseMoved: (LEPRA_APPLE_EVENT*)theEvent
{
	if (mDisplayManager) mDisplayManager->DispatchEvent(theEvent);
}
#ifndef LEPRA_IOS
-(void) windowDidResize:(NSNotification*)notification
{
	NSView* lView = [self contentView];
	if (mDisplayManager) mDisplayManager->DispatchResize(lView.frame.size.width, lView.frame.size.height);
}
#endif // !iOS
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

void DisplayManager::EnableScreensaver(bool pEnable)
{
	(void)pEnable;
	// TODO: implement!
}



MacDisplayManager::MacDisplayManager():
	mWnd(0),
	mIsOpen(false),
	mMinimized(false),
	mMaximized(false),
	mNormalWidth(0),
	mNormalHeight(0),
	mCaptionSet(false)
{
#ifdef LEPRA_IOS
	mScreenMode = FULLSCREEN;
	mEnumeratedDisplayModeCount = 10;
	mEnumeratedDisplayMode = new DisplayMode[mEnumeratedDisplayModeCount];
	DisplayMode lDisplayMode;
	lDisplayMode.mWidth = 320;
	lDisplayMode.mHeight = 480;
	lDisplayMode.mRefreshRate = 0;
	lDisplayMode.mBitDepth = 0;
	mEnumeratedDisplayMode[0] = lDisplayMode;
	std::swap(lDisplayMode.mWidth, lDisplayMode.mHeight);
	mEnumeratedDisplayMode[1] = lDisplayMode;
	lDisplayMode.mWidth = 640;
	lDisplayMode.mHeight = 960;
	mEnumeratedDisplayMode[2] = lDisplayMode;
	std::swap(lDisplayMode.mWidth, lDisplayMode.mHeight);
	mEnumeratedDisplayMode[3] = lDisplayMode;
	lDisplayMode.mWidth = 640;
	lDisplayMode.mHeight = 1136;
	mEnumeratedDisplayMode[4] = lDisplayMode;
	std::swap(lDisplayMode.mWidth, lDisplayMode.mHeight);
	mEnumeratedDisplayMode[5] = lDisplayMode;
	lDisplayMode.mWidth = 1024;
	lDisplayMode.mHeight = 768;
	mEnumeratedDisplayMode[6] = lDisplayMode;
	std::swap(lDisplayMode.mWidth, lDisplayMode.mHeight);
	mEnumeratedDisplayMode[7] = lDisplayMode;
	lDisplayMode.mWidth = 2048;
	lDisplayMode.mHeight = 1536;
	mEnumeratedDisplayMode[8] = lDisplayMode;
	std::swap(lDisplayMode.mWidth, lDisplayMode.mHeight);
	mEnumeratedDisplayMode[9] = lDisplayMode;
#else // !iOS
	// Obtain number of available displays.
	CGDisplayCount lDisplayCount = 0;
	CGDisplayErr lError = CGGetActiveDisplayList(0, 0, &lDisplayCount);
	deb_assert(lError == CGDisplayNoErr);
	deb_assert(lDisplayCount >= 1);
	if (lError != CGDisplayNoErr || lDisplayCount < 1)
	{
		throw std::runtime_error("CoreGraphics error: fetch of number of screens failed.");
	}

	// Allocate and convert.
	CGDirectDisplayID* lDisplays = (CGDirectDisplayID*)new char[sizeof(CGDirectDisplayID) * lDisplayCount];
	lError = CGGetActiveDisplayList(lDisplayCount, lDisplays, &lDisplayCount);
	deb_assert(lError == CGDisplayNoErr);
	deb_assert(lDisplayCount >= 1);
	if (lError != CGDisplayNoErr || lDisplayCount < 1)
	{
		throw std::runtime_error("CoreGraphics error: fetch of number of screens failed 2.");
	}
	if(lDisplayCount >= 1)
	{
		//CGSize lScreenResolution = CGDisplayBounds(lDisplays[0]).size;
		CFArrayRef lModeList = CGDisplayCopyAllDisplayModes(lDisplays[0], NULL);
		if (!lModeList)
		{
			throw std::runtime_error("CoreGraphics error: fetch of display modes failed.");
		}
		CFIndex lModeCount = CFArrayGetCount(lModeList);
		mEnumeratedDisplayModeCount = lModeCount;
		mEnumeratedDisplayMode = new DisplayMode[mEnumeratedDisplayModeCount];
		for (CFIndex x = 0; x < lModeCount; ++x)
		{
			CGDisplayModeRef lMode = (CGDisplayModeRef)CFArrayGetValueAtIndex(lModeList, x);
			mEnumeratedDisplayMode[x] = ConvertNativeDisplayMode(lMode);
		}
	}
	
	delete[] (lDisplays);
#endif // iOS/!iOS
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

unsigned MacDisplayManager::GetWidth() const
{
	return mDisplayMode.mWidth;
}

unsigned MacDisplayManager::GetHeight() const
{
	return mDisplayMode.mHeight;
}

unsigned MacDisplayManager::GetBitDepth() const
{
	return mDisplayMode.mBitDepth;
}

unsigned MacDisplayManager::GetRefreshRate() const
{
	return mDisplayMode.mRefreshRate;
}

bool MacDisplayManager::IsFullScreen() const
{
	return (mScreenMode == DisplayManager::FULLSCREEN);
}

double MacDisplayManager::GetPhysicalScreenSize() const
{
#ifndef LEPRA_IOS
	return 24.0;	// Who knows how big an average Mac user's screen is when this game runs.
#else // iOS
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
	{
		CGSize size = [[UIScreen mainScreen] bounds].size;
		const float lHeight = std::max(size.width, size.height);
		if (lHeight < 500)
		{
			return SCREEN_SIZE_IPHONE_4;	// Or 4th Gen iPod Touch or earlier.
		}
		else if (lHeight < 600)
		{
			return SCREEN_SIZE_IPHONE_5;
		}
		else if (lHeight < 700)
		{
			return SCREEN_SIZE_IPHONE_6;
		}
		else
		{
			return SCREEN_SIZE_IPHONE_6P;
		}
	}
	else
	{
		const str lHwName = SystemManager::GetHwName();
		const bool lIsMini = (lHwName == _T("iPad2,5") ||
			lHwName == _T("iPad2,6") || lHwName == _T("iPad2,7"));
		if (!lIsMini)
		{
			return SCREEN_SIZE_IPAD_CLASSIC;
		}
		else
		{
			return SCREEN_SIZE_IPAD_MINI;
		}

	}
#endif // !iOS / iOS
}

void MacDisplayManager::SetCaption(const str& pCaption)
{
	SetCaption(pCaption, false);
}

void MacDisplayManager::SetCaption(const str& pCaption, bool pInternalCall)
{
#ifndef LEPRA_IOS
	if (mIsOpen)
	{
		if (pInternalCall == false)
		{
			mCaptionSet = true;
		}

		if (pInternalCall == false || mCaptionSet == false)
		{
			[mWnd setTitle:MacLog::Encode(pCaption)];
		}
	}
#endif // !iOS
}

bool MacDisplayManager::OpenScreen(const DisplayMode& pDisplayMode, ScreenMode pScreenMode, Orientation pOrientation)
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
/*#ifdef LEPRA_IOS
		int w = mDisplayMode.mWidth;
		mDisplayMode.mWidth = mDisplayMode.mHeight;
		mDisplayMode.mHeight = w;
#endif // iOS*/
		lOk = InitWindow();
	}

	if (lOk)
	{
		lOk = mIsOpen = InitScreen();
	}

	return lOk;
}

void MacDisplayManager::CloseScreen()
{
	if (mIsOpen)
	{
		mIsOpen = false;

		if (mWnd)
		{
			((NativeWindow*)mWnd)->mDisplayManager = nil;
#ifndef LEPRA_IOS
			[mWnd release];
#endif // iOS
			mWnd = nil;
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

	NativeWindow* lWnd = [NativeWindow alloc];
	mWnd = lWnd;
	lWnd->mDisplayManager = this;
#ifdef LEPRA_IOS
	CGRect r = [UIScreen mainScreen].bounds;
	int s = [[UIScreen mainScreen] scale];
	r.size = CGSizeMake(r.size.width*s, r.size.height*s);
	[mWnd initWithFrame:r];
#else // !iOS
	[mWnd initWithContentRect:NSMakeRect(0, 0, mDisplayMode.mWidth, mDisplayMode.mHeight)
			styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask
			  backing:NSBackingStoreBuffered
			    defer:NO];
	lWnd.delegate = lWnd;
	[mWnd setAcceptsMouseMovedEvents:YES];
	[mWnd setIgnoresMouseEvents:NO];
        [mWnd setReleasedWhenClosed:YES];
	[mWnd makeKeyAndOrderFront:nil];
	[mWnd setFrame:NSMakeRect(0, 0, mDisplayMode.mWidth, mDisplayMode.mHeight)
	       display:YES];
	[mWnd center];
#endif // iOS/!iOS

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
	if (mScreenMode == FULLSCREEN || mScreenMode == SPLASH_WINDOW)
	{
		pSizeX = 0;
		pSizeY = 0;
	}
	else if (mWnd)
	{
		pSizeX = 0;
		pSizeY = 0;
	}
	else
	{
		deb_assert(false);
		pSizeX = 0;
		pSizeY = 0;
	}
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

LEPRA_APPLE_WINDOW* MacDisplayManager::GetWindow() const
{
	return mWnd;
}

void MacDisplayManager::DispatchEvent(LEPRA_APPLE_EVENT* e)
{
	ObserverSetTable::Iterator lTIter = mObserverSetTable.Find([e type]);
	if (lTIter != mObserverSetTable.End())
	{
		ObserverSet* lSet = *lTIter;
		ObserverSet::iterator lLIter;
		for (lLIter = lSet->begin(); lLIter != lSet->end(); ++lLIter)
		{
			(*lLIter)->OnEvent(e);
		}
	}
}

void MacDisplayManager::ProcessMessages()
{
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
#ifndef LEPRA_IOS
	NSRunAlertPanel(MacLog::Encode(pCaption), MacLog::Encode(pMsg), nil, nil, nil);
#else // iOS
	UIAlertView* message = [[UIAlertView alloc]
		initWithTitle:MacLog::Encode(pCaption)
		      message:MacLog::Encode(pMsg)
		     delegate:nil
	    cancelButtonTitle:@"OK"
	    otherButtonTitles:nil];
	[message show];
	//[message release];
#endif // !iOS / iOS
}


#ifndef LEPRA_IOS
DisplayMode MacDisplayManager::ConvertNativeDisplayMode(CGDisplayModeRef pMode)
{
	DisplayMode lDisplayMode;
	lDisplayMode.mWidth = CGDisplayModeGetWidth(pMode);
	lDisplayMode.mHeight = CGDisplayModeGetHeight(pMode);
	lDisplayMode.mRefreshRate = (int)CGDisplayModeGetRefreshRate(pMode);
	lDisplayMode.mBitDepth = 0;
	CFStringRef lPixEnc = CGDisplayModeCopyPixelEncoding(pMode);
	if (CFStringCompare(lPixEnc, CFSTR(IO32BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
	{
		lDisplayMode.mBitDepth = 32;
	}
	else if(CFStringCompare(lPixEnc, CFSTR(IO16BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
	{
		lDisplayMode.mBitDepth = 16;
	}
	else if(CFStringCompare(lPixEnc, CFSTR(IO8BitIndexedPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
	{
		lDisplayMode.mBitDepth = 8;
	}
	return (lDisplayMode);
}
#endif // !iOS



int MacDisplayManager::mWindowCount = 0;
loginstance(UI_GFX, MacDisplayManager);



}
