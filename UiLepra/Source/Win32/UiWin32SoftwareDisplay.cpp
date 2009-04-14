/*
	Class:  Win32SoftwareDisplay
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include <stdio.h>
#include "../../../Lepra/Include/LepraTarget.h"
#include "../../../Lepra/Include/String.h"
#include "../../Include/Win32/UiWin32SoftwareDisplay.h"

#pragma comment(lib, "msimg32.lib")

namespace UiLepra
{

Win32SoftwareDisplay::Win32SoftwareDisplay() :
	mDummyScreen(0),
	mBitmapInfo(0),
	mAlphaInfo(0),
	mPalInfo(0)
{
}

Win32SoftwareDisplay::~Win32SoftwareDisplay()
{
	if (mScreenOpened == true) 
	{
		CloseScreen();
	}
}

void* Win32SoftwareDisplay::GetScreenPtr()
{
	if (mInitialized == false)
	{
		return 0;
	}

	if (IsMinimized() == true)
	{
		return mDummyScreen;
	}

	return Win32DisplayManager::GetScreenPtr();
}

unsigned Win32SoftwareDisplay::GetPitch()
{
	return mDisplayMode.mWidth;
}

void Win32SoftwareDisplay::CloseScreen()
{
	if (mScreenOpened == false)
	{
		return;
	}

	if (mScreenMode == DisplayManager::FULLSCREEN)
	{
		::ChangeDisplaySettings(NULL, 0);
	}

	DeleteDIB();

	Win32DisplayManager::CloseScreen();
}

bool Win32SoftwareDisplay::Activate()
{
	return (true);
}

bool Win32SoftwareDisplay::UpdateScreen()
{
	RECT lClientRect;

	HDC lScreenDC = GetDC(mWnd);
	::GetClientRect(mWnd, &lClientRect);

	HDC lContext = ::CreateCompatibleDC(lScreenDC);
	HBITMAP lDefaultBitmap = SelectBitmap(lContext, mBitmap);

	int lWidth  = lClientRect.right  - lClientRect.left;
	int lHeight = lClientRect.bottom - lClientRect.top;

	if (::GetDeviceCaps(lScreenDC, SHADEBLENDCAPS) & SB_PIXEL_ALPHA && GetBitDepth() == 32)
	{
		BLENDFUNCTION lBlendFunc;
		lBlendFunc.BlendOp             = AC_SRC_OVER;
		lBlendFunc.BlendFlags          = 0;
		lBlendFunc.AlphaFormat         = AC_SRC_ALPHA;
		lBlendFunc.SourceConstantAlpha = 255;

		::AlphaBlend(lScreenDC, 0, 0, lWidth, lHeight, 
					 lContext, 0, 0, lWidth, lHeight, lBlendFunc);
	}
	else
	{
		::BitBlt(lScreenDC, 0, 0, lWidth, lHeight,
				 lContext, 0, 0, SRCCOPY);
	}

	SelectBitmap(lContext, lDefaultBitmap);
	::DeleteDC(lContext);
	::DeleteObject(lDefaultBitmap);
	::ReleaseDC(mWnd, lScreenDC);

	return (true);
}

bool Win32SoftwareDisplay::IsVSyncEnabled()
{
	// TODO: Implement!
	return false;
}

bool Win32SoftwareDisplay::SetVSyncEnabled(bool /*pEnabled*/)
{
	// TODO: Implement!
	return (false);
}

void Win32SoftwareDisplay::SetPalette(const Lepra::Color* pPalette)
{
	Win32DisplayManager::SetPalette(pPalette);

	HDC lScreenDC = GetDC(mWnd);
	HPALETTE lPalette;
	int i;

	if (mInitialized == false)
	{
		return;
	}

	if (mDisplayMode.mBitDepth != 8)
	{
		return;
	}

	mPalInfo->palVersion = 0x300;
	mPalInfo->palNumEntries = 256;

	for (i = 0; i < 256; i++) 
	{
		mBitmapInfo->bmiColors[i].rgbRed   = mPalette[i].mRed;
		mBitmapInfo->bmiColors[i].rgbGreen = mPalette[i].mGreen;
		mBitmapInfo->bmiColors[i].rgbBlue  = mPalette[i].mBlue;

		mPalInfo->palPalEntry[i].peRed   = mPalette[i].mRed;
		mPalInfo->palPalEntry[i].peGreen = mPalette[i].mGreen;
		mPalInfo->palPalEntry[i].peBlue  = mPalette[i].mBlue;
		mPalInfo->palPalEntry[i].peFlags = PC_NOCOLLAPSE;//PC_RESERVED;
	}

	lPalette = CreatePalette((tagLOGPALETTE*) mPalInfo);
	::SelectPalette(lScreenDC, lPalette, TRUE);
	::RealizePalette(lScreenDC);

	::DeleteObject(lPalette);
	::DeleteObject(mBitmap);

	mBitmap = ::CreateDIBSection(lScreenDC, 
								   (BITMAPINFO*) mBitmapInfo, 
								   DIB_RGB_COLORS,
								   (VOID**)&mScreen, NULL, NULL);

	::ReleaseDC(mWnd, lScreenDC);
}

void Win32SoftwareDisplay::OnResize(int pWidth, int pHeight)
{
	Resize(pWidth, pHeight);
	::ShowWindow(mWnd, SW_SHOWNORMAL);
}

void Win32SoftwareDisplay::Resize(int pWidth, int pHeight)
{
	if (mDummyScreen != 0)
	{
		delete[] mDummyScreen;
		mDummyScreen = 0;
	}

	if (IsFullScreen() == true)
	{
		// Since we are in fullscreen mode, we ignore the width and height
		// given as parameters.
		if (IsMinimized() == true)
		{
			DEVMODE lNewMode;
			lNewMode.dmBitsPerPel       = mDisplayMode.mBitDepth;
			lNewMode.dmPelsWidth        = mDisplayMode.mWidth;
			lNewMode.dmPelsHeight       = mDisplayMode.mHeight;
			lNewMode.dmDisplayFrequency = mDisplayMode.mRefreshRate;
			lNewMode.dmFields = DM_BITSPERPEL | 
								 DM_PELSWIDTH  | 
								 DM_PELSHEIGHT | 
								 DM_DISPLAYFREQUENCY;

			::ChangeDisplaySettings(&lNewMode, CDS_FULLSCREEN);
		}
	}
	else
	{
		mDisplayMode.mWidth  = pWidth;
		mDisplayMode.mHeight = pHeight;

		DeleteDIB();
		CreateNewDIB();
	}
}

void Win32SoftwareDisplay::OnMinimize()
{
	if (mDummyScreen == 0)
	{
		int lScreenMemSize = GetPitch() * GetHeight() * (GetBitDepth() >> 3);
		mDummyScreen = (void*)(new Lepra::uint8[lScreenMemSize]);
		memset(mDummyScreen, 0, lScreenMemSize);
	}

	if (IsFullScreen() == true)
	{
		::ChangeDisplaySettings(NULL, 0);
	}

	::ShowWindow(mWnd, SW_MINIMIZE);
}

void Win32SoftwareDisplay::OnMaximize(int pWidth, int pHeight)
{
	Resize(pWidth, pHeight);
	::ShowWindow(mWnd, SW_SHOWMAXIMIZED);
}

bool Win32SoftwareDisplay::InitScreen()
{
	if (mScreenMode == FULLSCREEN)
	{
		DEVMODE lNewMode;
		lNewMode.dmSize             = sizeof(lNewMode);
		lNewMode.dmBitsPerPel       = mDisplayMode.mBitDepth;
		lNewMode.dmPelsWidth        = mDisplayMode.mWidth;
		lNewMode.dmPelsHeight       = mDisplayMode.mHeight;
		lNewMode.dmDisplayFrequency = mDisplayMode.mRefreshRate;
		lNewMode.dmFields = DM_BITSPERPEL | 
							 DM_PELSWIDTH  | 
							 DM_PELSHEIGHT | 
							 DM_DISPLAYFREQUENCY;

		if (ChangeDisplaySettings(&lNewMode, CDS_FULLSCREEN) !=
		   DISP_CHANGE_SUCCESSFUL)
		{
			//return false;
		}

		::MoveWindow(mWnd, 0, 0, mDisplayMode.mWidth, mDisplayMode.mHeight, TRUE);
	}
	else if(mScreenMode == DisplayManager::WINDOWED ||
			mScreenMode == DisplayManager::STATIC_WINDOW)
	{
		int lWindowWidth  = GetWindowWidth(mDisplayMode.mWidth);
		int lWindowHeight = GetWindowHeight(mDisplayMode.mHeight);

		int lX = GetSystemMetrics(SM_CXSCREEN) / 2 - mDisplayMode.mWidth / 2;
		int lY = GetSystemMetrics(SM_CYSCREEN) / 2 - mDisplayMode.mHeight / 2;

		::MoveWindow(mWnd, lX, lY, lWindowWidth, lWindowHeight, TRUE);
	}
	else
	{
		int lX = GetSystemMetrics(SM_CXSCREEN) / 2 - mDisplayMode.mWidth / 2;
		int lY = GetSystemMetrics(SM_CYSCREEN) / 2 - mDisplayMode.mHeight / 2;

		::MoveWindow(mWnd, lX, lY, mDisplayMode.mWidth, mDisplayMode.mHeight, TRUE);
	}

	CreateNewDIB();

	return true;
}

void Win32SoftwareDisplay::UpdateCaption()
{
	Lepra::String lString = Lepra::StringUtility::Format(_T("Win32SoftwareDisplay (%ix%i %iBit %iHz) %s"),
		 mDisplayMode.mWidth, mDisplayMode.mHeight,
		 mDisplayMode.mBitDepth, mDisplayMode.mRefreshRate,
		 _T(LEPRA_STRING_TYPE_TEXT) _T(" ") _T(LEPRA_BUILD_TYPE_TEXT));
	SetCaption(lString, true);
}

void Win32SoftwareDisplay::CreateNewDIB()
{
	HDC lScreenDC = ::GetDC(mWnd);

	mBitmapInfo = new Win32BitmapInfo;
	mAlphaInfo  = new Win32BitmapInfo;
	mPalInfo	  = new Win32LogPalette;

	mBitmapInfo->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
	mBitmapInfo->bmiHeader.biWidth         = mDisplayMode.mWidth;
	mBitmapInfo->bmiHeader.biHeight        = -(int)mDisplayMode.mHeight; // Top-down bitmap
	mBitmapInfo->bmiHeader.biPlanes        = 1;
	mBitmapInfo->bmiHeader.biBitCount      = (WORD)mDisplayMode.mBitDepth;
	mBitmapInfo->bmiHeader.biCompression   = BI_RGB;
	mBitmapInfo->bmiHeader.biSizeImage     = NULL;
	mBitmapInfo->bmiHeader.biXPelsPerMeter = NULL;
	mBitmapInfo->bmiHeader.biYPelsPerMeter = NULL;
	mBitmapInfo->bmiHeader.biClrUsed       = 256;
	mBitmapInfo->bmiHeader.biClrImportant  = 256;

	mAlphaInfo->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
	mAlphaInfo->bmiHeader.biWidth         = mDisplayMode.mWidth;
	mAlphaInfo->bmiHeader.biHeight        = -(int)mDisplayMode.mHeight; // Top-down bitmap
	mAlphaInfo->bmiHeader.biPlanes        = 1;
	mAlphaInfo->bmiHeader.biBitCount      = 8;
	mAlphaInfo->bmiHeader.biCompression   = BI_RGB;
	mAlphaInfo->bmiHeader.biSizeImage     = NULL;
	mAlphaInfo->bmiHeader.biXPelsPerMeter = NULL;
	mAlphaInfo->bmiHeader.biYPelsPerMeter = NULL;
	mAlphaInfo->bmiHeader.biClrUsed       = 256;
	mAlphaInfo->bmiHeader.biClrImportant  = 256;

	if (mDisplayMode.mBitDepth == 15 || 
	   mDisplayMode.mBitDepth == 16)
	{
		mBitmapInfo->bmiHeader.biBitCount = 16;
		mBitmapInfo->bmiHeader.biCompression   = BI_BITFIELDS;
		mAlphaInfo ->bmiHeader.biCompression   = BI_BITFIELDS;

		if (mDisplayMode.mBitDepth == 16)
		{
			((unsigned *)mBitmapInfo->bmiColors)[0] = 0xF800;	//Red mask
			((unsigned *)mBitmapInfo->bmiColors)[1] = 0x07E0;	//Green mask
			((unsigned *)mBitmapInfo->bmiColors)[2] = 0x001F;	//Blue mask
		}
		else
		{
			((unsigned *)mBitmapInfo->bmiColors)[0] = 0x7C00;	//Red mask
			((unsigned *)mBitmapInfo->bmiColors)[1] = 0x03E0;	//Green mask
			((unsigned *)mBitmapInfo->bmiColors)[2] = 0x001F;	//Blue mask
		}
	}
	else if(mDisplayMode.mBitDepth == 8)
	{
		for (int i = 0; i < 256; i++) 
		{
			mBitmapInfo->bmiColors[i].rgbRed   = mPalette[i].mRed;
			mBitmapInfo->bmiColors[i].rgbGreen = mPalette[i].mGreen;
			mBitmapInfo->bmiColors[i].rgbBlue  = mPalette[i].mBlue;

			mPalInfo->palPalEntry[i].peRed   = mPalette[i].mRed;
			mPalInfo->palPalEntry[i].peGreen = mPalette[i].mGreen;
			mPalInfo->palPalEntry[i].peBlue  = mPalette[i].mBlue;
			mPalInfo->palPalEntry[i].peFlags = PC_NOCOLLAPSE;//PC_RESERVED;
		}
	}

	mPalInfo->palVersion = 0x300;
	mPalInfo->palNumEntries = 256;

	HPALETTE lPalette;

	lPalette = ::CreatePalette((tagLOGPALETTE*)mPalInfo);
	::SelectPalette(lScreenDC, lPalette, FALSE);
	::RealizePalette(lScreenDC);
	::DeleteObject(lPalette);

	mBitmap = ::CreateDIBSection(lScreenDC, (BITMAPINFO *)mBitmapInfo, DIB_RGB_COLORS, (VOID **)&mScreen, NULL, NULL);
//	mAlpha  = CreateDIBSection(lScreenDC, (BITMAPINFO *)mAlphaInfo, DIB_RGB_COLORS, (VOID **)&mAlpha, NULL, NULL);

	if (mBitmap == NULL/* || mAlpha == NULL*/)
	{
		LPVOID lMsgBuf;

		FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lMsgBuf,
				0,
				NULL);

		Lepra::String lString = Lepra::StringUtility::Format(_T("TSPWin32_Standard::InitScreen()\nCreateDIBSection()\n\nError Code: %i\nError Message: %s"),
						 GetLastError(), (LPCTSTR)lMsgBuf);
		ShowMessageBox(lString, _T("Error!"));

		LocalFree(lMsgBuf);
	}	

	ReleaseDC(mWnd, lScreenDC);

	UpdateCaption();
}

void Win32SoftwareDisplay::DeleteDIB()
{
	if (mInitialized == true)
	{
		delete mBitmapInfo;
		delete mAlphaInfo;
		delete mPalInfo;
		DeleteObject(mBitmap);
	}
}

void Win32SoftwareDisplay::SetFocus(bool pFocus)
{
	if (IsFullScreen() == true)
	{
		if (pFocus == false)
		{
			OnMinimize();
		}
		else
		{
			OnResize(mDisplayMode.mWidth, mDisplayMode.mHeight);
		}
	}
}

} // End namespace.
