
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uix11displaymanager.h"
#include <GL/glx.h>



namespace uilepra {



class X11OpenGLDisplay: public X11DisplayManager {
public:
	X11OpenGLDisplay();
	virtual ~X11OpenGLDisplay();

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
	void Resize(int width, int height);
	void UpdateCaption();

	bool CreateGLContext();
	void DeleteGLContext();

	static GLXContext gl_context_;	// The same one used for all displays (must have same VisualInfo).
	static int context_user_count_;

	logclass();
};



}
