
// Author: Jonas Byström
// Copyright (c) 2012, Pixel Doctrine



#pragma once

#include "uiwin32displaymanager.h"




namespace uilepra {



class Win32GDIDisplay: public Win32DisplayManager {
public:
	Win32GDIDisplay();
	virtual ~Win32GDIDisplay();

protected:
	virtual ContextType GetContextType();

	virtual bool Activate();
	virtual bool Deactivate();
	virtual bool UpdateScreen();

	virtual bool IsVSyncEnabled() const;
	virtual bool SetVSyncEnabled(bool enabled);

	virtual void OnResize(int width, int height);

	virtual void OnMinimize();
	virtual void OnMaximize(int width, int height);

	virtual bool InitScreen();
	virtual void SetFocus(bool focus);
};



}
