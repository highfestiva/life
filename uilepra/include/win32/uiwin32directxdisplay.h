/*
	Class:  Win32DirectXDisplay
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#pragma once

#include "uiwin32displaymanager.h"
#include <D3d9.h>

namespace uilepra {

class Win32DirectXDisplay : public Win32DisplayManager {
public:

	Win32DirectXDisplay();
	virtual ~Win32DirectXDisplay();

	virtual void CloseScreen();
	virtual bool Activate();
	virtual bool Deactivate();
	virtual bool UpdateScreen();

	bool IsVSyncEnabled() const;
	bool SetVSyncEnabled(bool enabled);

	inline ContextType GetContextType();

	inline IDirect3DDevice9* GetD3DDevice();

protected:
	void OnResize(int width, int height);
	void OnMinimize();
	void OnMaximize(int width, int height);

	bool InitScreen();
	void SetFocus(bool focus);

private:
	void Resize(int width, int height);
	void UpdateCaption();

	void InitD3DPresentParams(D3DPRESENT_PARAMETERS& params);

	LPDIRECT3D9 d3_d_;
	IDirect3DDevice9* d3_d_device_;

	bool v_sync_enabled_;
};

DisplayManager::ContextType Win32DirectXDisplay::GetContextType() {
	return DisplayManager::kDirectxContext;
}

IDirect3DDevice9* Win32DirectXDisplay::GetD3DDevice() {
	return d3_d_device_;
}

}
