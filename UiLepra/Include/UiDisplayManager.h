
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <set>
#include "../../Lepra/Include/Canvas.h"
#include "../../Lepra/Include/Log.h"



namespace UiLepra
{



class DisplayMode
{
public:

	// Set to default values on creation.
	inline DisplayMode() :
		mWidth(640),
		mHeight(480),
		mBitDepth(0),
		mRefreshRate(0)
	{
	}

	inline DisplayMode(int pWidth, int pHeight) :
		mWidth(pWidth),
		mHeight(pHeight),
		mBitDepth(0),
		mRefreshRate(0)
	{
	}

	inline DisplayMode(int pWidth, int pHeight, int pBitDepth) :
		mWidth(pWidth),
		mHeight(pHeight),
		mBitDepth(pBitDepth),
		mRefreshRate(0)
	{
	}

	inline bool IsValid() const
	{
		if (mWidth <= 0 || mHeight <= 0 || mRefreshRate <= 0 ||
			(
				mBitDepth != 0 &&
				mBitDepth != 8 &&
				mBitDepth != 15 &&
				mBitDepth != 16 &&
				mBitDepth != 24 &&
				mBitDepth != 32
			)
		  )
		{
			return false;
		}

		return true;
	}

	int mWidth;
	int mHeight;
	int mBitDepth;      // Set to 0 means "don't care". Will be set to highest possible value.
	int mRefreshRate;   // Set to 0 means "don't care", the system will 
                            // use the highest supported frequency.
};

class DisplayResizeObserver
{
public:
	virtual void OnResize(int pWidth, int pHeight) = 0;
	virtual void OnMinimize() = 0;
	virtual void OnMaximize(int pWidth, int pHeight) = 0;
};


class DisplayManager: public DisplayResizeObserver
{
public:

	enum ScreenMode
	{
		WINDOWED,		// Windowed and resizable.
		STATIC_WINDOW,	// Windowed and not resizable.
		SPLASH_WINDOW,	// No border.
		FULLSCREEN,
	};

	// On other platforms than Windows, DIRECTX_CONTEXT may 
	// be equivalent to OPENGL_CONTEXT.
	enum ContextType
	{
		OPENGL_CONTEXT,
		DIRECTX_CONTEXT,
		SOFTWARE_CONTEXT
	};

	// Creates a platform dependent display manager. This function
	// is implemented elsewhere, (not in DisplayManager.cpp). The 
	// exact location of this function depends on the platform 
	// specific implementation. There can be only one display manager.
	// If this function is called twice with different contexts,
	// the first display manager will be deleted.
	static DisplayManager* CreateDisplayManager(ContextType pCT);

	DisplayManager();
	virtual ~DisplayManager();

	virtual ContextType GetContextType() = 0;

	// Sets the caption of the window.
	virtual void SetCaption(const Lepra::String& pCaption) = 0;

	// Creates a window in windowed or fullscreen mode on the main display device.
	virtual bool OpenScreen(const DisplayMode& pDisplayMode, ScreenMode pMode) = 0;
	virtual void CloseScreen() = 0;

	void AddResizeObserver(DisplayResizeObserver* pResizeObserver);
	void RemoveResizeObserver(DisplayResizeObserver* pResizeObserver);
	void DispatchResize(int pWidth, int pHeight);
	void DispatchMinimize();
	void DispatchMaximize(int pWidth, int pHeight);

	virtual bool Activate() = 0;
	// Swaps buffers etc.
	virtual bool UpdateScreen() = 0;

	virtual bool IsVSyncEnabled() = 0;
	virtual bool SetVSyncEnabled(bool pEnabled) = 0;

	/*
		Display mode enumeration (fullscreen modes).
		Narrow the search by specifying width and height and/or bitrate.
	*/

	// Returns the number of supported display modes on this computer.
	virtual int GetNumDisplayModes() = 0;
	virtual int GetNumDisplayModes(int pBitDepth) = 0;
	virtual int GetNumDisplayModes(int pWidth, int pHeight) = 0;
	virtual int GetNumDisplayModes(int pWidth, int pHeight, int pBitDepth) = 0;

	// Fills pDisplayMode with the mode number pMode.
	// pMode must be a number greater or equal to 0, and less than the number returned from
	// the corresponding GetNumDisplayModes().
	virtual bool GetDisplayMode(DisplayMode& pDisplayMode, int pMode) = 0;
	virtual bool GetDisplayMode(DisplayMode& pDisplayMode, int pMode, int pBitDepth) = 0;
	virtual bool GetDisplayMode(DisplayMode& pDisplayMode, int pMode, int pWidth, int pHeight) = 0;
	virtual bool GetDisplayMode(DisplayMode& pDisplayMode, int pMode, int pWidth, int pHeight, int pBitDepth) = 0;

	// Display mode search. Returns true if a matching display mode was found, 
	// and fills the DisplayMode struct.
	virtual bool FindDisplayMode(DisplayMode& pDisplayMode, int pWidth, int pHeight) = 0;
	virtual bool FindDisplayMode(DisplayMode& pDisplayMode, int pWidth, int pHeight, int pBitDepth) = 0;
	virtual bool FindDisplayMode(DisplayMode& pDisplayMode, int pWidth, int pHeight, int pBitDepth, int pRefreshRate) = 0;



	// Returns the display memory pointer if available, NULL otherwise.
	virtual void* GetScreenPtr() = 0;
	virtual unsigned GetWidth() = 0;
	virtual unsigned GetHeight() = 0;
	virtual unsigned GetBitDepth() = 0;
	virtual unsigned GetRefreshRate() = 0;
	virtual unsigned GetPitch() = 0;
	virtual bool IsFullScreen() = 0;

	// Returns the canvas describing the screen. In accelerated modes 
	// (OpenGL, DirectX etc) you can't assume that there is a valid pointer
	// to the display memory. If there is a valid pointer, it's only valid
	// for the current frame.
	void GetScreenCanvas(Lepra::Canvas& pCanvas);
	inline void GetScreenCanvas(Lepra::Canvas* pCanvas);

	// Returns true if the 16 bit mode (565) really is 15 bit (555).
	virtual bool Is15Bit() = 0;

	// Returns the color that is closest to the desired color (pRed, pGreen, pBlue).
	// Do not use this in time critical loops!
	virtual Lepra::uint8 GetPaletteColor(int pRed, int pGreen, int pBlue) = 0;
	virtual const Lepra::Color* GetPaletteColor(unsigned pIndex) = 0;

	virtual void SetPalette(const Lepra::Color* pPalette) = 0;

	// Functions implemented for conveniency.
	static Lepra::uint8 GetPaletteColor(int pRed, int pGreen, int pBlue, const Lepra::Color* pPalette);

protected:
	typedef std::set<DisplayResizeObserver*> ResizeObserverSet;
	ResizeObserverSet mResizeObserverSet;

private:
	LOG_CLASS_DECLARE();
};

void DisplayManager::GetScreenCanvas(Lepra::Canvas* pCanvas)
{
	GetScreenCanvas(*pCanvas);
}



}
