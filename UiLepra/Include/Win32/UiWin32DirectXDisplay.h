/*
	Class:  Win32DirectXDisplay
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#pragma once

#include "UiWin32DisplayManager.h"
#include <D3d9.h>

namespace UiLepra
{

class Win32DirectXDisplay : public Win32DisplayManager
{
public:
	
	Win32DirectXDisplay();
	virtual ~Win32DirectXDisplay();

	void CloseScreen();
	bool Activate();
	bool UpdateScreen();

	bool IsVSyncEnabled() const;
	bool SetVSyncEnabled(bool pEnabled);

	inline ContextType GetContextType();

	inline IDirect3DDevice9* GetD3DDevice();

protected:
	void OnResize(int pWidth, int pHeight);
	void OnMinimize();
	void OnMaximize(int pWidth, int pHeight);

	bool InitScreen();
	void SetFocus(bool pFocus);

private:
	void Resize(int pWidth, int pHeight);
	void UpdateCaption();

	void InitD3DPresentParams(D3DPRESENT_PARAMETERS& pParams);

	LPDIRECT3D9 mD3D;
	IDirect3DDevice9* mD3DDevice;

	bool mVSyncEnabled;
};

DisplayManager::ContextType Win32DirectXDisplay::GetContextType()
{
	return DisplayManager::DIRECTX_CONTEXT;
}

IDirect3DDevice9* Win32DirectXDisplay::GetD3DDevice()
{
	return mD3DDevice;
}

}
