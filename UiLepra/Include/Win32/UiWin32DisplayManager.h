/*
	Class:  Win32DisplayManager
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#ifndef UIWIN32DISPLAYMANAGER_H
#define UIWIN32DISPLAYMANAGER_H

#include <hash_set>
#include "../UiDisplayManager.h"
#include "UiWin32Core.h"

#undef IsMinimized
#undef IsMaximized

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
	virtual void SetCaption(const Lepra::String& pCaption);

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

	// Returns true if the 16 bit mode (565) really is 15 bit (555).
	virtual bool Is15Bit();

	// Returns the color that is closest to the desired color (pRed, pGreen, pBlue).
	// OBS! This is slow! Do not use in time critical loops!
	virtual Lepra::uint8 GetPaletteColor(int pRed, int pGreen, int pBlue);
	virtual const Lepra::Color* GetPaletteColor(unsigned pIndex);

	virtual void SetPalette(const Lepra::Color* pPalette);

	/*
		Win32 implementation specific stuff...
	*/

	HWND GetHWND();

	static LRESULT CALLBACK WndProc(HWND pWnd, unsigned int pMessage, unsigned int pwParam, LONG plParam);
	bool InternalDispatchMessage(int pMessage, int pwParam, long plParam);
	void ProcessMessages();
	// Post message to self (to window used by this display manager).
	void PostMessage(int pMsg, int pwParam, long plParam);
	void AddObserver(unsigned int pMessage, Win32Observer* pObserver);
	void RemoveObserver(unsigned int pMessage, Win32Observer* pObserver);
	void RemoveObserver(Win32Observer* pObserver);
	// Show a popup dialog with a message.
	void ShowMessageBox(const Lepra::String& pMsg, const Lepra::String& pCaption);

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

	void SetCaption(const Lepra::String& pCaption, bool pInternalCall);

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

	void* mScreen;

	Lepra::Color mPalette[256];

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
	DisplayMode* mEnumeratedDisplayMode;
	int mEnumeratedDisplayModeCount;

	bool mCaptionSet;

	bool mConsumeChar;

	typedef std::hash_set<Win32Observer*, std::hash<void*> > ObserverSet;
	typedef Lepra::HashTable<unsigned int, ObserverSet*> ObserverSetTable;
	// A table of lists of observers.
	ObserverSetTable mObserverSetTable;

	LOG_CLASS_DECLARE();
};

} // End namespace.

#endif
