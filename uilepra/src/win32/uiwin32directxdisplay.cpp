/*
	Class:  Win32DirectXDisplay
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include <stdio.h>
#include "../../../lepra/include/string.h"
#include "../../include/win32/uiwin32directxdisplay.h"

#pragma comment(lib, "d3d9.lib")

namespace uilepra {

Win32DirectXDisplay::Win32DirectXDisplay() :
	v_sync_enabled_(true) {
}

Win32DirectXDisplay::~Win32DirectXDisplay() {
	CloseScreen();
}

void Win32DirectXDisplay::CloseScreen() {
	if (screen_opened_ == false) {
		return;
	}

	if (d3_d_device_ != 0)
		d3_d_device_->Release();

	if(d3_d_ != 0) {
		d3_d_->Release();
	}

	if (IsFullScreen() == true) {
		ChangeDisplaySettings(NULL, 0);
	}

	Win32DisplayManager::CloseScreen();
}

bool Win32DirectXDisplay::Activate() {
	// TODO: implement!
	return (true);
}

bool Win32DirectXDisplay::Deactivate() {
	// TODO: implement!
	return (true);
}

bool Win32DirectXDisplay::UpdateScreen() {
	if (screen_opened_ == false) {
		return (false);
	}

	// Disable scissor test to make sure that the entire screen will
	// be updated.
	DWORD value;
	d3_d_device_->GetRenderState(D3DRS_SCISSORTESTENABLE, &value);
	d3_d_device_->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	d3_d_device_->Present(NULL, NULL, NULL, NULL);

	d3_d_device_->SetRenderState(D3DRS_SCISSORTESTENABLE, value);
	return (true);
}

bool Win32DirectXDisplay::IsVSyncEnabled() const {
	return v_sync_enabled_;
}

bool Win32DirectXDisplay::SetVSyncEnabled(bool enabled) {
	v_sync_enabled_ = enabled;

	D3DPRESENT_PARAMETERS _params;
	InitD3DPresentParams(_params);
	d3_d_device_->Reset(&_params);
	return (true);
}


void Win32DirectXDisplay::OnResize(int width, int height) {
	Resize(width, height);
	::ShowWindow(wnd_, SW_SHOWNORMAL);
	Activate();
}

void Win32DirectXDisplay::OnMinimize() {
	if (IsFullScreen() == true) {
		::ChangeDisplaySettings(NULL, 0);
	}

	::ShowWindow(wnd_, SW_MINIMIZE);
}

void Win32DirectXDisplay::OnMaximize(int width, int height) {
	Resize(width, height);
	::ShowWindow(wnd_, SW_SHOWMAXIMIZED);
	Activate();
}

bool Win32DirectXDisplay::InitScreen() {
	UpdateCaption();

	if (screen_mode_ == kFullscreen) {
		DEVMODE new_mode;
		new_mode.dmSize	     = sizeof(new_mode);
		new_mode.dmBitsPerPel       = display_mode_.bit_depth_;
		new_mode.dmPelsWidth	= display_mode_.width_;
		new_mode.dmPelsHeight       = display_mode_.height_;
		new_mode.dmDisplayFrequency = display_mode_.refresh_rate_;
		new_mode.dmFields = DM_BITSPERPEL |
				     DM_PELSWIDTH  |
				     DM_PELSHEIGHT |
				     DM_DISPLAYFREQUENCY;

		if (ChangeDisplaySettings(&new_mode, CDS_FULLSCREEN) !=
		   DISP_CHANGE_SUCCESSFUL) {
			//return false;
		}

		::MoveWindow(wnd_, 0, 0, display_mode_.width_, display_mode_.height_, TRUE);
	} else if(screen_mode_ == DisplayManager::kWindowed ||
		screen_mode_ == DisplayManager::kStaticWindow) {
		int window_width  = GetWindowWidth(display_mode_.width_);
		int window_height = GetWindowHeight(display_mode_.height_);

		int x = GetSystemMetrics(SM_CXSCREEN) / 2 - display_mode_.width_ / 2;
		int y = GetSystemMetrics(SM_CYSCREEN) / 2 - display_mode_.height_ / 2;

		::MoveWindow(wnd_, x, y, window_width, window_height, TRUE);
	} else {
		int x = GetSystemMetrics(SM_CXSCREEN) / 2 - display_mode_.width_ / 2;
		int y = GetSystemMetrics(SM_CYSCREEN) / 2 - display_mode_.height_ / 2;

		::MoveWindow(wnd_, x, y, display_mode_.width_, display_mode_.height_, TRUE);
	}

	d3_d_ = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3_d_ == 0) {
		MessageBox(NULL,
				   "Unable to create the Direct3D interface!",
			 	   "DirectX Error", MB_OK | MB_ICONWARNING );
		return false;
	}

	D3DPRESENT_PARAMETERS d3_d_present_params;
	InitD3DPresentParams(d3_d_present_params);

	HRESULT res = d3_d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd_,
					     D3DCREATE_SOFTWARE_VERTEXPROCESSING,
					     &d3_d_present_params, &d3_d_device_);
	if (FAILED(res)) {
		MessageBox(NULL,
				   "Unable to create the Direct3D device!",
			 	   "DirectX Error", MB_OK | MB_ICONWARNING );
		d3_d_->Release();
		return false;
	}

	d3_d_device_->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	return true;
}

void Win32DirectXDisplay::SetFocus(bool focus) {
	if (IsFullScreen() == true) {
		if (focus == false) {
			DispatchMinimize();
		} else {
			DispatchResize(display_mode_.width_, display_mode_.height_);
		}
	}
}

void Win32DirectXDisplay::Resize(int width, int height) {
	if (IsFullScreen() == true) {
		// Since we are in fullscreen mode, we ignore the width and height
		// given as parameters.
		if (IsMinimized() == true) {
			DEVMODE new_mode;
			new_mode.dmBitsPerPel       = display_mode_.bit_depth_;
			new_mode.dmPelsWidth	= display_mode_.width_;
			new_mode.dmPelsHeight       = display_mode_.height_;
			new_mode.dmDisplayFrequency = display_mode_.refresh_rate_;
			new_mode.dmFields = DM_BITSPERPEL |
								 DM_PELSWIDTH  |
								 DM_PELSHEIGHT |
								 DM_DISPLAYFREQUENCY;

			::ChangeDisplaySettings(&new_mode, CDS_FULLSCREEN);
		}
	} else {
		display_mode_.width_  = width;
		display_mode_.height_ = height;
	}

	//glViewport(0, 0, display_mode_.width_, display_mode_.height_);
	UpdateCaption();
}

void Win32DirectXDisplay::UpdateCaption() {
	str s = strutil::Format("Win32DirectXDisplay (%ix%i %iBit %iHz %s)",
		 display_mode_.width_, display_mode_.height_,
		display_mode_.bit_depth_, display_mode_.refresh_rate_, kLepraBuildTypeText);
	SetCaption(s, true);
}

void Win32DirectXDisplay::InitD3DPresentParams(D3DPRESENT_PARAMETERS& params) {
	::ZeroMemory(&params, sizeof(params));
	params.hDeviceWindow    = wnd_;
	params.BackBufferWidth  = 0;
	params.BackBufferHeight = 0;
	params.FullScreen_RefreshRateInHz = 0;
	params.Windowed = TRUE;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.BackBufferFormat = D3DFMT_UNKNOWN;

	if (v_sync_enabled_ == true) {
		params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	} else {
		params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}
}

}
