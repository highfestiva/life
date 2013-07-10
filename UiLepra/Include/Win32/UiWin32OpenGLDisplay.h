
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiWin32DisplayManager.h"



namespace UiLepra
{



class Win32OpenGLDisplay: public Win32DisplayManager
{
public:	
	Win32OpenGLDisplay();
	virtual ~Win32OpenGLDisplay();

	void CloseScreen();
	bool Activate();
	bool UpdateScreen();

	bool IsVSyncEnabled() const;
	bool SetVSyncEnabled(bool pEnabled);

	ContextType GetContextType();

protected:
	void OnResize(int pWidth, int pHeight);
	void OnMinimize();
	void OnMaximize(int pWidth, int pHeight);

	bool InitScreen();
	void SetFocus(bool pFocus);

private:
	void Deactivate();

	void Resize(int pWidth, int pHeight);
	void UpdateCaption();

	bool CreateGLContext();
	void DeleteGLContext();
	bool SetGLPixelFormat();

	HDC mDC;
	static HGLRC msGlContext;	// The same one used for all windows (must be of same pixel format).
	static int msContextUserCount;
};



}
