
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <hash_set>
#include "../UiDisplayManager.h"
#include "UiX11Core.h"



namespace UiLepra
{



class X11Observer
{
public:
	// Returns true if the message was processed. Returns false otherwise.
	// If there are many observers listening to the same message,
	// returning true will stop propagation.
	virtual Bool OnMessage(XEvent* e) = 0;
};

class X11DisplayManager: public DisplayManager, public X11Observer
{
public:
	friend class DisplayManager;

	X11DisplayManager();
	virtual ~X11DisplayManager();

	// Sets the caption of the window.
	virtual void SetCaption(const str& pCaption);

	// The border flag only has effect in windowed mode.
	virtual bool OpenScreen(const DisplayMode& pDisplayMode, ScreenMode pMode);
	virtual void CloseScreen();

	virtual bool IsVisible() const;

	/*
		Display mode enumeration (fullscreen modes).
		Narrow the search by specifying width and height and/or bitrate.
	*/

	// Returns the number of supported display modes on this computer.
	virtual int GetNumDisplayModes();
	virtual int GetNumDisplayModes(int pBitDepth);
	virtual int GetNumDisplayModes(int pWidth, int pHeight);
	virtual int GetNumDisplayModes(int pWidth, int pHeight, int pBitDepth);

	// Fills pDisplayMode with the mode number pMode.
	// pMode must be a number greater or equal to 0, and less than the number returned from
	// the corresponding GetNumDisplayModes().
	virtual bool GetDisplayMode(DisplayMode& pDisplayMode, int pMode);
	virtual bool GetDisplayMode(DisplayMode& pDisplayMode, int pMode, int pBitDepth);
	virtual bool GetDisplayMode(DisplayMode& pDisplayMode, int pMode, int pWidth, int pHeight);
	virtual bool GetDisplayMode(DisplayMode& pDisplayMode, int pMode, int pWidth, int pHeight, int pBitDepth);

	// Display mode search. Returns true if a display mode was found, and fills the DisplayMode struct.
	virtual bool FindDisplayMode(DisplayMode& pDisplayMode, int pWidth, int pHeight);
	virtual bool FindDisplayMode(DisplayMode& pDisplayMode, int pWidth, int pHeight, int pBitDepth);
	virtual bool FindDisplayMode(DisplayMode& pDisplayMode, int pWidth, int pHeight, int pBitDepth, int pRefreshRate);

	// Returns the pointer if available, NULL otherwise.
	virtual void* GetScreenPtr();
	virtual unsigned GetWidth();
	virtual unsigned GetHeight();
	virtual unsigned GetBitDepth();
	virtual unsigned GetRefreshRate();
	virtual bool IsFullScreen();

	Display* GetDisplay() const;
	Window GetWindow() const;

	static Bool WaitForNotify(Display* d, XEvent* e, char* arg);
	bool InternalDispatchMessage(XEvent* e);
	void ProcessMessages();
	void AddObserver(int pMessage, X11Observer* pObserver);
	void RemoveObserver(int pMessage, X11Observer* pObserver);
	void RemoveObserver(X11Observer* pObserver);
	// Show a popup dialog with a message.
	void ShowMessageBox(const str& pMsg, const str& pCaption);

	virtual bool OnMessage(XEvent* e);

protected:

	// Normal resize (not maximize, nor minimized).
	virtual void OnResize(int pWidth, int pHeight) = 0;

	virtual void OnMinimize() = 0;
	virtual void OnMaximize(int pWidth, int pHeight) = 0;

	// Different implementations needed depending on renderer type.
	virtual bool InitScreen() = 0;
	virtual void SetFocus(bool pFocus) = 0;

	bool InitWindow();

	void SetCaption(const str& pCaption, bool pInternalCall);

	void GetBorderSize(int& pSizeX, int& pSizeY);
	int GetWindowWidth(int pClientWidth);
	int GetWindowHeight(int pClientHeight);
	int GetClientWidth(int pWindowWidth);
	int GetClientHeight(int pWindowHeight);

	inline bool IsMinimized() const
	{
		return mMinimized;
	}

	inline bool IsMaximized() const
	{
		return mMaximized;
	}

	//Screen Stuff

	DisplayMode mDisplayMode;
	ScreenMode mScreenMode;	// Fullscreen or windowed.

	static int msWindowCount;
	Display* mDisplay;
	Window mWnd;
	bool mIsOpen;

	bool mMinimized;
	bool mMaximized;

	int mNormalWidth;
	int mNormalHeight;

private:
	DisplayMode* mEnumeratedDisplayMode;
	int mEnumeratedDisplayModeCount;

	bool mCaptionSet;

	bool mConsumeChar;

	typedef std::hash_set<X11Observer*, std::hash<void*> > ObserverSet;
	typedef HashTable<int, ObserverSet*> ObserverSetTable;
	// A table of lists of observers.
	ObserverSetTable mObserverSetTable;

	LOG_CLASS_DECLARE();
};



}
