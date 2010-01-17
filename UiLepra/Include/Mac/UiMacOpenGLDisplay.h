
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "UiMacDisplayManager.h"
#include <GL/glx.h>
#include <Mac/Xutil.h>



namespace UiLepra
{



class MacOpenGLDisplay: public MacDisplayManager
{
public:	
	MacOpenGLDisplay();
	virtual ~MacOpenGLDisplay();

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
	XVisualInfo* GetVisualInfo() const;

	static GLXContext mGlContext;	// The same one used for all displays (must have same VisualInfo).
	static int mContextUserCount;
};



}
