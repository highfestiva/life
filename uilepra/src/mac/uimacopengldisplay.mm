
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../include/mac/uimacopengldisplay.h"
#include <stdio.h>
#include "../../../lepra/include/string.h"
#ifdef LEPRA_IOS
#include "../../include/mac/rotatingcontroller.h"
#endif // iOS
#include "../../include/uiopenglextensions.h"



namespace uilepra {



MacOpenGLDisplay::MacOpenGLDisplay():
	gl_view_(0) {
}

MacOpenGLDisplay::~MacOpenGLDisplay() {
	CloseScreen();
}

void MacOpenGLDisplay::CloseScreen() {
	Deactivate();

	if (!is_open_) {
		return;
	}

	/*if (IsFullScreen() == true) {
		TODO: port!
		::ChangeDisplaySettings(NULL, 0);
	}*/

	DeleteGLContext();

	MacDisplayManager::CloseScreen();
}

bool MacOpenGLDisplay::Activate() {
#ifdef LEPRA_IOS
	gl_view_.context = gl_context_;
	[gl_view_ framebuffer_];
#else // !iOS
	[gl_context_ makeCurrentContext];
#endif // iOS/!iOS
	return true;
}

bool MacOpenGLDisplay::Deactivate() {
	if (is_open_) {
#ifdef LEPRA_IOS
		[LEPRA_APPLE_GL_CONTEXT setCurrentContext:nil];
#else // !iOS
		[LEPRA_APPLE_GL_CONTEXT clearCurrentContext];
#endif // iOS/!iOS
	}
	return is_open_;
}

bool MacOpenGLDisplay::UpdateScreen() {
	if (!is_open_) {
		return (false);
	}

	// Disable scissor test to make sure that the entire screen will
	// be updated.
	GLboolean scissors_enabled = ::glIsEnabled(GL_SCISSOR_TEST);
	::glDisable(GL_SCISSOR_TEST);

#ifdef LEPRA_IOS
	[gl_view_ presentFramebuffer];
#else // !iOS
	[gl_context_ flushBuffer];
#endif // iOS/!iOS

	if (scissors_enabled) {
		::glEnable(GL_SCISSOR_TEST);
	}

	return (true);
}

void MacOpenGLDisplay::SetOrientation(Orientation orientation) {
	Parent::SetOrientation(orientation);
#ifdef LEPRA_IOS
	gl_view_.orientationStrictness = (int)orientation_;
#endif // iOS
}



bool MacOpenGLDisplay::IsVSyncEnabled() const {
	return true; //OpenGLExtensions::IsVSyncEnabled();
}

bool MacOpenGLDisplay::SetVSyncEnabled(bool enabled) {
	return true;
//	return (is_open_? OpenGLExtensions::SetVSyncEnabled(enabled) : false);
}

DisplayManager::ContextType MacOpenGLDisplay::GetContextType() {
	return DisplayManager::kOpenglContext;
}

LEPRA_APPLE_GL_VIEW* MacOpenGLDisplay::GetGlView() const {
	return gl_view_;
}

void MacOpenGLDisplay::SetFocus(bool focus) {
	if (IsFullScreen() == true) {
		if (focus == false) {
			DispatchMinimize();
		} else {
			DispatchResize(display_mode_.width_, display_mode_.height_);
		}
	}
}

void MacOpenGLDisplay::OnResize(int width, int height) {
	Resize(width, height);
	//TODO: Mac resize!
	//::ShowWindow(wnd_, SW_SHOWNORMAL);
	Activate();
}

void MacOpenGLDisplay::Resize(int width, int height) {
	if (!gl_view_) {
		return;
	}

	if (IsFullScreen() == true) {
		// Since we are in fullscreen mode, we ignore the width and height
		// given as parameters.
		if (IsMinimized() == true) {
			//TODO: something!
			//::ChangeDisplaySettings(&new_mode, CDS_FULLSCREEN);
		}
	} else {
		display_mode_.width_  = width;
		display_mode_.height_ = height;
	}

	::glViewport(0, 0, display_mode_.width_, display_mode_.height_);
	UpdateCaption();
}

void MacOpenGLDisplay::OnMinimize() {
	Deactivate();

	if (IsFullScreen() == true) {
		//TODO: something!
		//::ChangeDisplaySettings(NULL, 0);
	}

	// TODO:
	//::ShowWindow(wnd_, SW_MINIMIZE);
}

void MacOpenGLDisplay::OnMaximize(int width, int height) {
	Resize(width, height);
	//TODO: port!
	//::ShowWindow(wnd_, SW_SHOWMAXIMIZED);
	Activate();
}

bool MacOpenGLDisplay::InitScreen() {
	UpdateCaption();



/*TODO: port!

	if (screen_mode_ == kFullscreen) {
		DEVMODE new_mode;
		new_mode.dmSize	     = sizeof(new_mode);
		new_mode.dmBitsPerPel       = display_mode_.bit_depth_;
		new_mode.dmPelsWidth	= display_mode_.width_;
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
	}*/

	if (!SetGLPixelFormat()) {
		printf("OpenGL error: unable to setup pixel format. Please try installing a new OpenGL driver, " \
			"or try closing other running applications.");
		return false;
	}

	if (!CreateGLContext()) {
		printf("OpenGL error: unable to create context. Please try installing a new OpenGL driver, " \
			"or try closing other running applications.");
		return false;
	}

	::glEnable(GL_SCISSOR_TEST);
	if (context_user_count_ == 1) {
		OpenGLExtensions::InitExtensions();
	}

#ifndef LEPRA_IOS
	[wnd_ setContentSize:wnd_.frame.size];
#endif // !iOS

	return true;
}

void MacOpenGLDisplay::UpdateCaption() {
	str s = strutil::Format("MacOpenGLDisplay (%ix%i %iBit %iHz %s)",
		display_mode_.width_, display_mode_.height_,
		display_mode_.bit_depth_, display_mode_.refresh_rate_,
		kLepraStringTypeText " " kLepraBuildTypeText);
	SetCaption(s, true);
}

bool MacOpenGLDisplay::CreateGLContext() {
	if (gl_context_ == 0) {
#ifdef LEPRA_IOS
		gl_context_ = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		[EAGLContext setCurrentContext:gl_context_];
		switch (orientation_) {

		}
#else // !iOS
		gl_context_ = [gl_view_ openGLContext];
#endif // iOS/!iOS
	}

	bool ok = (gl_context_ != 0);
	if (ok) {
		++context_user_count_;
		ok = Activate();
	}
	return (ok);
}

void MacOpenGLDisplay::DeleteGLContext() {
	if (context_user_count_ >= 1) {
		--context_user_count_;
	}
	if (context_user_count_ == 0) {
		Deactivate();
		gl_context_ = 0;
	}
}



bool MacOpenGLDisplay::SetGLPixelFormat() {
#ifdef LEPRA_IOS
	gl_view_ = [[EAGLView alloc] initWithFrame:wnd_.bounds];
	gl_view_.orientationStrictness = (int)orientation_;
	if (IsFullScreen()) {
		// Add the controller, and add the subview by setting root controller.
		RotatingController* controller = [[RotatingController alloc] init];
		controller.navigationBarHidden = YES;
		controller.view = gl_view_;
		wnd_.rootViewController = controller;
		[wnd_ makeKeyAndVisible];	// Only visible after we add the view.
	}
#else // !iOS
	NSOpenGLPixelFormatAttribute pixel_format_attribs[32];
	NSOpenGLPixelFormatAttribute* attrib = pixel_format_attribs;

	*attrib++ = NSOpenGLPFANoRecovery;
	*attrib++ = NSOpenGLPFADoubleBuffer;
	*attrib++ = NSOpenGLPFAAccelerated;
	//      *attrib++ = kCGLPFAFullScreen;
	*attrib++ = NSOpenGLPFAAccumSize;
	*attrib++ = (NSOpenGLPixelFormatAttribute)display_mode_.bit_depth_;
	*attrib++ = NSOpenGLPFAColorSize;
	*attrib++ = (NSOpenGLPixelFormatAttribute)display_mode_.bit_depth_;
	if (display_mode_.bit_depth_ > 0) {
		*attrib++ = NSOpenGLPFADepthSize;
		*attrib++ = (NSOpenGLPixelFormatAttribute)display_mode_.bit_depth_;
	}
	if (true) {	// TODO: stencil depth somehow?
		*attrib++ = NSOpenGLPFAStencilSize;
		*attrib++ = (NSOpenGLPixelFormatAttribute)8;
	}
	*attrib++ = (NSOpenGLPixelFormatAttribute) 0;
	NSOpenGLPixelFormat* pixel_format = [NSOpenGLPixelFormat alloc];
	[pixel_format initWithAttributes:pixel_format_attribs];

	gl_view_ = [NSOpenGLView alloc];
	//CGRect content_size = [wnd_ contentSize];
	[gl_view_ initWithFrame:wnd_.frame pixelFormat:pixel_format];
	[wnd_ setContentView:gl_view_];
	[wnd_ setAcceptsMouseMovedEvents: YES];
	[wnd_ setIgnoresMouseEvents: NO];
	//[gl_view_ setAutoresizingMask:NSViewWidthSizable|NSViewHeightSizable];
	//[gl_view_ setAutoresizesSubviews:YES];
	//[wnd_ setFrame: NSMakeRect(0, 0, screen_->GetActiveResolution().GetWidth(), screen_->GetActiveResolution().GetHeight()) display: YES];
	//gl_view_.frame = content_size;

	//[gl_view_ reshape];
#endif // iOS/!iOS
	return (true);	// TODO: add error checking!
}



LEPRA_APPLE_GL_CONTEXT* MacOpenGLDisplay::gl_context_ = 0;
int MacOpenGLDisplay::context_user_count_ = 0;



}
