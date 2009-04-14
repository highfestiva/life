/*
	Class:  Win32OpenGLDisplay
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#ifndef UIWIN32OPENGLDISPLAY_H
#define UIWIN32OPENGLDISPLAY_H

#include "UiWin32DisplayManager.h"

namespace UiLepra
{

class Win32OpenGLDisplay : public Win32DisplayManager
{
public:
	
	Win32OpenGLDisplay();
	virtual ~Win32OpenGLDisplay();

	void CloseScreen();
	bool Activate();
	bool UpdateScreen();

	bool IsVSyncEnabled();
	bool SetVSyncEnabled(bool pEnabled);

	inline ContextType GetContextType();

	unsigned GetPitch();

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

DisplayManager::ContextType Win32OpenGLDisplay::GetContextType()
{
	return DisplayManager::OPENGL_CONTEXT;
}

} // End namespace.

#endif
