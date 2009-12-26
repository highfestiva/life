
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "UiX11DisplayManager.h"



namespace UiLepra
{



class X11OpenGLDisplay: public X11DisplayManager
{
public:	
	X11OpenGLDisplay();
	virtual ~X11OpenGLDisplay();

	void CloseScreen();
	bool Activate();
	bool UpdateScreen();

	bool IsVSyncEnabled() const;
	bool SetVSyncEnabled(bool pEnabled);

	ContextType GetContextType();

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
	static XVisualInfo* GetVisualInfo();

	static GLXContext mGlContext;	// The same one used for all displays (must have same VisualInfo).
	static int mContextUserCount;
};



}
