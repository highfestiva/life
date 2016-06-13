
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/win32/uiwin32displaymanager.h"
#include "../../../lepra/include/log.h"
#include "../../../lepra/include/string.h"
#include "../../../lepra/include/systemmanager.h"
#include "../../include/uilepra.h"
#include "../../include/win32/uiwin32directxdisplay.h"
#include "../../include/win32/uiwin32opengldisplay.h"

#define kIdiMainIcon 1001	// This must be identical to resource definition in the .rc file.



namespace uilepra {



DisplayManager* DisplayManager::CreateDisplayManager(ContextType ct) {
	DisplayManager* display_manager = 0;
	switch(ct) {
		case DisplayManager::kOpenglContext:	display_manager = new Win32OpenGLDisplay;				break;
		case DisplayManager::kDirectxContext:	display_manager = new Win32DirectXDisplay;				break;
		default:				log_.Error("Invalid context type in CreateDisplayManager().");		break;
	}
	return (display_manager);
}

void DisplayManager::EnableScreensaver(bool enable) {
	BOOL old_value;
	::SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, enable, &old_value, 0);
}



Win32DisplayManager::Win32DisplayManager() :
	wnd_(0),
	initialized_(false),
	screen_opened_(false),
	minimized_(false),
	maximized_(false),
	normal_width_(0),
	normal_height_(0),
	caption_set_(false),
	consume_char_(true) {
	// Count display modes.
	DEVMODE dev_mode;
	while (EnumDisplaySettings(NULL, enumerated_display_mode_count_, &dev_mode) != 0) {
		enumerated_display_mode_count_++;
	}
	enumerated_display_mode_ = new DisplayMode[enumerated_display_mode_count_];

	int count = 0;
	while (EnumDisplaySettings(NULL, count, &dev_mode) != 0) {
		enumerated_display_mode_[count].width_ = dev_mode.dmPelsWidth;
		enumerated_display_mode_[count].height_ = dev_mode.dmPelsHeight;
		enumerated_display_mode_[count].bit_depth_ = dev_mode.dmBitsPerPel;
		enumerated_display_mode_[count].refresh_rate_ = dev_mode.dmDisplayFrequency;

		count++;
	}

	Register();
}

Win32DisplayManager::~Win32DisplayManager() {
	CloseScreen();

	Unregister();

	ObserverSetTable::Iterator t_iter;
	for (t_iter = observer_set_table_.First();
		t_iter != observer_set_table_.End();
		++t_iter) {
		ObserverSet* set = *t_iter;
		delete (set);
	}
}

void Win32DisplayManager::SetFocus(bool focus) {
	if (focus) {
		//::BringWindowToTop(wnd_);
		//::SwitchToThisWindow(wnd_, FALSE);
		::SetWindowPos(wnd_, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
		::SetWindowPos(wnd_, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
		::SetForegroundWindow(wnd_);
		//HWND_NOTOPMOST
	}
}

bool Win32DisplayManager::Register() {
	bool ok = true;
	if (register_count_ == 0) {
		++register_count_;

		this_instance_ = Win32Core::GetAppInstance();

		// Register window class.
		window_class_.cbSize		= sizeof(window_class_);
		window_class_.cbClsExtra	= 0;
		window_class_.cbWndExtra	= 0;
		window_class_.hbrBackground	= GetStockBrush(BLACK_BRUSH);
		window_class_.style			= CS_DBLCLKS;
		window_class_.lpfnWndProc	= WndProc;
		window_class_.hInstance		= (HINSTANCE)this_instance_;
		window_class_.hIcon			= ::LoadIcon((HINSTANCE)this_instance_, MAKEINTRESOURCE(kIdiMainIcon));
		window_class_.hIconSm		= ::LoadIcon((HINSTANCE)this_instance_, MAKEINTRESOURCE(kIdiMainIcon));
		window_class_.hCursor		= ::LoadCursor(0, IDC_ARROW);
		window_class_.lpszMenuName	= NULL;
		window_class_.lpszClassName	= "LepraWin32Class";

		ok = (RegisterClassEx(&window_class_) != 0);
	}
	if (ok) {
		display_mode_.width_	= 0;
		display_mode_.height_	= 0;
		display_mode_.bit_depth_	= 0;
		initialized_		= true;
	} else {
		log_.Error("Register() - failed to register window class.");
	}
	return ok;
}

void Win32DisplayManager::Unregister() {
	--register_count_;
	if (register_count_ == 0) {
		::UnregisterClass("LepraWin32Class", (HINSTANCE)this_instance_);
	}
}

unsigned Win32DisplayManager::GetWidth() const {
	return display_mode_.width_;
}

unsigned Win32DisplayManager::GetHeight() const {
	return display_mode_.height_;
}

unsigned Win32DisplayManager::GetBitDepth() const {
	return display_mode_.bit_depth_;
}

unsigned Win32DisplayManager::GetRefreshRate() const {
	return display_mode_.refresh_rate_;
}

bool Win32DisplayManager::IsFullScreen() const {
	return (screen_mode_ == DisplayManager::kFullscreen);
}

double Win32DisplayManager::GetPhysicalScreenSize() const {
	return 23.000;	// Estimated average user's screen size at time of playing any one of my games. Extremely accurate.
}

void Win32DisplayManager::SetCaption(const str& caption) {
	SetCaption(caption, false);
}

void Win32DisplayManager::SetCaption(const str& caption, bool internal_call) {
	if (initialized_) {
		if (internal_call == false) {
			caption_set_ = true;
		}

		if (internal_call == false || caption_set_ == false) {
			::SetWindowText(wnd_, caption.c_str());
		}
	}
}

bool Win32DisplayManager::OpenScreen(const DisplayMode& display_mode, ScreenMode screen_mode, Orientation orientation) {
	bool ok = true;

	if (initialized_ == false) {
		log_.Warning("OpenScreen() - DisplayManager not initialized.");
		ok = false;
	} else if(screen_opened_ == true) {
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
				str err(strutil::Format("OpenScreen() - Display mode %i-bit %ix%i at %i Hz is not supported!",
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
		screen_opened_ = InitScreen();

		if (screen_opened_ == true) {
			AddObserver(WM_SIZE, this);
			AddObserver(WM_SIZING, this);
		} else {
			ok = false;
		}
	}

	return ok;
}

void Win32DisplayManager::CloseScreen() {
	if (screen_opened_) {
		screen_opened_ = false;
		Win32Core::RemoveDisplayManager(this);
		RemoveObserver(this);

		::DestroyWindow(wnd_);
		wnd_ = 0;

		// Repaint the desktop to restore background.
		RECT rect;
		::GetWindowRect(GetDesktopWindow(), &rect);
		::MoveWindow(GetDesktopWindow(),
			rect.left,
			rect.top,
			rect.right - rect.left,
			rect.bottom - rect.top,
			TRUE);
	}
}

bool Win32DisplayManager::IsVisible() const {
	return !IsMinimized() && ::IsWindowVisible(wnd_);
}

bool Win32DisplayManager::IsFocused() const {
	return IsVisible() && (::GetActiveWindow() == wnd_);
}

void Win32DisplayManager::HideWindow(bool hide) {
	::ShowWindow(wnd_, hide? SW_HIDE : SW_SHOW);
}

bool Win32DisplayManager::InitWindow() {
	bool ok = initialized_;

	if (ok) {
		if (display_mode_.width_ == 0 || display_mode_.height_ == 0) {
			display_mode_.width_  = GetSystemMetrics(SM_CXSCREEN);
			display_mode_.height_ = GetSystemMetrics(SM_CYSCREEN);
		}

		int _window_width  = display_mode_.width_;
		int _window_height = display_mode_.height_;

		// Create window
		if (wnd_ == 0) {
			switch(screen_mode_) {
				case DisplayManager::kFullscreen:
				case DisplayManager::kSplashWindow: {
					wnd_ = ::CreateWindowEx(WS_EX_APPWINDOW | WS_EX_TOPMOST,
								"LepraWin32Class", "Lepra",
								WS_POPUP | WS_CLIPSIBLINGS | WS_VISIBLE,
								GetSystemMetrics(SM_CXSCREEN) / 2 - _window_width / 2,
								GetSystemMetrics(SM_CYSCREEN) / 2 - _window_height / 2,
								_window_width, _window_height,
								GetDesktopWindow(), NULL, (HINSTANCE)this_instance_, NULL);
				} break;
				case DisplayManager::kWindowed:
				case DisplayManager::kStaticWindow: {
					_window_width = GetWindowWidth(_window_width);
					_window_height = GetWindowHeight(_window_height);
					DWORD __style = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;
					if (screen_mode_ == DisplayManager::kWindowed) {
						__style |= (WS_SIZEBOX | WS_MAXIMIZEBOX);
					}
					wnd_ = ::CreateWindowEx(WS_EX_APPWINDOW,
						"LepraWin32Class", "Lepra",
						__style,
						GetSystemMetrics(SM_CXSCREEN) / 2 - _window_height / 2,
						GetSystemMetrics(SM_CYSCREEN) / 2 - _window_height / 2,
						_window_width, _window_height,
						NULL, NULL, (HINSTANCE)this_instance_, NULL);
				} break;
				default:
				break;
			}

			::ShowWindow(wnd_, SW_SHOW);
			::UpdateWindow(wnd_);
			minimized_ = false;
			maximized_ = false;
		}

		ok = (wnd_ != 0);

		if (ok) {
			++window_count_;
		} else {
			log_.Error("InitWindow() - Failed to create window.");
		}
	}

	if (ok) {
		Win32Core::AddDisplayManager(this);
	}

	return ok;
}

void Win32DisplayManager::GetBorderSize(int& size_x, int& size_y) {
	if (screen_mode_ == kFullscreen ||
	   screen_mode_ == kSplashWindow) {
		size_x = 0;
		size_y = 0;
	} else if(wnd_ != 0) {
		// Use the safest way there is... Taking the difference between
		// the size of the window and the size of the client area.
		// These numbers can't lie.
		RECT client_rect;
		RECT window_rect;
		::GetClientRect(wnd_, &client_rect);
		::GetWindowRect(wnd_, &window_rect);

		size_x = ((window_rect.right - window_rect.left) -
					(client_rect.right - client_rect.left));

		size_y = ((window_rect.bottom - window_rect.top) -
					(client_rect.bottom - client_rect.top));
	} else {
		// We have to use the awful system function GetSystemMetrics().
		// I don't trust that function at all, or any of the values it returns.
		if (screen_mode_ == kWindowed) {
			size_x = ::GetSystemMetrics(SM_CXSIZEFRAME) * 2;
			size_y = ::GetSystemMetrics(SM_CYSIZEFRAME) * 2 +
					   ::GetSystemMetrics(SM_CYCAPTION);
		} else {
			// Static window.
			size_x = ::GetSystemMetrics(SM_CXFIXEDFRAME) * 2;
			size_y = ::GetSystemMetrics(SM_CYFIXEDFRAME) * 2 +
					   ::GetSystemMetrics(SM_CYCAPTION);
		}
	}
}

int Win32DisplayManager::GetWindowWidth(int client_width) {
	int border_size_x;
	int border_size_y;
	GetBorderSize(border_size_x, border_size_y);

	return client_width + border_size_x;
}

int Win32DisplayManager::GetWindowHeight(int client_height) {
	int border_size_x;
	int border_size_y;
	GetBorderSize(border_size_x, border_size_y);

	return client_height + border_size_y;
}

int Win32DisplayManager::GetClientWidth(int window_width) {
	int border_size_x;
	int border_size_y;
	GetBorderSize(border_size_x, border_size_y);

	return window_width - border_size_x;
}

int Win32DisplayManager::GetClientHeight(int window_height) {
	int border_size_x;
	int border_size_y;
	GetBorderSize(border_size_x, border_size_y);

	return window_height - border_size_y;
}

HWND Win32DisplayManager::GetHWND() {
	return wnd_;
}

LRESULT CALLBACK Win32DisplayManager::WndProc(HWND wnd, unsigned int message, unsigned int wparam, LONG lparam) {
	if (message  == WM_QUIT ||
		message == WM_DESTROY ||
		message == WM_CLOSE) {
		if (window_count_ > 0) {
			--window_count_;
		}
		if (window_count_ == 0) {
			SystemManager::AddQuitRequest(+1);
			// First close attempt.
			if (message == WM_CLOSE && SystemManager::GetQuitRequest() <= 1) {
				return (TRUE);
			}
		}
	}

	bool message_was_consumed = false;
	Win32DisplayManager* display_manager = Win32Core::GetDisplayManager(wnd);
	if (display_manager) {
		message_was_consumed = display_manager->InternalDispatchMessage(message, wparam, lparam);
	}
	LRESULT result = 0;
	if (!message_was_consumed) {
		result = DefWindowProc(wnd, message, wparam, lparam);
	}
	return (result);
}

bool Win32DisplayManager::InternalDispatchMessage(int message, int wparam, long lparam) {
	if (message == WM_CHAR && consume_char_) {	// Consume all follow-up WM_CHAR's when we already dispatched the WM_KEYDOWN.
		return (true);
	}

	bool consumed = false;
	ObserverSetTable::Iterator t_iter = observer_set_table_.Find(message);
	if (t_iter != observer_set_table_.End()) {
		ObserverSet* set = *t_iter;
		ObserverSet::iterator l_iter;
		for (l_iter = set->begin(); l_iter != set->end(); ++l_iter) {
			consumed |= (*l_iter)->OnMessage(message, wparam, lparam);
		}
	}
	if (message == WM_KEYDOWN) {
		consume_char_ = consumed;
	}
	return (consumed);
}

void Win32DisplayManager::ProcessMessages() {
	if (wnd_ != 0) {
		MSG _msg;
		while (::PeekMessage(&_msg, wnd_, 0, 0, PM_REMOVE) == TRUE) {
			::TranslateMessage(&_msg);
			::DispatchMessage(&_msg);
		}
	}
}

void Win32DisplayManager::AddObserver(unsigned int message, Win32Observer* observer) {
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

void Win32DisplayManager::RemoveObserver(unsigned int message, Win32Observer* observer) {
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

void Win32DisplayManager::RemoveObserver(Win32Observer* observer) {
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

void Win32DisplayManager::ShowMessageBox(const str& msg, const str& caption) {
	::MessageBox(wnd_, msg.c_str(), caption.c_str(), MB_OK);
}

bool Win32DisplayManager::OnMessage(int msg, int wparam, long lparam) {
	switch(msg) {
		case WM_SIZING: {
			LPRECT rect = (LPRECT)(intptr_t)lparam;

			int _client_width  = GetClientWidth(rect->right - rect->left);
			int _client_height = GetClientHeight(rect->bottom - rect->top);

			DispatchResize(_client_width, _client_height);
			minimized_ = false;
			maximized_ = false;
		} break;
		case WM_SIZE: {
			switch(wparam) {
				case SIZE_MINIMIZED: {
					DispatchMinimize();
					minimized_ = true;
					maximized_ = false;
				} break;
				case SIZE_MAXIMIZED: {
					normal_width_  = display_mode_.width_;
					normal_height_ = display_mode_.height_;
					DispatchMaximize((int)LOWORD(lparam), (int)HIWORD(lparam));
					maximized_ = true;
					minimized_ = false;
				} break;
				case SIZE_RESTORED: {
					DispatchResize((int)LOWORD(lparam), (int)HIWORD(lparam));
					minimized_ = false;
					maximized_ = false;
				} break;
			}
		}
	}

	return (false);
}



int Win32DisplayManager::window_count_ = 0;
int Win32DisplayManager::register_count_ = 0;
HANDLE Win32DisplayManager::this_instance_ = 0;
WNDCLASSEX Win32DisplayManager::window_class_;
loginstance(kUiGfx, Win32DisplayManager);



}
