
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../include/mac/uimacdisplaymanager.h"
#include <stdexcept>
#include "../../../lepra/include/posix/maclog.h"
#include "../../../lepra/include/log.h"
#include "../../../lepra/include/string.h"
#include "../../../lepra/include/systemmanager.h"
#include "../../include/uilepra.h"
#include "../../include/mac/uiiosdef.h"
#include "../../include/mac/uimacopengldisplay.h"



#ifdef LEPRA_IOS
@interface NativeWindow: LEPRA_APPLE_WINDOW
#else // !iOS
@interface NativeWindow: NSWindow <NSWindowDelegate>
#endif // iOS/!iOS
{
	@public uilepra::MacDisplayManager* display_manager_;
}
@end

@implementation NativeWindow
-(void) keyDown:(LEPRA_APPLE_EVENT*)theEvent {
	if (display_manager_) display_manager_->DispatchEvent(theEvent);
}
-(void) keyUp:(LEPRA_APPLE_EVENT*)theEvent {
	if (display_manager_) display_manager_->DispatchEvent(theEvent);
}
-(void) flagsChanged:(LEPRA_APPLE_EVENT*)theEvent {
	if (display_manager_) display_manager_->DispatchEvent(theEvent);
}
- (void) mouseDragged: (LEPRA_APPLE_EVENT*)theEvent {
	if (display_manager_) display_manager_->DispatchEvent(theEvent);
}
- (void)mouseMoved: (LEPRA_APPLE_EVENT*)theEvent {
	if (display_manager_) display_manager_->DispatchEvent(theEvent);
}
#ifndef LEPRA_IOS
-(void) windowDidResize:(NSNotification*)notification {
	NSView* view = [self contentView];
	if (display_manager_) display_manager_->DispatchResize(view.frame.size.width, view.frame.size.height);
}
#endif // !iOS
- (BOOL)acceptsFirstResponder {
	return YES;
}
- (BOOL)becomeFirstResponder {
	return YES;
}
- (BOOL)resignFirstResponder {
	return YES;
}
@end



namespace uilepra {



DisplayManager* DisplayManager::CreateDisplayManager(ContextType ct) {
	DisplayManager* display_manager = 0;
	switch(ct) {
		case DisplayManager::kOpenglContext:	display_manager = new MacOpenGLDisplay;				break;
		default:				log_.Error("Invalid context type in CreateDisplayManager().");	break;
	}
	return (display_manager);
}

void DisplayManager::EnableScreensaver(bool enable) {
	(void)enable;
	// TODO: implement!
}



MacDisplayManager::MacDisplayManager():
	wnd_(0),
	is_open_(false),
	minimized_(false),
	maximized_(false),
	normal_width_(0),
	normal_height_(0),
	caption_set_(false) {
#ifdef LEPRA_IOS
	screen_mode_ = kFullscreen;
	enumerated_display_mode_count_ = 10;
	enumerated_display_mode_ = new DisplayMode[enumerated_display_mode_count_];
	DisplayMode _display_mode;
	_display_mode.width_ = 320;
	_display_mode.height_ = 480;
	_display_mode.refresh_rate_ = 0;
	_display_mode.bit_depth_ = 0;
	enumerated_display_mode_[0] = _display_mode;
	std::swap(_display_mode.width_, _display_mode.height_);
	enumerated_display_mode_[1] = _display_mode;
	_display_mode.width_ = 640;
	_display_mode.height_ = 960;
	enumerated_display_mode_[2] = _display_mode;
	std::swap(_display_mode.width_, _display_mode.height_);
	enumerated_display_mode_[3] = _display_mode;
	_display_mode.width_ = 640;
	_display_mode.height_ = 1136;
	enumerated_display_mode_[4] = _display_mode;
	std::swap(_display_mode.width_, _display_mode.height_);
	enumerated_display_mode_[5] = _display_mode;
	_display_mode.width_ = 1024;
	_display_mode.height_ = 768;
	enumerated_display_mode_[6] = _display_mode;
	std::swap(_display_mode.width_, _display_mode.height_);
	enumerated_display_mode_[7] = _display_mode;
	_display_mode.width_ = 2048;
	_display_mode.height_ = 1536;
	enumerated_display_mode_[8] = _display_mode;
	std::swap(_display_mode.width_, _display_mode.height_);
	enumerated_display_mode_[9] = _display_mode;
#else // !iOS
	// Obtain number of available displays.
	CGDisplayCount display_count = 0;
	CGDisplayErr error = CGGetActiveDisplayList(0, 0, &display_count);
	deb_assert(error == CGDisplayNoErr);
	deb_assert(display_count >= 1);
	if (error != CGDisplayNoErr || display_count < 1) {
		throw std::runtime_error("CoreGraphics error: fetch of number of screens failed.");
	}

	// Allocate and convert.
	CGDirectDisplayID* displays = (CGDirectDisplayID*)new char[sizeof(CGDirectDisplayID) * display_count];
	error = CGGetActiveDisplayList(display_count, displays, &display_count);
	deb_assert(error == CGDisplayNoErr);
	deb_assert(display_count >= 1);
	if (error != CGDisplayNoErr || display_count < 1) {
		throw std::runtime_error("CoreGraphics error: fetch of number of screens failed 2.");
	}
	if(display_count >= 1) {
		//CGSize lScreenResolution = CGDisplayBounds(displays[0]).size;
		CFArrayRef mode_list = CGDisplayCopyAllDisplayModes(displays[0], NULL);
		if (!mode_list) {
			throw std::runtime_error("CoreGraphics error: fetch of display modes failed.");
		}
		CFIndex mode_count = CFArrayGetCount(mode_list);
		enumerated_display_mode_count_ = mode_count;
		enumerated_display_mode_ = new DisplayMode[enumerated_display_mode_count_];
		for (CFIndex x = 0; x < mode_count; ++x) {
			CGDisplayModeRef _mode = (CGDisplayModeRef)CFArrayGetValueAtIndex(mode_list, x);
			enumerated_display_mode_[x] = ConvertNativeDisplayMode(_mode);
		}
	}

	delete[] (displays);
#endif // iOS/!iOS
}

MacDisplayManager::~MacDisplayManager() {
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

unsigned MacDisplayManager::GetWidth() const {
	return display_mode_.width_;
}

unsigned MacDisplayManager::GetHeight() const {
	return display_mode_.height_;
}

unsigned MacDisplayManager::GetBitDepth() const {
	return display_mode_.bit_depth_;
}

unsigned MacDisplayManager::GetRefreshRate() const {
	return display_mode_.refresh_rate_;
}

bool MacDisplayManager::IsFullScreen() const {
	return (screen_mode_ == DisplayManager::kFullscreen);
}

double MacDisplayManager::GetPhysicalScreenSize() const {
#ifndef LEPRA_IOS
	return 24.0;	// Who knows how big an average Mac user's screen is when this game runs.
#else // iOS
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone) {
		CGSize size = [[UIScreen mainScreen] bounds].size;
		const float __height = std::max(size.width, size.height);
		if (__height < 500) {
			return SCREEN_SIZE_IPHONE_4;	// Or 4th Gen iPod touch or earlier.
		} else if (__height < 600) {
			return SCREEN_SIZE_IPHONE_5;
		} else if (__height < 700) {
			return SCREEN_SIZE_IPHONE_6;
		} else {
			return SCREEN_SIZE_IPHONE_6P;
		}
	} else {
		const str hw_name = SystemManager::GetHwName();
		const bool is_mini = (hw_name == "iPad2,5" ||
			hw_name == "iPad2,6" || hw_name == "iPad2,7");
		if (!is_mini) {
			return SCREEN_SIZE_IPAD_CLASSIC;
		} else {
			return SCREEN_SIZE_IPAD_MINI;
		}

	}
#endif // !iOS / iOS
}

void MacDisplayManager::SetCaption(const str& caption) {
	SetCaption(caption, false);
}

void MacDisplayManager::SetCaption(const str& caption, bool internal_call) {
#ifndef LEPRA_IOS
	if (is_open_) {
		if (internal_call == false) {
			caption_set_ = true;
		}

		if (internal_call == false || caption_set_ == false) {
			[wnd_ setTitle:MacLog::Encode(caption)];
		}
	}
#endif // !iOS
}

bool MacDisplayManager::OpenScreen(const DisplayMode& display_mode, ScreenMode screen_mode, Orientation orientation) {
	bool ok = true;

	if(is_open_ == true) {
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
				str err(strutil::Format("OpenScreen( - Display mode %i-bit %ix%i at %i Hz is not supported!"),
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
/*#ifdef LEPRA_IOS
		int w = display_mode_.width_;
		display_mode_.width_ = display_mode_.height_;
		display_mode_.height_ = w;
#endif // iOS*/
		ok = InitWindow();
	}

	if (ok) {
		ok = is_open_ = InitScreen();
	}

	return ok;
}

void MacDisplayManager::CloseScreen() {
	if (is_open_) {
		is_open_ = false;

		if (wnd_) {
			((NativeWindow*)wnd_)->display_manager_ = nil;
#ifndef LEPRA_IOS
			[wnd_ release];
#endif // iOS
			wnd_ = nil;
		}
	}
}

bool MacDisplayManager::IsVisible() const {
	return (!IsMinimized());
}

bool MacDisplayManager::IsFocused() const {
	return (!IsMinimized());
}

void MacDisplayManager::HideWindow(bool hide) {
#ifdef LEPRA_IOS
#else
	if (hide) {
		[wnd_ orderOut:wnd_];
	} else {
		[wnd_ makeKeyAndOrderFront:wnd_];
		[[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
	}
#endif // iOS / Mac
}

bool MacDisplayManager::InitWindow() {
	CloseScreen();

	bool ok = is_open_ = true;

	NativeWindow* wnd = [NativeWindow alloc];
	wnd_ = wnd;
	wnd->display_manager_ = this;
#ifdef LEPRA_IOS
	CGRect r = [UIScreen mainScreen].bounds;
	int s = [[UIScreen mainScreen] scale];
	r.size = CGSizeMake(r.size.width*s, r.size.height*s);
	[wnd_ initWithFrame:r];
#else // !iOS
	[wnd_ initWithContentRect:NSMakeRect(0, 0, display_mode_.width_, display_mode_.height_)
			styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask
			  backing:NSBackingStoreBuffered
			    defer:NO];
	wnd.delegate = wnd;
	[wnd_ setAcceptsMouseMovedEvents:YES];
	[wnd_ setIgnoresMouseEvents:NO];
        [wnd_ setReleasedWhenClosed:YES];
	[wnd_ makeKeyAndOrderFront:nil];
	[wnd_ setFrame:NSMakeRect(0, 0, display_mode_.width_, display_mode_.height_)
	       display:YES];
	[wnd_ center];
#endif // iOS/!iOS

	++window_count_;

	/*if (ok) {
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
								"LepraMacClass", "Lepra",
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

					DWORD style = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;

					if (screen_mode_ == DisplayManager::kWindowed) {
						style |= (WS_SIZEBOX | WS_MAXIMIZEBOX);
					}

					wnd_ = ::CreateWindowEx(0, "LepraMacClass", "Lepra",
						style,
						GetSystemMetrics(SM_CXSCREEN) / 2 - _window_height / 2,
						GetSystemMetrics(SM_CYSCREEN) / 2 - _window_height / 2,
						_window_width, _window_height,
						GetDesktopWindow(), NULL, (HINSTANCE)this_instance_, NULL);
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
			++window_count__;
		} else {
			log_.Error("InitWindow() - Failed to create window.");
		}
	}*/

	if (ok) {
		MacCore::AddDisplayManager(this);
	}

	return ok;
}

void MacDisplayManager::GetBorderSize(int& size_x, int& size_y) {
	if (screen_mode_ == kFullscreen || screen_mode_ == kSplashWindow) {
		size_x = 0;
		size_y = 0;
	} else if (wnd_) {
		size_x = 0;
		size_y = 0;
	} else {
		deb_assert(false);
		size_x = 0;
		size_y = 0;
	}
}

int MacDisplayManager::GetWindowWidth(int client_width) {
	int border_size_x;
	int border_size_y;
	GetBorderSize(border_size_x, border_size_y);
	return client_width + border_size_x;
}

int MacDisplayManager::GetWindowHeight(int client_height) {
	int border_size_x;
	int border_size_y;
	GetBorderSize(border_size_x, border_size_y);
	return client_height + border_size_y;
}

int MacDisplayManager::GetClientWidth(int window_width) {
	int border_size_x;
	int border_size_y;
	GetBorderSize(border_size_x, border_size_y);
	return window_width - border_size_x;
}

int MacDisplayManager::GetClientHeight(int window_height) {
	int border_size_x;
	int border_size_y;
	GetBorderSize(border_size_x, border_size_y);
	return window_height - border_size_y;
}

LEPRA_APPLE_WINDOW* MacDisplayManager::GetWindow() const {
	return wnd_;
}

void MacDisplayManager::DispatchEvent(LEPRA_APPLE_EVENT* e) {
	ObserverSetTable::Iterator t_iter = observer_set_table_.Find([e type]);
	if (t_iter != observer_set_table_.End()) {
		ObserverSet* set = *t_iter;
		ObserverSet::iterator l_iter;
		for (l_iter = set->begin(); l_iter != set->end(); ++l_iter) {
			(*l_iter)->OnEvent(e);
		}
	}
}

void MacDisplayManager::ProcessMessages() {
}

void MacDisplayManager::AddObserver(int _message, MacObserver* observer) {
	ObserverSetTable::Iterator t_iter = observer_set_table_.Find(_message);
	ObserverSet* set = 0;

	if (t_iter == observer_set_table_.End()) {
		set = new ObserverSet;
		observer_set_table_.Insert(_message, set);
	} else {
		set = *t_iter;
	}

	ObserverSet::iterator l_iter = set->find(observer);
	if (l_iter == set->end()) {
		set->insert(observer);
	}
}

void MacDisplayManager::RemoveObserver(int _message, MacObserver* observer) {
	ObserverSetTable::Iterator t_iter = observer_set_table_.Find(_message);

	if (t_iter != observer_set_table_.End()) {
		ObserverSet* set = *t_iter;
		set->erase(observer);
		if (set->empty()) {
			observer_set_table_.Remove(t_iter);
			delete (set);
		}
	}
}

void MacDisplayManager::RemoveObserver(MacObserver* observer) {
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

void MacDisplayManager::ShowMessageBox(const str& msg, const str& caption) {
#ifndef LEPRA_IOS
	NSRunAlertPanel(MacLog::Encode(caption), MacLog::Encode(msg), nil, nil, nil);
#else // iOS
	UIAlertView* message = [[UIAlertView alloc]
		initWithTitle:MacLog::Encode(caption)
		      message:MacLog::Encode(msg)
		     delegate:nil
	    cancelButtonTitle:@"OK"
	    otherButtonTitles:nil];
	[message show];
	//[message release];
#endif // !iOS / iOS
}


#ifndef LEPRA_IOS
DisplayMode MacDisplayManager::ConvertNativeDisplayMode(CGDisplayModeRef mode) {
	DisplayMode _display_mode;
	_display_mode.width_ = CGDisplayModeGetWidth(mode);
	_display_mode.height_ = CGDisplayModeGetHeight(mode);
	_display_mode.refresh_rate_ = (int)CGDisplayModeGetRefreshRate(mode);
	_display_mode.bit_depth_ = 0;
	CFStringRef pix_enc = CGDisplayModeCopyPixelEncoding(mode);
	if (CFStringCompare(pix_enc, CFSTR(IO32BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
		_display_mode.bit_depth_ = 32;
	} else if(CFStringCompare(pix_enc, CFSTR(IO16BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
		_display_mode.bit_depth_ = 16;
	} else if(CFStringCompare(pix_enc, CFSTR(IO8BitIndexedPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
		_display_mode.bit_depth_ = 8;
	}
	return (_display_mode);
}
#endif // !iOS



int MacDisplayManager::window_count_ = 0;
loginstance(kUiGfx, MacDisplayManager);



}
