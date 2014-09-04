
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../../Lepra/Include/Unordered.h"
#include "../UiDisplayManager.h"
#include "UiMacCore.h"



namespace UiLepra
{



class MacObserver
{
public:
	virtual void OnEvent(LEPRA_APPLE_EVENT* e) = 0;
};

class MacDisplayManager: public DisplayManager
{
public:
	friend class DisplayManager;

	MacDisplayManager();
	virtual ~MacDisplayManager();

	// Sets the caption of the window.
	virtual void SetCaption(const str& pCaption);

	// The border flag only has effect in windowed mode.
	virtual bool OpenScreen(const DisplayMode& pDisplayMode, ScreenMode pMode, Orientation pOrientation);
	virtual void CloseScreen();

	virtual bool IsVisible() const;

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
	void AddObserver(int pMessage, MacObserver* pObserver);
	void RemoveObserver(int pMessage, MacObserver* pObserver);
	void RemoveObserver(MacObserver* pObserver);
	// Show a popup dialog with a message.
	void ShowMessageBox(const str& pMsg, const str& pCaption);

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

#ifndef LEPRA_IOS
	static DisplayMode ConvertNativeDisplayMode(CGDisplayModeRef pMode);
#endif // !iOS

	//Screen Stuff

	static int mWindowCount;

	LEPRA_APPLE_WINDOW* mWnd;
	bool mIsOpen;

	bool mMinimized;
	bool mMaximized;

	int mNormalWidth;
	int mNormalHeight;

private:
	bool mCaptionSet;

	bool mConsumeChar;

	typedef std::unordered_set<MacObserver*, LEPRA_VOIDP_HASHER> ObserverSet;
	typedef HashTable<int, ObserverSet*> ObserverSetTable;
	// A table of lists of observers.
	ObserverSetTable mObserverSetTable;

	logclass();
};



}
