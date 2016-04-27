
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiX11DisplayManager.h"
#include <GL/glx.h>



namespace UiLepra
{



class X11OpenGLDisplay: public X11DisplayManager
{
public:	
	X11OpenGLDisplay();
	virtual ~X11OpenGLDisplay();

	virtual void CloseScreen();
	virtual bool Activate();
	virtual bool Deactivate();
	virtual bool UpdateScreen();

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
	void Resize(int pWidth, int pHeight);
	void UpdateCaption();

	bool CreateGLContext();
	void DeleteGLContext();

	static GLXContext mGlContext;	// The same one used for all displays (must have same VisualInfo).
	static int mContextUserCount;

	logclass();
};



}
