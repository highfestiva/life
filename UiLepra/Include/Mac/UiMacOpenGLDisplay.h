
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "UiMacDisplayManager.h"
#include <OpenGL/gl.h>

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
	bool SetGLPixelFormat();

	NSOpenGLView* mGlView;
	static NSOpenGLContext* mGlContext;
	static int mContextUserCount;
};



}
