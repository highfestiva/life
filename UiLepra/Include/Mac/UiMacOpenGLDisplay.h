
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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
	typedef MacDisplayManager Parent;
public:	
	MacOpenGLDisplay();
	virtual ~MacOpenGLDisplay();

	virtual void CloseScreen();
	virtual bool Activate();
	virtual bool Deactivate();
	virtual bool UpdateScreen();
	virtual void SetOrientation(Orientation pOrientation);

	bool IsVSyncEnabled() const;
	bool SetVSyncEnabled(bool pEnabled);

	ContextType GetContextType();
	LEPRA_APPLE_GL_VIEW* GetGlView() const;

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
	bool SetGLPixelFormat();

	LEPRA_APPLE_GL_VIEW* mGlView;
	static LEPRA_APPLE_GL_CONTEXT* mGlContext;
	static int mContextUserCount;
};



}
