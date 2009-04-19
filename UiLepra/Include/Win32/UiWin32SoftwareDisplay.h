/*
	Class:  Win32SoftwareDisplay
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#ifndef UIWIN32SOFTWAREDISPLAY_H
#define UIWIN32SOFTWAREDISPLAY_H

#include "UiWin32DisplayManager.h"

namespace UiLepra
{

class Win32SoftwareDisplay : public Win32DisplayManager
{
public:
	
	Win32SoftwareDisplay();
	virtual ~Win32SoftwareDisplay();

	void CloseScreen();
	bool Activate();
	bool UpdateScreen();

	bool IsVSyncEnabled() const;
	bool SetVSyncEnabled(bool pEnabled);

	inline ContextType GetContextType();

	void SetPalette(const Lepra::Color* pPalette);

	unsigned GetPitch();

	void* GetScreenPtr();

protected:
	void OnResize(int pWidth, int pHeight);
	void OnMinimize();
	void OnMaximize(int pWidth, int pHeight);

	bool InitScreen();
	void SetFocus(bool pFocus);

private:

	void CreateNewDIB();
	void DeleteDIB();
	void Resize(int pWidth, int pHeight);

	void UpdateCaption();

	// Used when minimizing window.
	void* mDummyScreen;

	struct Win32BitmapInfo
	{
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD			 bmiColors[256];
	};
	
	struct Win32LogPalette
	{
		WORD         palVersion;
		WORD         palNumEntries;
		PALETTEENTRY palPalEntry[256];
	};

	Win32BitmapInfo* mBitmapInfo;
	Win32BitmapInfo* mAlphaInfo;
	Win32LogPalette* mPalInfo;

	HBITMAP    mBitmap;         // A handle to the bitmap.
	HBITMAP    mDesktopBitmap;  // Used in order to render transparent images..
};

DisplayManager::ContextType Win32SoftwareDisplay::GetContextType()
{
	return DisplayManager::SOFTWARE_CONTEXT;
}

} // End namespace.

#endif
