
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/unordered.h"
#include "../../lepra/include/canvas.h"
#include "../../lepra/include/log.h"
#include "../include/uilepra.h"



namespace uilepra {



class DisplayMode {
public:

	// Set to default values on creation.
	inline DisplayMode() :
		width_(640),
		height_(480),
		bit_depth_(0),
		refresh_rate_(0) {
	}

	inline DisplayMode(int width, int height) :
		width_(width),
		height_(height),
		bit_depth_(0),
		refresh_rate_(0) {
	}

	inline DisplayMode(int width, int height, int bit_depth) :
		width_(width),
		height_(height),
		bit_depth_(bit_depth),
		refresh_rate_(0) {
	}

	inline bool IsValid() const {
		if (width_ <= 0 || height_ <= 0 || refresh_rate_ <= 0 ||
			(
				bit_depth_ != 0 &&
				bit_depth_ != 8 &&
				bit_depth_ != 15 &&
				bit_depth_ != 16 &&
				bit_depth_ != 24 &&
				bit_depth_ != 32
			)
		  ) {
			return false;
		}

		return true;
	}

	int width_;
	int height_;
	int bit_depth_;      // Set to 0 means "don't care". Will be set to highest possible value.
	int refresh_rate_;   // Set to 0 means "don't care", the system will
                            // use the highest supported frequency.
};

class DisplayResizeObserver {
public:
	virtual void OnResize(int width, int height) = 0;
	virtual void OnMinimize() = 0;
	virtual void OnMaximize(int width, int height) = 0;
};


class DisplayManager: public DisplayResizeObserver {
public:

	enum ScreenMode {
		kWindowed,	// Windowed and resizable.
		kStaticWindow,	// Windowed and not resizable.
		kSplashWindow,	// No border.
		kFullscreen,
	};

	enum Orientation {
		kOrientationAllowAny = 0,
		kOrientationAllowUpsideDown,
		kOrientationAllowAnyInternal,
		kOrientationAllowUpsideDownInternal,
		kOrientationFixed,
	};

	enum ContextType {
		kOpenglContext,
		kDirectxContext,
	};

	// Creates a platform dependent display manager. This function
	// is implemented elsewhere, (not in DisplayManager.cpp). The
	// exact location of this function depends on the platform
	// specific implementation. There can be only one display manager.
	// If this function is called twice with different contexts,
	// the first display manager will be deleted.
	static DisplayManager* CreateDisplayManager(ContextType ct);

	DisplayManager();
	virtual ~DisplayManager();

	static void EnableScreensaver(bool enable);

	virtual ContextType GetContextType() = 0;

	// Sets the caption of the window.
	virtual void SetCaption(const str& caption) = 0;

	// Creates a window in windowed or fullscreen mode on the main display device.
	virtual bool OpenScreen(const DisplayMode& display_mode, ScreenMode mode, Orientation orientation) = 0;
	virtual void CloseScreen() = 0;

	virtual void SetOrientation(Orientation orientation);
	void AddResizeObserver(DisplayResizeObserver* resize_observer);
	void RemoveResizeObserver(DisplayResizeObserver* resize_observer);
	void DispatchResize(int width, int height);
	void DispatchMinimize();
	void DispatchMaximize(int width, int height);

	virtual void SetFocus(bool focus) = 0;
	virtual bool Activate() = 0;
	virtual bool Deactivate() = 0;
	// Swaps buffers etc.
	virtual bool UpdateScreen() = 0;

	virtual bool IsVSyncEnabled() const = 0;
	virtual bool SetVSyncEnabled(bool enabled) = 0;
	virtual bool IsVisible() const = 0;
	virtual bool IsFocused() const = 0;
	virtual void HideWindow(bool hide) = 0;

	/*
		Display mode enumeration (fullscreen modes).
		Narrow the search by specifying width and height and/or bitrate.
	*/

	// Returns the number of supported display modes on this computer.
	virtual int GetNumDisplayModes();
	virtual int GetNumDisplayModes(int bit_depth);
	virtual int GetNumDisplayModes(int width, int height);
	virtual int GetNumDisplayModes(int width, int height, int bit_depth);

	// Fills display_mode with the mode number mode.
	// mode must be a number greater or equal to 0, and less than the number returned from
	// the corresponding GetNumDisplayModes().
	virtual bool GetDisplayMode(DisplayMode& display_mode, int mode);
	virtual bool GetDisplayMode(DisplayMode& display_mode, int mode, int bit_depth);
	virtual bool GetDisplayMode(DisplayMode& display_mode, int mode, int width, int height);
	virtual bool GetDisplayMode(DisplayMode& display_mode, int mode, int width, int height, int bit_depth);

	// Display mode search. Returns true if a matching display mode was found,
	// and fills the DisplayMode struct.
	virtual bool FindDisplayMode(DisplayMode& display_mode, int width, int height);
	virtual bool FindDisplayMode(DisplayMode& display_mode, int width, int height, int bit_depth);
	virtual bool FindDisplayMode(DisplayMode& display_mode, int width, int height, int bit_depth, int refresh_rate);

	virtual unsigned GetWidth() const = 0;
	virtual unsigned GetHeight() const = 0;
	virtual unsigned GetBitDepth() const = 0;
	virtual unsigned GetRefreshRate() const = 0;
	virtual bool IsFullScreen() const = 0;
	virtual double GetPhysicalScreenSize() const = 0;

	// Returns the canvas describing the screen. In accelerated modes
	// (OpenGL, DirectX etc) you can't assume that there is a valid pointer
	// to the display memory. If there is a valid pointer, it's only valid
	// for the current frame.
	void GetScreenCanvas(Canvas& canvas);
	void GetScreenCanvas(Canvas* canvas);

	// Functions implemented for conveniency.
	static lepra::uint8 GetPaletteColor(int red, int green, int blue, const Color* palette);

	virtual void ShowMessageBox(const str& msg, const str& caption) = 0;

protected:
	typedef std::unordered_set<DisplayResizeObserver*, LEPRA_VOIDP_HASHER> ResizeObserverSet;
	ResizeObserverSet resize_observer_set_;

	DisplayMode display_mode_;
	ScreenMode screen_mode_;	// Fullscreen or windowed.
	Orientation orientation_;

	DisplayMode* enumerated_display_mode_;
	int enumerated_display_mode_count_;

private:
	logclass();
};



}
