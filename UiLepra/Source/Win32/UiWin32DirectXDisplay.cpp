/*
	Class:  Win32DirectXDisplay
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include <stdio.h>
#include "../../../Lepra/Include/String.h"
#include "../../Include/Win32/UiWin32DirectXDisplay.h"

#pragma comment(lib, "d3d9.lib")

namespace UiLepra
{

Win32DirectXDisplay::Win32DirectXDisplay() :
	mVSyncEnabled(true)
{
}

Win32DirectXDisplay::~Win32DirectXDisplay()
{
	if (mScreenOpened == true) 
	{
		CloseScreen();
	}
}

void Win32DirectXDisplay::CloseScreen()
{
	if (mScreenOpened == false)
	{
		return;
	}

	if (mD3DDevice != 0)
		mD3DDevice->Release();

    if(mD3D != 0)
        mD3D->Release();

	if (IsFullScreen() == true)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	Win32DisplayManager::CloseScreen();
}

bool Win32DirectXDisplay::UpdateScreen()
{
	if (mScreenOpened == false)
	{
		return (false);
	}

	if (mScreen != 0)
	{
		// TODO: Release back buffer.
		mScreen = 0;
	}

	// Disable scissor test to make sure that the entire screen will
	// be updated.
	DWORD lValue;
	mD3DDevice->GetRenderState(D3DRS_SCISSORTESTENABLE, &lValue);
	mD3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	mD3DDevice->Present(NULL, NULL, NULL, NULL);

	mD3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, lValue);
	return (true);
}

bool Win32DirectXDisplay::IsVSyncEnabled() const
{
	return mVSyncEnabled;
}

bool Win32DirectXDisplay::SetVSyncEnabled(bool pEnabled)
{
	mVSyncEnabled = pEnabled;

	D3DPRESENT_PARAMETERS lParams;
	InitD3DPresentParams(lParams);
	mD3DDevice->Reset(&lParams);
	return (true);
}


void* Win32DirectXDisplay::GetScreenPtr()
{
	if (mScreen != 0)
	{
		return mScreen;
	}
	else
	{
		// TODO: Lock backbuffer and retrieve a pointer to it.
		return 0;
	}
}

unsigned Win32DirectXDisplay::GetPitch()
{
	// TODO: Return the actual pitch of the screen.
	return mDisplayMode.mWidth;
}

void Win32DirectXDisplay::OnResize(int pWidth, int pHeight)
{
	Resize(pWidth, pHeight);
	::ShowWindow(mWnd, SW_SHOWNORMAL);
	Activate();
}

void Win32DirectXDisplay::OnMinimize()
{
	if (IsFullScreen() == true)
	{
		::ChangeDisplaySettings(NULL, 0);
	}

	::ShowWindow(mWnd, SW_MINIMIZE);
}

void Win32DirectXDisplay::OnMaximize(int pWidth, int pHeight)
{
	Resize(pWidth, pHeight);
	::ShowWindow(mWnd, SW_SHOWMAXIMIZED);
	Activate();
}

bool Win32DirectXDisplay::InitScreen()
{
	UpdateCaption();

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

	mD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (mD3D == 0)
	{
		MessageBox(NULL,
				   _T("Unable to create the Direct3D interface!"),
			 	   _T("DirectX Error"), MB_OK | MB_ICONWARNING );
		return false;
	}

	D3DPRESENT_PARAMETERS lD3DPresentParams;
	InitD3DPresentParams(lD3DPresentParams);

	HRESULT lRes = mD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mWnd,
					     D3DCREATE_SOFTWARE_VERTEXPROCESSING,
					     &lD3DPresentParams, &mD3DDevice);
	if (FAILED(lRes))
	{
		MessageBox(NULL,
				   _T("Unable to create the Direct3D device!"),
			 	   _T("DirectX Error"), MB_OK | MB_ICONWARNING );
		mD3D->Release();
		return false;
	}

	mD3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	return true;
}

void Win32DirectXDisplay::SetFocus(bool pFocus)
{
	if (IsFullScreen() == true)
	{
		if (pFocus == false)
		{
			DispatchMinimize();
		}
		else
		{
			DispatchResize(mDisplayMode.mWidth, mDisplayMode.mHeight);
		}
	}
}

void Win32DirectXDisplay::Resize(int pWidth, int pHeight)
{
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
	}

	//glViewport(0, 0, mDisplayMode.mWidth, mDisplayMode.mHeight);
	UpdateCaption();
}

void Win32DirectXDisplay::UpdateCaption()
{
	Lepra::String lString = Lepra::StringUtility::Format(_T("Lepra::Win32DirectXDisplay (%ix%i %iBit %iHz) %s"),
		 mDisplayMode.mWidth, mDisplayMode.mHeight,
		mDisplayMode.mBitDepth, mDisplayMode.mRefreshRate, LEPRA_BUILD_TYPE_TEXT);
	SetCaption(lString, true);
}

void Win32DirectXDisplay::InitD3DPresentParams(D3DPRESENT_PARAMETERS& pParams)
{
	::ZeroMemory(&pParams, sizeof(pParams));
	pParams.hDeviceWindow    = mWnd;
	pParams.BackBufferWidth  = 0;
	pParams.BackBufferHeight = 0;
	pParams.FullScreen_RefreshRateInHz = 0;
	pParams.Windowed = TRUE;
	pParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pParams.BackBufferFormat = D3DFMT_UNKNOWN;

	if (mVSyncEnabled == true)
	{
		pParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	}
	else
	{
		pParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}
}

}
