
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uiopenglextensions.h"
#include "uimacdisplaymanager.h"
#ifdef LEPRA_IOS
#include "eaglview.h"
#endif // iOS



namespace uilepra {



class MacOpenGLDisplay: public MacDisplayManager {
	typedef MacDisplayManager Parent;
public:
	MacOpenGLDisplay();
	virtual ~MacOpenGLDisplay();

	virtual void CloseScreen();
	virtual bool Activate();
	virtual bool Deactivate();
	virtual bool UpdateScreen();
	virtual void SetOrientation(Orientation orientation);

	bool IsVSyncEnabled() const;
	bool SetVSyncEnabled(bool enabled);

	ContextType GetContextType();
	LEPRA_APPLE_GL_VIEW* GetGlView() const;

protected:
	void OnResize(int width, int height);
	void OnMinimize();
	void OnMaximize(int width, int height);

	bool InitScreen();
	void SetFocus(bool focus);

private:
	void Resize(int width, int height);
	void UpdateCaption();

	bool CreateGLContext();
	void DeleteGLContext();
	bool SetGLPixelFormat();

	LEPRA_APPLE_GL_VIEW* gl_view_;
	static LEPRA_APPLE_GL_CONTEXT* gl_context_;
	static int context_user_count_;
};



}
