
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/win32/uiwin32opengldisplay.h"
#include <stdio.h>
#include "../../../lepra/include/string.h"
#include "../../include/uilepra.h"
#include "../../include/uiopenglextensions.h"

#pragma comment(lib, "opengl32.lib")



namespace uilepra {



Win32OpenGLDisplay::Win32OpenGLDisplay():
	dc_(0) {
}

Win32OpenGLDisplay::~Win32OpenGLDisplay() {
	CloseScreen();
}

void Win32OpenGLDisplay::CloseScreen() {
	Deactivate();

	if (screen_opened_ == false) {
		return;
	}

	if (IsFullScreen() == true) {
		::ChangeDisplaySettings(NULL, 0);
	}

	DeleteGLContext();

	Win32DisplayManager::CloseScreen();
}

bool Win32OpenGLDisplay::Activate() {
	if (dc_ == 0) {
		dc_ = ::GetDC(wnd_);
	}
	bool ok = (::wglMakeCurrent(dc_, gl_context_) != FALSE);
	return (ok);
}

bool Win32OpenGLDisplay::Deactivate() {
	::wglMakeCurrent(0, 0);
	if (dc_) {
		::ReleaseDC(wnd_, dc_);
		dc_ = 0;
		return true;
	}
	return false;
}

bool Win32OpenGLDisplay::UpdateScreen() {
	if (screen_opened_ == false) {
		return (false);
	}

	// Disable scissor test to make sure that the entire screen will
	// be updated.
	GLboolean scissors_enabled = glIsEnabled(GL_SCISSOR_TEST);
	glDisable(GL_SCISSOR_TEST);

	bool ok = (::SwapBuffers(dc_) != FALSE);

	if (scissors_enabled) {
		glEnable(GL_SCISSOR_TEST);
	}
	return (ok);
}

bool Win32OpenGLDisplay::IsVSyncEnabled() const {
	return OpenGLExtensions::IsVSyncEnabled();
}

bool Win32OpenGLDisplay::SetVSyncEnabled(bool enabled) {
	return (dc_? OpenGLExtensions::SetVSyncEnabled(enabled) : false);
}

DisplayManager::ContextType Win32OpenGLDisplay::GetContextType() {
	return DisplayManager::kOpenglContext;
}

void Win32OpenGLDisplay::SetFocus(bool focus) {
	if (IsFullScreen() == true) {
		if (focus == false) {
			DispatchMinimize();
		} else {
			DispatchResize(display_mode_.width_, display_mode_.height_);
		}
	}
	Parent::SetFocus(focus);
}

void Win32OpenGLDisplay::OnResize(int width, int height) {
	Resize(width, height);
	::ShowWindow(wnd_, SW_SHOWNORMAL);
	Activate();
}

void Win32OpenGLDisplay::Resize(int width, int height) {
	if (IsFullScreen() == true) {
		// Since we are in fullscreen mode, we ignore the width and height
		// given as parameters.
		if (IsMinimized() == true) {
			DEVMODE new_mode;
			new_mode.dmBitsPerPel       = display_mode_.bit_depth_;
			new_mode.dmPelsWidth        = display_mode_.width_;
			new_mode.dmPelsHeight       = display_mode_.height_;
			new_mode.dmDisplayFrequency = display_mode_.refresh_rate_;
			new_mode.dmFields = DM_BITSPERPEL |
					     DM_PELSWIDTH  |
					     DM_PELSHEIGHT |
					     DM_DISPLAYFREQUENCY;

			::ChangeDisplaySettings(&new_mode, CDS_FULLSCREEN);
		}
	} else {
		display_mode_.width_  = width;
		display_mode_.height_ = height;
	}

	glViewport(0, 0, display_mode_.width_, display_mode_.height_);
	UpdateCaption();
}

void Win32OpenGLDisplay::OnMinimize() {
	Deactivate();

	if (IsFullScreen() == true) {
		::ChangeDisplaySettings(NULL, 0);
	}

	::ShowWindow(wnd_, SW_MINIMIZE);
}

void Win32OpenGLDisplay::OnMaximize(int width, int height) {
	Resize(width, height);
	::ShowWindow(wnd_, SW_SHOWMAXIMIZED);
	Activate();
}

bool Win32OpenGLDisplay::InitScreen() {
	UpdateCaption();

	if (screen_mode_ == kFullscreen) {
		DEVMODE new_mode;
		new_mode.dmSize             = sizeof(new_mode);
		new_mode.dmBitsPerPel       = display_mode_.bit_depth_;
		new_mode.dmPelsWidth        = display_mode_.width_;
		new_mode.dmPelsHeight       = display_mode_.height_;
		new_mode.dmDisplayFrequency = display_mode_.refresh_rate_;
		new_mode.dmFields = DM_BITSPERPEL |
							 DM_PELSWIDTH  |
							 DM_PELSHEIGHT |
							 DM_DISPLAYFREQUENCY;

		if (::ChangeDisplaySettings(&new_mode, CDS_FULLSCREEN) !=
		   DISP_CHANGE_SUCCESSFUL) {
			//return false;
		}

		::MoveWindow(wnd_, 0, 0, display_mode_.width_, display_mode_.height_, TRUE);
	} else if(screen_mode_ == DisplayManager::kWindowed ||
		screen_mode_ == DisplayManager::kStaticWindow) {
		// TODO: This stuff is weird! Sometimes it works, and sometimes
		// it doesn't.
		int window_width  = GetWindowWidth(display_mode_.width_);
		int window_height = GetWindowHeight(display_mode_.height_);

		int x = GetSystemMetrics(SM_CXSCREEN) / 2 - display_mode_.width_ / 2;
		int y = GetSystemMetrics(SM_CYSCREEN) / 2 - display_mode_.height_ / 2;

		::MoveWindow(wnd_, x, y, window_width, window_height, TRUE);
	} else {
		int x = GetSystemMetrics(SM_CXSCREEN) / 2 - display_mode_.width_ / 2;
		int y = GetSystemMetrics(SM_CYSCREEN) / 2 - display_mode_.height_ / 2;

		::MoveWindow(wnd_, x, y, display_mode_.width_, display_mode_.height_, TRUE);
	}

	dc_ = ::GetDC(wnd_);

	if (!SetGLPixelFormat()) {
		MessageBox(NULL,
			   "Unable to setup pixel format, Please install a new OpenGL driver," \
			   "or try closing other running applications...",
			   "OpenGL Error" ,MB_OK | MB_ICONWARNING );
		return false;
	}

	if (!CreateGLContext()) {
		MessageBox(NULL,
			   "Unable to setup OpenGL, Please install a new OpenGL driver," \
			   "or try closing other running applications...",
			   "OpenGL Error",MB_OK | MB_ICONWARNING );
		return false;
	}

	//glEnable(GL_SCISSOR_TEST);
	if (context_user_count_ == 1) {
		OpenGLExtensions::InitExtensions();
		::glGetError();	// Clear errors, if any.
	}

	return true;
}

void Win32OpenGLDisplay::UpdateCaption() {
	str s = strutil::Format("Win32OpenGLDisplay (%ix%i %iBit %iHz %s)",
		display_mode_.width_, display_mode_.height_,
		display_mode_.bit_depth_, display_mode_.refresh_rate_,
		 kLepraStringTypeText " " kLepraBuildTypeText);
	SetCaption(s, true);
}

bool Win32OpenGLDisplay::CreateGLContext() {
	if (gl_context_ == 0) {
		gl_context_ = ::wglCreateContext(dc_);
	}

	bool ok = (gl_context_ != 0);
	if (ok) {
		++context_user_count_;
		ok = Activate();
	}
	return (ok);
}

void Win32OpenGLDisplay::DeleteGLContext() {
	if (context_user_count_ >= 1) {
		--context_user_count_;
	}
	if (context_user_count_ == 0) {
		::wglMakeCurrent(0, 0);
		::wglDeleteContext(gl_context_);
		gl_context_ = 0;
	}
}

bool Win32OpenGLDisplay::SetGLPixelFormat() {
	int	gl_pixel_index;
	PIXELFORMATDESCRIPTOR pixel_desc;

	pixel_desc.nSize = sizeof(pixel_desc);
	pixel_desc.nVersion = 1;

	pixel_desc.dwFlags = PFD_DRAW_TO_WINDOW |
			      PFD_SUPPORT_OPENGL |
			      PFD_DOUBLEBUFFER |
			      PFD_STEREO_DONTCARE;

	pixel_desc.iPixelType			= PFD_TYPE_RGBA;
	pixel_desc.cColorBits			= (BYTE)display_mode_.bit_depth_;
	pixel_desc.cRedBits			= 0;
	pixel_desc.cRedShift			= 0;
	pixel_desc.cGreenBits			= 0;
	pixel_desc.cGreenShift			= 0;
	pixel_desc.cBlueBits			= 0;
	pixel_desc.cBlueShift			= 0;
	pixel_desc.cAlphaBits			= 0;
	pixel_desc.cAlphaShift			= 0;
	pixel_desc.cAccumBits			= 0;	// TODO: Implement accumulation buffer?
	pixel_desc.cAccumRedBits		= 0;
	pixel_desc.cAccumGreenBits		= 0;
	pixel_desc.cAccumBlueBits		= 0;
	pixel_desc.cAccumAlphaBits		= 0;
	pixel_desc.cDepthBits			= 32;	// Depth buffer.
	pixel_desc.cStencilBits			= 8;	// Stencil buffer.
	pixel_desc.cAuxBuffers			= 0;
	pixel_desc.iLayerType			= PFD_MAIN_PLANE;
	pixel_desc.bReserved			= 0;
	pixel_desc.dwLayerMask			= 0;
	pixel_desc.dwVisibleMask		= 0;
	pixel_desc.dwDamageMask			= 0;

	gl_pixel_index = ::ChoosePixelFormat(dc_, &pixel_desc);

	if (gl_pixel_index == 0) { // Choose default
		gl_pixel_index = 1;

		if (::DescribePixelFormat(dc_, gl_pixel_index, sizeof(PIXELFORMATDESCRIPTOR), &pixel_desc) == false) {
			return false;
		}
	}

	if (::SetPixelFormat(dc_, gl_pixel_index, &pixel_desc) == false) {
		return false;
	}

	return true;
}

HGLRC Win32OpenGLDisplay::gl_context_ = 0;
int Win32OpenGLDisplay::context_user_count_ = 0;

}
