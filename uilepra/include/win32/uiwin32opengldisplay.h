
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uiwin32displaymanager.h"



namespace uilepra {



class Win32OpenGLDisplay: public Win32DisplayManager {
	typedef Win32DisplayManager Parent;
public:
	Win32OpenGLDisplay();
	virtual ~Win32OpenGLDisplay();

	virtual void CloseScreen();
	virtual bool Activate();
	virtual bool Deactivate();
	virtual bool UpdateScreen();

	bool IsVSyncEnabled() const;
	bool SetVSyncEnabled(bool enabled);

	ContextType GetContextType();

protected:
	void OnResize(int width, int height);
	void OnMinimize();
	void OnMaximize(int width, int height);

	bool InitScreen();
	void SetFocus(bool focus);

private:
	void Deactivate();

	void Resize(int width, int height);
	void UpdateCaption();

	bool CreateGLContext();
	void DeleteGLContext();
	bool SetGLPixelFormat();

	HDC dc_;
	static HGLRC gl_context_;	// The same one used for all windows (must be of same pixel format).
	static int context_user_count_;
};



}
