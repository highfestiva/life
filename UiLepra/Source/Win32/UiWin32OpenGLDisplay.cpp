/*
	Class:  Win32OpenGLDisplay
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include <stdio.h>
#include "../../../Lepra/Include/String.h"
#include "../../Include/UiLepra.h"
#include "../../Include/Win32/UiWin32OpenGLDisplay.h"

// OpenGL headers included in OpenGLExtensions.h.
#include "../../Include/UiOpenGLExtensions.h"

#pragma comment(lib, "opengl32.lib")

namespace UiLepra
{

Win32OpenGLDisplay::Win32OpenGLDisplay():
	mDC(0)
{
}

Win32OpenGLDisplay::~Win32OpenGLDisplay()
{
	if (mScreenOpened == true) 
	{
		CloseScreen();
	}
}

void Win32OpenGLDisplay::CloseScreen()
{
	Deactivate();

	if (mScreenOpened == false)
	{
		return;
	}

	if (IsFullScreen() == true)
	{
		::ChangeDisplaySettings(NULL, 0);
	}

	DeleteGLContext();

	Win32DisplayManager::CloseScreen();
}

bool Win32OpenGLDisplay::Activate()
{
	if (mDC == 0)
	{
		mDC = ::GetDC(mWnd);
	}
	bool lOk = (::wglMakeCurrent(mDC, msGlContext) != FALSE);
	return (lOk);
}

bool Win32OpenGLDisplay::UpdateScreen()
{
	if (mScreenOpened == false)
	{
		return (false);
	}

	// Disable scissor test to make sure that the entire screen will
	// be updated.
	GLboolean lScissorsEnabled = glIsEnabled(GL_SCISSOR_TEST);
	glDisable(GL_SCISSOR_TEST);

	bool lOk = (::SwapBuffers(mDC) != FALSE);

	if (lScissorsEnabled)
	{
		glEnable(GL_SCISSOR_TEST);
	}
	return (lOk);
}

bool Win32OpenGLDisplay::IsVSyncEnabled() const
{
	return OpenGLExtensions::IsVSyncEnabled();
}

bool Win32OpenGLDisplay::SetVSyncEnabled(bool pEnabled)
{
	return (mDC? OpenGLExtensions::SetVSyncEnabled(pEnabled) : false);
}

unsigned Win32OpenGLDisplay::GetPitch()
{
	return mDisplayMode.mWidth;
}

void Win32OpenGLDisplay::SetFocus(bool pFocus)
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

void Win32OpenGLDisplay::Deactivate()
{
	::wglMakeCurrent(0, 0);
	if (mDC)
	{
		::ReleaseDC(mWnd, mDC);
		mDC = 0;
	}
}

void Win32OpenGLDisplay::OnResize(int pWidth, int pHeight)
{
	Resize(pWidth, pHeight);
	::ShowWindow(mWnd, SW_SHOWNORMAL);
}

void Win32OpenGLDisplay::Resize(int pWidth, int pHeight)
{
	if (IsFullScreen() == true)
	{
		// Since we are in fullscreen mode, we ignore the width and height
		// given as parameters.
		if (IsMinimized() == true)
		{
			DEVMODE lNewMode;
			lNewMode.dmBitsPerPel       = mDisplayMode.mBitDepth;
			lNewMode.dmPelsWidth        = mDisplayMode.mWidth;
			lNewMode.dmPelsHeight       = mDisplayMode.mHeight;
			lNewMode.dmDisplayFrequency = mDisplayMode.mRefreshRate;
			lNewMode.dmFields = DM_BITSPERPEL | 
					     DM_PELSWIDTH  | 
					     DM_PELSHEIGHT | 
					     DM_DISPLAYFREQUENCY;

			::ChangeDisplaySettings(&lNewMode, CDS_FULLSCREEN);
		}
	}
	else
	{
		mDisplayMode.mWidth  = pWidth;
		mDisplayMode.mHeight = pHeight;
	}

	glViewport(0, 0, mDisplayMode.mWidth, mDisplayMode.mHeight);
	UpdateCaption();
}

void Win32OpenGLDisplay::OnMinimize()
{
	Deactivate();

	if (IsFullScreen() == true)
	{
		::ChangeDisplaySettings(NULL, 0);
	}

	::ShowWindow(mWnd, SW_MINIMIZE);
}

void Win32OpenGLDisplay::OnMaximize(int pWidth, int pHeight)
{
	Resize(pWidth, pHeight);
	::ShowWindow(mWnd, SW_SHOWMAXIMIZED);
}

bool Win32OpenGLDisplay::InitScreen()
{
	UpdateCaption();

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
	if (msContextUserCount == 1)
	{
		OpenGLExtensions::InitExtensions();
	}

	return true;
}

void Win32OpenGLDisplay::UpdateCaption()
{
	Lepra::String lString = Lepra::StringUtility::Format(_T("Win32OpenGLDisplay (%ix%i %iBit %iHz) %s"),
		mDisplayMode.mWidth, mDisplayMode.mHeight,
		mDisplayMode.mBitDepth, mDisplayMode.mRefreshRate,
		 _T(LEPRA_STRING_TYPE_TEXT) _T(" ") _T(LEPRA_BUILD_TYPE_TEXT));
	SetCaption(lString, true);
}

bool Win32OpenGLDisplay::CreateGLContext()
{
	if (msGlContext == 0)
	{
		msGlContext = ::wglCreateContext(mDC);
	}

	bool lOk = (msGlContext != 0);
	if (lOk)
	{
		++msContextUserCount;
		lOk = Activate();
	}
	return (lOk);
}

void Win32OpenGLDisplay::DeleteGLContext()
{
	if (msContextUserCount >= 1)
	{
		--msContextUserCount;
	}
	if (msContextUserCount == 0)
	{
		::wglMakeCurrent(0, 0);
		::wglDeleteContext(msGlContext);
		msGlContext = 0;
	}
}

bool Win32OpenGLDisplay::SetGLPixelFormat()
{
	int	lGLPixelIndex;
	PIXELFORMATDESCRIPTOR lPixelDesc;

	lPixelDesc.nSize = sizeof(lPixelDesc);
	lPixelDesc.nVersion = 1;

	lPixelDesc.dwFlags = PFD_DRAW_TO_WINDOW | 
			      PFD_SUPPORT_OPENGL |
			      PFD_DOUBLEBUFFER |
			      PFD_STEREO_DONTCARE;

	lPixelDesc.iPixelType			= PFD_TYPE_RGBA;
	lPixelDesc.cColorBits			= (BYTE)mDisplayMode.mBitDepth;
	lPixelDesc.cRedBits			= 0;
	lPixelDesc.cRedShift			= 0;
	lPixelDesc.cGreenBits			= 0;
	lPixelDesc.cGreenShift			= 0;
	lPixelDesc.cBlueBits			= 0;
	lPixelDesc.cBlueShift			= 0;
	lPixelDesc.cAlphaBits			= 0;
	lPixelDesc.cAlphaShift			= 0;
	lPixelDesc.cAccumBits			= 0;	// TODO: Implement accumulation buffer?
	lPixelDesc.cAccumRedBits		= 0;
	lPixelDesc.cAccumGreenBits		= 0;
	lPixelDesc.cAccumBlueBits		= 0;
	lPixelDesc.cAccumAlphaBits		= 0;
	lPixelDesc.cDepthBits			= 32;	// Depth buffer.
	lPixelDesc.cStencilBits		= 8;	// Stencil buffer.
	lPixelDesc.cAuxBuffers			= 0;
	lPixelDesc.iLayerType			= PFD_MAIN_PLANE;
	lPixelDesc.bReserved			= 0;
	lPixelDesc.dwLayerMask			= 0;
	lPixelDesc.dwVisibleMask		= 0;
	lPixelDesc.dwDamageMask		= 0;

	lGLPixelIndex = ::ChoosePixelFormat(mDC, &lPixelDesc);

	if (lGLPixelIndex == 0) // Choose default
	{
		lGLPixelIndex = 1;

		if (::DescribePixelFormat(mDC, lGLPixelIndex, sizeof(PIXELFORMATDESCRIPTOR), &lPixelDesc) == false)
		{
			return false;
		}
	}

	if (::SetPixelFormat(mDC, lGLPixelIndex, &lPixelDesc) == false)
	{
		return false;
	}

	return true;
}

HGLRC Win32OpenGLDisplay::msGlContext = 0;
int Win32OpenGLDisplay::msContextUserCount = 0;

}
