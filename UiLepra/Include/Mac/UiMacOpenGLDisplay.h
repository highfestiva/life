
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../UiOpenGLExtensions.h"
#include "UiMacDisplayManager.h"
#ifdef LEPRA_IOS
#include "EAGLView.h"
#endif // iOS



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

	LEPRA_APPLE_GL_VIEW* mGlView;
	static LEPRA_APPLE_GL_CONTEXT* mGlContext;
	static int mContextUserCount;
};



}
