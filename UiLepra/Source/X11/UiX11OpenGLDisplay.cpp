
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/X11/UiX11OpenGLDisplay.h"
#include <stdio.h>
#include "../../../Lepra/Include/String.h"
#include "../../Include/UiLepra.h"
#include "../../Include/UiOpenGLExtensions.h"



namespace UiLepra
{



X11OpenGLDisplay::X11OpenGLDisplay()
{
}

X11OpenGLDisplay::~X11OpenGLDisplay()
{
	CloseScreen();
}

void X11OpenGLDisplay::CloseScreen()
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

	X11DisplayManager::CloseScreen();
}

bool X11OpenGLDisplay::Activate()
{
	bool lOk = ::glXMakeCurrent(GetDisplay(), GetWindow(), mGlContext);
	return (lOk);
}

bool X11OpenGLDisplay::UpdateScreen()
{
	if (!mIsOpen)
	{
		return (false);
	}

	// Disable scissor test to make sure that the entire screen will
	// be updated.
	GLboolean lScissorsEnabled = ::glIsEnabled(GL_SCISSOR_TEST);
	::glDisable(GL_SCISSOR_TEST);
	::glXSwapBuffers(GetDisplay(), GetWindow());
	if (lScissorsEnabled)
	{
		::glEnable(GL_SCISSOR_TEST);
	}
	return (true);
}

bool X11OpenGLDisplay::IsVSyncEnabled() const
{
	return OpenGLExtensions::IsVSyncEnabled();
}

bool X11OpenGLDisplay::SetVSyncEnabled(bool pEnabled)
{
	return (mIsOpen? OpenGLExtensions::SetVSyncEnabled(pEnabled) : false);
}

DisplayManager::ContextType X11OpenGLDisplay::GetContextType()
{
	return DisplayManager::OPENGL_CONTEXT;
}

unsigned X11OpenGLDisplay::GetPitch()
{
	return mDisplayMode.mWidth;
}

void X11OpenGLDisplay::SetFocus(bool pFocus)
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

void X11OpenGLDisplay::Deactivate()
{
	if (GetDisplay())
	{
		::glXMakeCurrent(GetDisplay(), 0, 0);
	}
}

void X11OpenGLDisplay::OnResize(int pWidth, int pHeight)
{
	Resize(pWidth, pHeight);
	//TODO: X11 resize!
	//::ShowWindow(mWnd, SW_SHOWNORMAL);
	Activate();
}

void X11OpenGLDisplay::Resize(int pWidth, int pHeight)
{
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

void X11OpenGLDisplay::OnMinimize()
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

void X11OpenGLDisplay::OnMaximize(int pWidth, int pHeight)
{
	Resize(pWidth, pHeight);
	//TODO: port!
	//::ShowWindow(mWnd, SW_SHOWMAXIMIZED);
	Activate();
}

bool X11OpenGLDisplay::InitScreen()
{
	UpdateCaption();

/*TODO: port!

	if (mScreenMode == FULLSCREEN)
	{
		DEVMODE lNewMode;
		lNewMode.dmSize             = sizeof(lNewMode);
		lNewMode.dmBitsPerPel       = mDisplayMode.mBitDepth;
		lNewMode.dmPelsWidth        = mDisplayMode.mWidth;
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
	}

	glEnable(GL_SCISSOR_TEST);
	if (mContextUserCount == 1)
	{
		OpenGLExtensions::InitExtensions();
	}*/

	return true;
}

void X11OpenGLDisplay::UpdateCaption()
{
	str lString = strutil::Format(_T("X11OpenGLDisplay (%ix%i %iBit %iHz) %s"),
		mDisplayMode.mWidth, mDisplayMode.mHeight,
		mDisplayMode.mBitDepth, mDisplayMode.mRefreshRate,
		 _T(LEPRA_STRING_TYPE_TEXT) _T(" ") _T(LEPRA_BUILD_TYPE_TEXT));
	SetCaption(lString, true);
}

bool X11OpenGLDisplay::CreateGLContext()
{
	if (mGlContext == 0)
	{
		mGlContext = ::glXCreateContext(GetDisplay(), GetVisualInfo(), 0, GL_TRUE);
	}

	bool lOk = (mGlContext != 0);
	if (lOk)
	{
		++mContextUserCount;
		lOk = Activate();
	}
	return (lOk);
}

void X11OpenGLDisplay::DeleteGLContext()
{
	if (mContextUserCount >= 1)
	{
		--mContextUserCount;
	}
	if (mContextUserCount == 0)
	{
		::glXMakeCurrent(GetDisplay(), 0, 0);
		::glXDestroyContext(GetDisplay(), mGlContext);
		mGlContext = 0;
	}
}

XVisualInfo* X11OpenGLDisplay::GetVisualInfo() const
{
	int lX11AttributeList[] =
	{
			GLX_RGBA, GLX_DOUBLEBUFFER,
			GLX_RED_SIZE, 1,
			GLX_GREEN_SIZE, 1,
			GLX_BLUE_SIZE, 1,
			None
	};
	return(::glXChooseVisual(GetDisplay(), DefaultScreen(GetDisplay()), lX11AttributeList));
}



GLXContext X11OpenGLDisplay::mGlContext = 0;
int X11OpenGLDisplay::mContextUserCount = 0;



}
