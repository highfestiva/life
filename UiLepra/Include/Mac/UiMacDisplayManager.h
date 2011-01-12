
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <hash_set>
#include "../UiDisplayManager.h"
#include "UiMacCore.h"



namespace UiLepra
{



class MacObserver
{
public:
	virtual void OnEvent(NSEvent* e) = 0;
};

class MacDisplayManager: public DisplayManager, public MacObserver
{
public:
	friend class DisplayManager;

	MacDisplayManager();
	virtual ~MacDisplayManager();

	// Sets the caption of the window.
	virtual void SetCaption(const str& pCaption);

	// The border flag only has effect in windowed mode.
	virtual bool OpenScreen(const DisplayMode& pDisplayMode, ScreenMode pMode);
	virtual void CloseScreen();

	virtual bool IsVisible() const;

	// Returns the pointer if available, NULL otherwise.
	virtual unsigned GetWidth();
	virtual unsigned GetHeight();
	virtual unsigned GetBitDepth();
	virtual unsigned GetRefreshRate();
	virtual bool IsFullScreen();

	NSWindow* GetWindow() const;

	void DispatchEvent(NSEvent* e);

	void ProcessMessages();
	void AddObserver(int pMessage, MacObserver* pObserver);
	void RemoveObserver(int pMessage, MacObserver* pObserver);
	void RemoveObserver(MacObserver* pObserver);
	// Show a popup dialog with a message.
	void ShowMessageBox(const str& pMsg, const str& pCaption);

	virtual void OnEvent(NSEvent* e);

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

	static DisplayMode ConvertNativeDisplayMode(CFDictionaryRef pMode);
	
	//Screen Stuff

	DisplayMode mDisplayMode;
	ScreenMode mScreenMode;	// Fullscreen or windowed.

	static int mWindowCount;

	NSWindow* mWnd;
	bool mIsOpen;

	bool mMinimized;
	bool mMaximized;

	int mNormalWidth;
	int mNormalHeight;

private:
	bool mCaptionSet;

	bool mConsumeChar;

	typedef std::hash_set<MacObserver*, LEPRA_VOIDP_HASHER> ObserverSet;
	typedef HashTable<int, ObserverSet*> ObserverSetTable;
	// A table of lists of observers.
	ObserverSetTable mObserverSetTable;

	LOG_CLASS_DECLARE();
};



}
