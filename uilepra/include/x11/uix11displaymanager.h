
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../../lepra/include/unordered.h"
#include "../uidisplaymanager.h"
#include "uix11core.h"
#include <X11/Xutil.h>



namespace uilepra {



class X11Observer {
public:
	// Returns true if the message was processed. Returns false otherwise.
	// If there are many observers listening to the same message,
	// returning true will stop propagation.
	virtual bool OnMessage(const XEvent& e) = 0;
};

class X11DisplayManager: public DisplayManager, public X11Observer {
public:
	friend class DisplayManager;

	X11DisplayManager();
	virtual ~X11DisplayManager();

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

	Display* GetDisplay() const;
	Window GetWindow() const;

	void ProcessMessages();
	void AddObserver(unsigned message, X11Observer* observer);
	void RemoveObserver(unsigned message, X11Observer* observer);
	void RemoveObserver(X11Observer* observer);
	// Show a popup dialog with a message.
	void ShowMessageBox(const str& msg, const str& caption);

	virtual bool OnMessage(const XEvent& e);
	virtual bool DispatchMessage(const XEvent& e);

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

	static Bool WaitForNotify(Display* d, XEvent* e, char* arg);
	XVisualInfo* GetVisualInfo() const;

	//Screen Stuff

	static int display_use_count_;
	Display* display_;
	Window wnd_;
	bool is_screen_open_;
	bool is_hidden_;

	bool minimized_;
	bool maximized_;

	int normal_width_;
	int normal_height_;

private:
	typedef std::unordered_set<X11Observer*, LEPRA_VOIDP_HASHER> ObserverSet;
	typedef HashTable<int, ObserverSet*> ObserverSetTable;
	// A table of lists of observers.
	ObserverSetTable observer_set_table_;

	int window_x_;
	int window_y_;
	bool caption_set_;

	logclass();
};



}
