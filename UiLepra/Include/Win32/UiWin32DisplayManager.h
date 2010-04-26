
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <hash_set>
#include "../UiDisplayManager.h"
#include "UiWin32Core.h"



namespace UiLepra
{



class Win32Observer
{
public:
	// Returns true if the message was processed. Returns false otherwise,
	// in which case the DefWindowProc() will be called.
	// If there are many observers listening to the same message,
	// the one returning true "has the right of veto".
	virtual bool OnMessage(int pMsg, int pwParam, long plParam) = 0;
};

class Win32DisplayManager: public DisplayManager, public Win32Observer
{
public:
	friend class DisplayManager;

	Win32DisplayManager();
	virtual ~Win32DisplayManager();

	// Sets the caption of the window.
	virtual void SetCaption(const str& pCaption);

	// The border flag only has effect in windowed mode.
	virtual bool OpenScreen(const DisplayMode& pDisplayMode, ScreenMode pMode);
	virtual void CloseScreen();

	virtual bool IsVisible() const;

	virtual unsigned GetWidth();
	virtual unsigned GetHeight();
	virtual unsigned GetBitDepth();
	virtual unsigned GetRefreshRate();
	virtual bool IsFullScreen();

	HWND GetHWND();

	static LRESULT CALLBACK WndProc(HWND pWnd, unsigned int pMessage, unsigned int pwParam, LONG plParam);
	bool InternalDispatchMessage(int pMessage, int pwParam, long plParam);
	void ProcessMessages();
	void AddObserver(unsigned int pMessage, Win32Observer* pObserver);
	void RemoveObserver(unsigned int pMessage, Win32Observer* pObserver);
	void RemoveObserver(Win32Observer* pObserver);
	// Show a popup dialog with a message.
	void ShowMessageBox(const str& pMsg, const str& pCaption);

	virtual bool OnMessage(int pMsg, int pwParam, long plParam);

protected:

	// Normal resize (not maximize, nor minimized).
	virtual void OnResize(int pWidth, int pHeight) = 0;

	virtual void OnMinimize() = 0;
	virtual void OnMaximize(int pWidth, int pHeight) = 0;

	// Different implementations needed whether you want to use a software
	// renderer, OpenGL or DirectX.
	virtual bool InitScreen() = 0;
	virtual void SetFocus(bool pFocus) = 0;

	bool Register();
	void Unregister();
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
	static int msRegisterCount;
	static HANDLE mshThisInstance;
	static WNDCLASSEX msWindowClass;
	HWND mWnd;
	bool mInitialized;
	bool mScreenOpened;

	bool mMinimized;
	bool mMaximized;

	int mNormalWidth;
	int mNormalHeight;

private:
	bool mCaptionSet;

	bool mConsumeChar;

	typedef std::hash_set<Win32Observer*, LEPRA_VOIDP_HASHER> ObserverSet;
	typedef HashTable<unsigned int, ObserverSet*> ObserverSetTable;
	// A table of lists of observers.
	ObserverSetTable mObserverSetTable;

	LOG_CLASS_DECLARE();
};



}
