
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/x11/uix11displaymanager.h"
#include "../../../lepra/include/cyclicarray.h"
#include "../../../lepra/include/log.h"
#include "../../../lepra/include/string.h"
#include "../../../lepra/include/systemmanager.h"
#include "../../include/uilepra.h"
#include "../../include/x11/uix11opengldisplay.h"



namespace uilepra {



DisplayManager* DisplayManager::CreateDisplayManager(ContextType ct) {
	DisplayManager* display_manager = 0;
	switch(ct) {
		case DisplayManager::kOpenglContext:	display_manager = new X11OpenGLDisplay;				break;
		default:				log_.Error("Invalid context type in CreateDisplayManager().");	break;
	}
	return (display_manager);
}

void DisplayManager::EnableScreensaver(bool enable) {
	//BOOL old_value;
	//::SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, enable, &old_value, 0);
}



X11DisplayManager::X11DisplayManager() :
	display_(0),
	wnd_(0),
	is_screen_open_(false),
	is_hidden_(true),
	minimized_(false),
	maximized_(false),
	normal_width_(0),
	normal_height_(0),
	window_x_(0),
	window_y_(0),
	caption_set_(false) {
	display_ = ::XOpenDisplay(0);
	if (display_) {
		++display_use_count_;
		Screen* __screen = DefaultScreenOfDisplay(display_);
		enumerated_display_mode_count_ = 1;
		enumerated_display_mode_ = new DisplayMode[enumerated_display_mode_count_];
		enumerated_display_mode_[0].width_ = WidthOfScreen(__screen);
		enumerated_display_mode_[0].height_ = HeightOfScreen(__screen);
		enumerated_display_mode_[0].bit_depth_ = DefaultDepthOfScreen(__screen);
		enumerated_display_mode_[0].refresh_rate_ = 0;
	}
}

X11DisplayManager::~X11DisplayManager() {
	CloseScreen();

	delete[] enumerated_display_mode_;
	enumerated_display_mode_ = 0;
	enumerated_display_mode_count_ = 0;

	ObserverSetTable::Iterator t_iter;
	for (t_iter = observer_set_table_.First();
		t_iter != observer_set_table_.End();
		++t_iter) {
		ObserverSet* set = *t_iter;
		delete (set);
	}
}

void X11DisplayManager::SetFocus(bool focus) {
	if (focus) {
		::XSetInputFocus(display_, focus, RevertToNone, CurrentTime);
	}
}

unsigned X11DisplayManager::GetWidth() const {
	return display_mode_.width_;
}

unsigned X11DisplayManager::GetHeight() const {
	return display_mode_.height_;
}

unsigned X11DisplayManager::GetBitDepth() const {
	return display_mode_.bit_depth_;
}

unsigned X11DisplayManager::GetRefreshRate() const {
	return display_mode_.refresh_rate_;
}

bool X11DisplayManager::IsFullScreen() const {
	return (screen_mode_ == DisplayManager::kFullscreen);
}

double X11DisplayManager::GetPhysicalScreenSize() const {
	return 23.000;	// Estimated average user's screen size at time of playing any one of my games. Extremely accurate.
}

void X11DisplayManager::SetCaption(const str& caption) {
	SetCaption(caption, false);
}

void X11DisplayManager::SetCaption(const str& caption, bool internal_call) {
	if (internal_call == false) {
		caption_set_ = true;
	}

	if (internal_call == false || caption_set_ == false) {
		::XStoreName(GetDisplay(), GetWindow(), caption.c_str());
	}
}

bool X11DisplayManager::OpenScreen(const DisplayMode& display_mode, ScreenMode screen_mode, Orientation orientation) {
	bool ok = true;

	if (is_screen_open_) {
		log_.Warning("OpenScreen() - Screen already opened.");
		ok = false;
	} else if(display_mode.IsValid() == false && screen_mode == kFullscreen) {
		log_.Error("OpenScreen() - Invalid display mode.");
		ok = false;
	}

	if (ok) {
		screen_mode_ = screen_mode;
		orientation_ = orientation;

		if (screen_mode_ == DisplayManager::kFullscreen) {
			bool supported_mode = false;

			// We can't trust that the user actually knows what he or she is doing,
			// so check if the mode is actually supported.
			if (display_mode.bit_depth_ == 0) {
				supported_mode = FindDisplayMode(display_mode_,
								display_mode.width_,
								display_mode.height_);
			} else if(display_mode.refresh_rate_ == 0) {
				supported_mode = FindDisplayMode(display_mode_,
								display_mode.width_,
								display_mode.height_,
								display_mode.bit_depth_);
			} else {
				supported_mode = FindDisplayMode(display_mode_,
								display_mode.width_,
								display_mode.height_,
								display_mode.bit_depth_,
								display_mode.refresh_rate_);
			}

			if (supported_mode == false) {
				str err(strutil::Format("OpenScreen - Display mode %i-bit %ix%i at %i Hz is not supported!",
						 display_mode.bit_depth_,
						 display_mode.width_,
						 display_mode.height_,
						 display_mode.refresh_rate_));

				log_.Error(err);
				ok = false;
			}
		} else {
			display_mode_ = display_mode;
		}
	}

	if (ok) {
		ok = InitWindow();
	}

	if (ok) {
		ok = is_screen_open_ = InitScreen();
		if (ok) {
			AddObserver(ConfigureNotify, this);
			AddObserver(ClientMessage, this);
		}
	}

	return ok;
}

void X11DisplayManager::CloseScreen() {
	if (is_screen_open_) {
		is_screen_open_ = false;
		X11Core::RemoveDisplayManager(this);
		RemoveObserver(this);

		HideWindow(true);
		::XDestroyWindow(display_, wnd_);
		wnd_ = 0;

		ProcessMessages();

		if (--display_use_count_ == 0) {
			::XCloseDisplay(display_);
		}
		display_ = 0;
	}
}

bool X11DisplayManager::IsVisible() const {
	return !IsMinimized() && !is_hidden_;
}

bool X11DisplayManager::IsFocused() const {
	Window focused;
	int revert_to;
	::XGetInputFocus(display_, &focused, &revert_to);
	return IsVisible() && (focused == wnd_);
}

void X11DisplayManager::HideWindow(bool hide) {
	if (is_hidden_ == hide) {
		return;
	}

	if (hide) {
		Window child;
		::XTranslateCoordinates(display_, wnd_, ::XDefaultRootWindow(display_), 0, 0, &window_x_, &window_y_, &child);
		XWindowAttributes wnd_attr;
		::XGetWindowAttributes(display_, wnd_, &wnd_attr);
		window_x_ -= wnd_attr.x;
		window_y_ -= wnd_attr.y;
		::XUnmapWindow(display_, wnd_);
	} else {
		::XMapWindow(display_, wnd_);
		::XMoveWindow(display_, wnd_, window_x_, window_y_);
	}
	is_hidden_ = hide;
}

bool X11DisplayManager::InitWindow() {
	bool ok = true;

	if (ok && !display_) {
		log_.Error("Display not opened. X started?");
		ok = false;
	}

	if (ok && !wnd_) {
		XVisualInfo* visual_info = GetVisualInfo();
		Screen* __screen = DefaultScreenOfDisplay(display_);
		const int screen_width  = WidthOfScreen(__screen);
		const int screen_height = HeightOfScreen(__screen);

		if (display_mode_.width_ == 0 || display_mode_.height_ == 0) {
			display_mode_.width_  = screen_width;
			display_mode_.height_ = screen_height;
		}

		int _window_width  = display_mode_.width_;
		int _window_height = display_mode_.height_;

		switch(screen_mode_) {
			case DisplayManager::kWindowed:
			case DisplayManager::kStaticWindow: {

				_window_width = GetWindowWidth(_window_width);
				_window_height = GetWindowHeight(_window_height);
			}
			// TRICKY: fall through!
			case DisplayManager::kFullscreen:
			case DisplayManager::kSplashWindow: {
				Colormap col_map = ::XCreateColormap(display_, RootWindow(display_, visual_info->screen), visual_info->visual, AllocNone);
				XSetWindowAttributes win_attr;
				win_attr.colormap = col_map;
				win_attr.border_pixel = 0;
				win_attr.event_mask = StructureNotifyMask |
							FocusChangeMask | EnterWindowMask | LeaveWindowMask |
							KeyPressMask | KeyReleaseMask |
							ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
							Button1MotionMask | Button2MotionMask | Button3MotionMask;
				window_x_ = screen_width/2 - _window_width/2;
				window_y_ = screen_height/2 - _window_height/2;
				wnd_ = ::XCreateWindow(
					display_,
					RootWindow(display_, visual_info->screen),
					window_x_,
					window_y_,
					_window_width,
					_window_height,
					0,
					visual_info->depth,
					InputOutput,
					visual_info->visual,
					CWBorderPixel | CWColormap | CWEventMask,
					&win_attr
				);

				if (wnd_) {
					// Don't delete without telling.
					Atom delete_window = XInternAtom(display_, "WM_DELETE_WINDOW", False);
					XSetWMProtocols(display_, wnd_, &delete_window, 1);

					HideWindow(false);
					XEvent _event;
					::XIfEvent(display_, &_event, WaitForNotify, (char*)wnd_);
				}
			} break;
			default:
			break;
		}

		minimized_ = false;
		maximized_ = false;

		ok = (wnd_ != 0);

		if (!ok) {
			log_.Error("InitWindow() - Failed to create window.");
		}
	}

	if (ok) {
		X11Core::AddDisplayManager(this);
	}

	return ok;
}

void X11DisplayManager::GetBorderSize(int& size_x, int& size_y) {
	if (screen_mode_ == kFullscreen || screen_mode_ == kSplashWindow) {
		size_x = 0;
		size_y = 0;
	} else if(wnd_ != 0) {
		// Use the safest way there is... Taking the difference between
		// the size of the window and the size of the client area.
		// These numbers can't lie.
		XWindowAttributes wnd_attr;
		::XGetWindowAttributes(display_, wnd_, &wnd_attr);
		size_x = wnd_attr.border_width * 2;
		size_y = wnd_attr.border_width * 3 + 20;	// TODO!
	} else {
		// TODO
		size_x = 5*2;
		size_y = 3*3+20;
	}
}

int X11DisplayManager::GetWindowWidth(int client_width) {
	int border_size_x;
	int border_size_y;
	GetBorderSize(border_size_x, border_size_y);
	return client_width + border_size_x;
}

int X11DisplayManager::GetWindowHeight(int client_height) {
	int border_size_x;
	int border_size_y;
	GetBorderSize(border_size_x, border_size_y);
	return client_height + border_size_y;
}

int X11DisplayManager::GetClientWidth(int window_width) {
	int border_size_x;
	int border_size_y;
	GetBorderSize(border_size_x, border_size_y);
	return window_width - border_size_x;
}

int X11DisplayManager::GetClientHeight(int window_height) {
	int border_size_x;
	int border_size_y;
	GetBorderSize(border_size_x, border_size_y);
	return window_height - border_size_y;
}

Display* X11DisplayManager::GetDisplay() const {
	return display_;
}

Window X11DisplayManager::GetWindow() const {
	return wnd_;
}

void X11DisplayManager::ProcessMessages() {
	if (!display_) {
		return;
	}

	while (::XPending(display_) > 0) {
		XEvent _event;
		::XNextEvent(display_, &_event);
		if (!SystemManager::GetQuitRequest()) {
			DispatchMessage(_event);
		}
	}
}

void X11DisplayManager::AddObserver(unsigned message, X11Observer* observer) {
	ObserverSetTable::Iterator t_iter = observer_set_table_.Find(message);
	ObserverSet* set = 0;

	if (t_iter == observer_set_table_.End()) {
		set = new ObserverSet;
		observer_set_table_.Insert(message, set);
	} else {
		set = *t_iter;
	}

	ObserverSet::iterator l_iter = set->find(observer);
	if (l_iter == set->end()) {
		set->insert(observer);
	}
}

void X11DisplayManager::RemoveObserver(unsigned message, X11Observer* observer) {
	ObserverSetTable::Iterator t_iter = observer_set_table_.Find(message);

	if (t_iter != observer_set_table_.End()) {
		ObserverSet* set = *t_iter;
		set->erase(observer);
		if (set->empty()) {
			observer_set_table_.Remove(t_iter);
			delete (set);
		}
	}
}

void X11DisplayManager::RemoveObserver(X11Observer* observer) {
	ObserverSetTable::Iterator t_iter = observer_set_table_.First();

	while (t_iter != observer_set_table_.End()) {
		ObserverSet* set = *t_iter;
		ObserverSet::iterator l_iter = set->find(observer);
		if (l_iter != set->end()) {
			set->erase(l_iter);
			if (set->empty()) {
				observer_set_table_.Remove(t_iter++);
				delete (set);
			} else {
				++t_iter;
			}
		} else {
			++t_iter;
		}
	}
}

void X11DisplayManager::ShowMessageBox(const str& msg, const str& caption) {
	//::MessageBox(wnd_, msg.c_str(), caption.c_str(), MB_OK);
}

bool X11DisplayManager::OnMessage(const XEvent& event) {
	switch(event.type) {
		case ConfigureNotify: {
			const XConfigureEvent& config_event = (const XConfigureEvent&)event;
			if (config_event.width != display_mode_.width_ || config_event.height != display_mode_.height_) {
				DispatchResize(config_event.width, config_event.height);
			}

			/*switch(param) {
				case SIZE_MINIMIZED: {
					DispatchMinimize();
					minimized_ = true;
					maximized_ = false;
				} break;
				case SIZE_MAXIMIZED: {
					normal_width_  = display_mode_.width_;
					normal_height_ = display_mode_.height_;
					DispatchMaximize((int)LOWORD(param), (int)HIWORD(param));
					maximized_ = true;
					minimized_ = false;
				} break;
				case SIZE_RESTORED: {
					DispatchResize((int)LOWORD(param), (int)HIWORD(param));
					minimized_ = false;
					maximized_ = false;
				} break;
			}*/
		} break;
		case ClientMessage: {
			if(event.xclient.data.l[0] == XInternAtom(display_, "WM_DELETE_WINDOW", False)) {
				SystemManager::AddQuitRequest(1);
			}
		} break;
	}

	return false;
}

bool X11DisplayManager::DispatchMessage(const XEvent& event) {
	//log_.Infof("X message: %i", event.type);

	bool consumed = false;
	ObserverSetTable::Iterator t_iter = observer_set_table_.Find(event.type);
	if (t_iter != observer_set_table_.End()) {
		ObserverSet* set = *t_iter;
		ObserverSet::iterator l_iter;
		for (l_iter = set->begin(); l_iter != set->end(); ++l_iter) {
			consumed |= (*l_iter)->OnMessage(event);
		}
	}
	return consumed;
}

Bool X11DisplayManager::WaitForNotify(Display* d, XEvent* e, char* arg) {
	return((e->type == MapNotify) && (e->xmap.window == (::Window)arg));
}

XVisualInfo* X11DisplayManager::GetVisualInfo() const {
	int x11_attribute_list[] =
	{
			GLX_RGBA,
			GLX_RED_SIZE, 1,
			GLX_GREEN_SIZE, 1,
			GLX_BLUE_SIZE, 1,
			GLX_DOUBLEBUFFER,
			GLX_DEPTH_SIZE, 24,
			GLX_STENCIL_SIZE, 8,
			None
	};
	int attribute_offsets[] = {12, 10, 8, 7, 1, 0};
	for (int x = 0; x < LEPRA_ARRAY_COUNT(attribute_offsets); ++x) {
		x11_attribute_list[attribute_offsets[x]] = None;
		XVisualInfo* __visual = ::glXChooseVisual(display_, DefaultScreen(display_), x11_attribute_list);
		if (__visual) {
			return __visual;
		}
	}
	return 0;
}



int X11DisplayManager::display_use_count_ = 0;
loginstance(kUiGfx, X11DisplayManager);



}
