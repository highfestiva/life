
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/Mac/UiMacOpenGLDisplay.h"
#include <stdio.h>
#include "../../../Lepra/Include/String.h"
#include "../../Include/Mac/UiMacInput.h"
#include "../../Include/UiLepra.h"
#include "../../Include/UiOpenGLExtensions.h"



namespace UiLepra
{



MacOpenGLDisplay::MacOpenGLDisplay():
	mGlView(0)
{
}

MacOpenGLDisplay::~MacOpenGLDisplay()
{
	CloseScreen();
}

void MacOpenGLDisplay::CloseScreen()
{
	Deactivate();

	if (!mIsOpen)
	{
		return;
	}

	/*if (IsFullScreen() == true)
	{
		TODO: port!
		::ChangeDisplaySettings(NULL, 0);
	}*/

	DeleteGLContext();

	MacDisplayManager::CloseScreen();
}

bool MacOpenGLDisplay::Activate()
{
#ifdef LEPRA_IOS
	mGlView.context = mGlContext;
	[mGlView setFramebuffer];
#else // !iOS
	[mGlContext makeCurrentContext];
#endif // iOS/!iOS
	return true;
}

bool MacOpenGLDisplay::UpdateScreen()
{
	if (!mIsOpen)
	{
		return (false);
	}

	// Disable scissor test to make sure that the entire screen will
	// be updated.
	GLboolean lScissorsEnabled = ::glIsEnabled(GL_SCISSOR_TEST);
	::glDisable(GL_SCISSOR_TEST);

#ifdef LEPRA_IOS
	[mGlView presentFramebuffer];
#else // !iOS
	[mGlContext flushBuffer];
#endif // iOS/!iOS

	if (lScissorsEnabled)
	{
		::glEnable(GL_SCISSOR_TEST);
	}

	return (true);
}

bool MacOpenGLDisplay::IsVSyncEnabled() const
{
	return true; //OpenGLExtensions::IsVSyncEnabled();
}

bool MacOpenGLDisplay::SetVSyncEnabled(bool pEnabled)
{
	return true;
//	return (mIsOpen? OpenGLExtensions::SetVSyncEnabled(pEnabled) : false);
}

DisplayManager::ContextType MacOpenGLDisplay::GetContextType()
{
	return DisplayManager::OPENGL_CONTEXT;
}

void MacOpenGLDisplay::SetFocus(bool pFocus)
{
	if (IsFullScreen() == true)
	{
		if (pFocus == false)
		{
			DispatchMinimize();
		}
		else
		{
			DispatchResize(mDisplayMode.mWidth, mDisplayMode.mHeight);
		}
	}
}

void MacOpenGLDisplay::Deactivate()
{
	if (mIsOpen)
	{
#ifdef LEPRA_IOS
		[LEPRA_APPLE_GL_CONTEXT setCurrentContext:nil];
#else // !iOS
		[LEPRA_APPLE_GL_CONTEXT clearCurrentContext];
#endif // iOS/!iOS
	}
}

void MacOpenGLDisplay::OnResize(int pWidth, int pHeight)
{
	Resize(pWidth, pHeight);
	//TODO: Mac resize!
	//::ShowWindow(mWnd, SW_SHOWNORMAL);
	Activate();
}

void MacOpenGLDisplay::Resize(int pWidth, int pHeight)
{
	if (!mGlView)
	{
		return;
	}

	if (IsFullScreen() == true)
	{
		// Since we are in fullscreen mode, we ignore the width and height
		// given as parameters.
		if (IsMinimized() == true)
		{
			//TODO: something!
			//::ChangeDisplaySettings(&lNewMode, CDS_FULLSCREEN);
		}
	}
	else
	{
		mDisplayMode.mWidth  = pWidth;
		mDisplayMode.mHeight = pHeight;
	}

	::glViewport(0, 0, mDisplayMode.mWidth, mDisplayMode.mHeight);
	UpdateCaption();
}

void MacOpenGLDisplay::OnMinimize()
{
	Deactivate();

	if (IsFullScreen() == true)
	{
		//TODO: something!
		//::ChangeDisplaySettings(NULL, 0);
	}

	// TODO:
	//::ShowWindow(mWnd, SW_MINIMIZE);
}

void MacOpenGLDisplay::OnMaximize(int pWidth, int pHeight)
{
	Resize(pWidth, pHeight);
	//TODO: port!
	//::ShowWindow(mWnd, SW_SHOWMAXIMIZED);
	Activate();
}

bool MacOpenGLDisplay::InitScreen()
{
	UpdateCaption();



/*TODO: port!

	if (mScreenMode == FULLSCREEN)
	{
		DEVMODE lNewMode;
		lNewMode.dmSize	     = sizeof(lNewMode);
		lNewMode.dmBitsPerPel       = mDisplayMode.mBitDepth;
		lNewMode.dmPelsWidth	= mDisplayMode.mWidth;
		lNewMode.dmPelsHeight       = mDisplayMode.mHeight;
		lNewMode.dmDisplayFrequency = mDisplayMode.mRefreshRate;
		lNewMode.dmFields = DM_BITSPERPEL |
							 DM_PELSWIDTH  |
							 DM_PELSHEIGHT |
							 DM_DISPLAYFREQUENCY;

		if (::ChangeDisplaySettings(&lNewMode, CDS_FULLSCREEN) !=
		   DISP_CHANGE_SUCCESSFUL)
		{
			//return false;
		}

		::MoveWindow(mWnd, 0, 0, mDisplayMode.mWidth, mDisplayMode.mHeight, TRUE);
	}
	else if(mScreenMode == DisplayManager::WINDOWED ||
		mScreenMode == DisplayManager::STATIC_WINDOW)
	{
		// TODO: This stuff is weird! Sometimes it works, and sometimes
		// it doesn't.
		int lWindowWidth  = GetWindowWidth(mDisplayMode.mWidth);
		int lWindowHeight = GetWindowHeight(mDisplayMode.mHeight);

		int lX = GetSystemMetrics(SM_CXSCREEN) / 2 - mDisplayMode.mWidth / 2;
		int lY = GetSystemMetrics(SM_CYSCREEN) / 2 - mDisplayMode.mHeight / 2;

		::MoveWindow(mWnd, lX, lY, lWindowWidth, lWindowHeight, TRUE);
	}
	else
	{
		int lX = GetSystemMetrics(SM_CXSCREEN) / 2 - mDisplayMode.mWidth / 2;
		int lY = GetSystemMetrics(SM_CYSCREEN) / 2 - mDisplayMode.mHeight / 2;

		::MoveWindow(mWnd, lX, lY, mDisplayMode.mWidth, mDisplayMode.mHeight, TRUE);
	}

	mDC = ::GetDC(mWnd);

	if (!SetGLPixelFormat())
	{
		MessageBox(NULL,
			   _T("Unable to setup pixel format, Please install a new OpenGL driver,") \
			   _T("or try closing other running applications..."),
			   _T("OpenGL Error") ,MB_OK | MB_ICONWARNING );
		return false;
	}

	if (!CreateGLContext())
	{
		MessageBox(NULL,
			   _T("Unable to setup OpenGL, Please install a new OpenGL driver,") \
			   _T("or try closing other running applications..."),
			   _T("OpenGL Error"),MB_OK | MB_ICONWARNING );
		return false;
	}*/

	if (!SetGLPixelFormat())
	{
		printf("OpenGL error: unable to setup pixel format. Please try installing a new OpenGL driver, " \
			"or try closing other running applications.");
		return false;
	}

	if (!CreateGLContext())
	{
		printf("OpenGL error: unable to create context. Please try installing a new OpenGL driver, " \
			"or try closing other running applications.");
		return false;
	}

	::glEnable(GL_SCISSOR_TEST);
	if (mContextUserCount == 1)
	{
		// TODO: ?
		//OpenGLExtensions::InitExtensions();
	}

#ifndef LEPRA_IOS
	[mWnd setContentSize:mWnd.frame.size];
#endif // !iOS

	return true;
}

void MacOpenGLDisplay::UpdateCaption()
{
	str lString = strutil::Format(_T("MacOpenGLDisplay (%ix%i %iBit %iHz) %s"),
		mDisplayMode.mWidth, mDisplayMode.mHeight,
		mDisplayMode.mBitDepth, mDisplayMode.mRefreshRate,
		 _T(LEPRA_STRING_TYPE_TEXT) _T(" ") _T(LEPRA_BUILD_TYPE_TEXT));
	SetCaption(lString, true);
}

bool MacOpenGLDisplay::CreateGLContext()
{
	if (mGlContext == 0)
	{
#ifdef LEPRA_IOS
		mGlContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
#else // !iOS
		mGlContext = [mGlView openGLContext];
#endif // iOS/!iOS
	}

	bool lOk = (mGlContext != 0);
	if (lOk)
	{
		++mContextUserCount;
		lOk = Activate();
	}
	return (lOk);
}

void MacOpenGLDisplay::DeleteGLContext()
{
	if (mContextUserCount >= 1)
	{
		--mContextUserCount;
	}
	if (mContextUserCount == 0)
	{
		Deactivate();
		mGlContext = 0;
	}
}



bool MacOpenGLDisplay::SetGLPixelFormat()
{
#ifdef LEPRA_IOS
	mGlView = [[EAGLView alloc] initWithFrame:mWnd.bounds];
	[mWnd addSubview:mGlView];
#else // !iOS
	NSOpenGLPixelFormatAttribute lPixelFormatAttribs[32];
	NSOpenGLPixelFormatAttribute* lAttrib = lPixelFormatAttribs;

	*lAttrib++ = NSOpenGLPFANoRecovery;
	*lAttrib++ = NSOpenGLPFADoubleBuffer;
	*lAttrib++ = NSOpenGLPFAAccelerated;
	//      *lAttrib++ = kCGLPFAFullScreen;
	*lAttrib++ = NSOpenGLPFAAccumSize;
	*lAttrib++ = (NSOpenGLPixelFormatAttribute)mDisplayMode.mBitDepth;
	*lAttrib++ = NSOpenGLPFAColorSize;
	*lAttrib++ = (NSOpenGLPixelFormatAttribute)mDisplayMode.mBitDepth;
	if (mDisplayMode.mBitDepth > 0)
	{
		*lAttrib++ = NSOpenGLPFADepthSize;
		*lAttrib++ = (NSOpenGLPixelFormatAttribute)mDisplayMode.mBitDepth;
	}
	if (true)	// TODO: stencil depth somehow?
	{
		*lAttrib++ = NSOpenGLPFAStencilSize;
		*lAttrib++ = (NSOpenGLPixelFormatAttribute)8;
	}
	*lAttrib++ = (NSOpenGLPixelFormatAttribute) 0;
	NSOpenGLPixelFormat* lPixelFormat = [NSOpenGLPixelFormat alloc];
	[lPixelFormat initWithAttributes:lPixelFormatAttribs];

	mGlView = [NSOpenGLView alloc];
	//CGRect lContentSize = [mWnd contentSize];
	[mGlView initWithFrame:mWnd.frame pixelFormat:lPixelFormat];
	[mWnd setContentView:mGlView];
	[mWnd setAcceptsMouseMovedEvents: YES];
	[mWnd setIgnoresMouseEvents: NO];
	//[mGlView setAutoresizingMask:NSViewWidthSizable|NSViewHeightSizable];
	//[mGlView setAutoresizesSubviews:YES];
	//[mWnd setFrame: NSMakeRect(0, 0, mScreen->GetActiveResolution().GetWidth(), mScreen->GetActiveResolution().GetHeight()) display: YES];
	//mGlView.frame = lContentSize;

	//[mGlView reshape];
#endif // iOS/!iOS
	return (true);	// TODO: add error checking!
}



LEPRA_APPLE_GL_CONTEXT* MacOpenGLDisplay::mGlContext = 0;
int MacOpenGLDisplay::mContextUserCount = 0;



}
