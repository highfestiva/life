
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../../lepra/include/unordered.h"
#include "../uidisplaymanager.h"
#include "uiwin32core.h"



namespace uilepra {



class Win32Observer {
public:
	// Returns true if the message was processed. Returns false otherwise,
	// in which case the DefWindowProc() will be called.
	// If there are many observers listening to the same message,
	// the one returning true "has the right of veto".
	virtual bool OnMessage(int msg, int wparam, long lparam) = 0;
};

class Win32DisplayManager: public DisplayManager, public Win32Observer {
public:
	friend class DisplayManager;

	Win32DisplayManager();
	virtual ~Win32DisplayManager();

	// Sets the caption of the window.
	virtual void SetCaption(const str& caption);

	// The border flag only has effect in windowed mode.
	virtual bool OpenScreen(const DisplayMode& display_mode, ScreenMode mode, Orientation orientation);
	virtual void CloseScreen();

	virtual bool IsVisible() const;
	virtual bool IsFocused() const;
	virtual void HideWindow(bool hide);

	virtual unsigned GetWidth() const;
	virtual unsigned GetHeight() const;
	virtual unsigned GetBitDepth() const;
	virtual unsigned GetRefreshRate() const;
	virtual bool IsFullScreen() const;
	virtual double GetPhysicalScreenSize() const;

	HWND GetHWND();

	static LRESULT CALLBACK WndProc(HWND wnd, unsigned int message, unsigned int wparam, LONG lparam);
	bool InternalDispatchMessage(int message, int wparam, long lparam);
	void ProcessMessages();
	void AddObserver(unsigned int message, Win32Observer* observer);
	void RemoveObserver(unsigned int message, Win32Observer* observer);
	void RemoveObserver(Win32Observer* observer);
	// Show a popup dialog with a message.
	void ShowMessageBox(const str& msg, const str& caption);

	virtual bool OnMessage(int msg, int wparam, long lparam);

protected:

	// Normal resize (not maximize, nor minimized).
	virtual void OnResize(int width, int height) = 0;

	virtual void OnMinimize() = 0;
	virtual void OnMaximize(int width, int height) = 0;

	// Different implementations needed whether you want to use a software
	// renderer, OpenGL or DirectX.
	virtual bool InitScreen() = 0;
	virtual void SetFocus(bool focus);

	bool Register();
	void Unregister();
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

	//Screen Stuff

	static int window_count_;
	static int register_count_;
	static HANDLE this_instance_;
	static WNDCLASSEX window_class_;
	HWND wnd_;
	bool initialized_;
	bool screen_opened_;

	bool minimized_;
	bool maximized_;

	int normal_width_;
	int normal_height_;

private:
	bool caption_set_;

	bool consume_char_;

	typedef std::unordered_set<Win32Observer*, LEPRA_VOIDP_HASHER> ObserverSet;
	typedef HashTable<unsigned int, ObserverSet*> ObserverSetTable;
	// A table of lists of observers.
	ObserverSetTable observer_set_table_;

	logclass();
};



}
