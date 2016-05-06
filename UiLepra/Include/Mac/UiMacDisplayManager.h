
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../../lepra/include/unordered.h"
#include "../uidisplaymanager.h"
#include "uimaccore.h"



namespace uilepra {



class MacObserver {
public:
	virtual void OnEvent(LEPRA_APPLE_EVENT* e) = 0;
};

class MacDisplayManager: public DisplayManager {
public:
	friend class DisplayManager;

	MacDisplayManager();
	virtual ~MacDisplayManager();

	// Sets the caption of the window.
	virtual void SetCaption(const str& caption);

	// The border flag only has effect in windowed mode.
	virtual bool OpenScreen(const DisplayMode& display_mode, ScreenMode mode, Orientation orientation);
	virtual void CloseScreen();

	virtual bool IsVisible() const;
	virtual bool IsFocused() const;
	virtual void HideWindow(bool hide);

	// Returns the pointer if available, NULL otherwise.
	virtual unsigned GetWidth() const;
	virtual unsigned GetHeight() const;
	virtual unsigned GetBitDepth() const;
	virtual unsigned GetRefreshRate() const;
	virtual bool IsFullScreen() const;
	virtual double GetPhysicalScreenSize() const;

	LEPRA_APPLE_WINDOW* GetWindow() const;

	void DispatchEvent(LEPRA_APPLE_EVENT* e);

	void ProcessMessages();
	void AddObserver(int message, MacObserver* observer);
	void RemoveObserver(int message, MacObserver* observer);
	void RemoveObserver(MacObserver* observer);
	// Show a popup dialog with a message.
	void ShowMessageBox(const str& msg, const str& caption);

protected:
	// Normal resize (not maximize, nor minimized).
	virtual void OnResize(int width, int height) = 0;

	virtual void OnMinimize() = 0;
	virtual void OnMaximize(int width, int height) = 0;

	// Different implementations needed depending on renderer type.
	virtual bool InitScreen() = 0;
	virtual void SetFocus(bool focus) = 0;

	bool InitWindow();

	void SetCaption(const str& caption, bool internal_call);

	void GetBorderSize(int& size_x, int& size_y);
	int GetWindowWidth(int client_width);
	int GetWindowHeight(int client_height);
	int GetClientWidth(int window_width);
	int GetClientHeight(int window_height);

	inline bool IsMinimized() const {
		return minimized_;
	}

	inline bool IsMaximized() const {
		return maximized_;
	}

#ifndef LEPRA_IOS
	static DisplayMode ConvertNativeDisplayMode(CGDisplayModeRef mode);
#endif // !iOS

	//Screen Stuff

	static int window_count_;

	LEPRA_APPLE_WINDOW* wnd_;
	bool is_open_;

	bool minimized_;
	bool maximized_;

	int normal_width_;
	int normal_height_;

private:
	bool caption_set_;

	bool consume_char_;

	typedef std::unordered_set<MacObserver*, LEPRA_VOIDP_HASHER> ObserverSet;
	typedef HashTable<int, ObserverSet*> ObserverSetTable;
	// A table of lists of observers.
	ObserverSetTable observer_set_table_;

	logclass();
};



}
