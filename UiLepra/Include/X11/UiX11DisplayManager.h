
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../../Lepra/Include/Unordered.h"
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
	virtual bool OnMessage(XEvent& e) = 0;
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
	virtual bool OpenScreen(const DisplayMode& pDisplayMode, ScreenMode pMode, Orientation pOrientation);
	virtual void CloseScreen();

	virtual bool IsVisible() const;
	virtual bool IsFocused() const;
	virtual void HideWindow(bool pHide);

	// Returns the pointer if available, NULL otherwise.
	virtual unsigned GetWidth() const;
	virtual unsigned GetHeight() const;
	virtual unsigned GetBitDepth() const;
	virtual unsigned GetRefreshRate() const;
	virtual bool IsFullScreen() const;
	virtual double GetPhysicalScreenSize() const;

	Display* GetDisplay() const;
	Window GetWindow() const;

	static Bool WaitForNotify(Display* d, XEvent& e, char* arg);
	void ProcessMessages();
	void AddObserver(unsigned pMessage, X11Observer* pObserver);
	void RemoveObserver(unsigned pMessage, X11Observer* pObserver);
	void RemoveObserver(X11Observer* pObserver);
	// Show a popup dialog with a message.
	void ShowMessageBox(const str& pMsg, const str& pCaption);

	virtual bool OnMessage(XEvent& e);

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

	static Bool WaitForNotify(Display* d, XEvent* e, char* arg);

	//Screen Stuff

	static int msWindowCount;
	Display* mDisplay;
	Window mWnd;
	bool mIsScreenOpen;
	bool mIsHidden;

	bool mMinimized;
	bool mMaximized;

	int mNormalWidth;
	int mNormalHeight;

private:
	bool mCaptionSet;

	bool mConsumeChar;

	typedef std::unordered_set<X11Observer*, LEPRA_VOIDP_HASHER> ObserverSet;
	typedef HashTable<int, ObserverSet*> ObserverSetTable;
	// A table of lists of observers.
	ObserverSetTable mObserverSetTable;

	logclass();
};



}
