
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../../Lepra/Include/Unordered.h"
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
	virtual bool OpenScreen(const DisplayMode& pDisplayMode, ScreenMode pMode, Orientation pOrientation);
	virtual void CloseScreen();

	virtual bool IsVisible() const;

	virtual unsigned GetWidth() const;
	virtual unsigned GetHeight() const;
	virtual unsigned GetBitDepth() const;
	virtual unsigned GetRefreshRate() const;
	virtual bool IsFullScreen() const;
	virtual double GetPhysicalScreenSize() const;

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
	virtual void SetFocus(bool pFocus);

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

	typedef std::unordered_set<Win32Observer*, LEPRA_VOIDP_HASHER> ObserverSet;
	typedef HashTable<unsigned int, ObserverSet*> ObserverSetTable;
	// A table of lists of observers.
	ObserverSetTable mObserverSetTable;

	logclass();
};



}
