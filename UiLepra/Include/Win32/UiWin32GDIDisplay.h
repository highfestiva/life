
// Author: Jonas Byström
// Copyright (c) 2012, Pixel Doctrine



#pragma once

#include "UiWin32DisplayManager.h"




namespace UiLepra
{



class Win32GDIDisplay: public Win32DisplayManager
{
public:
	Win32GDIDisplay();
	virtual ~Win32GDIDisplay();

protected:
	virtual ContextType GetContextType();

	virtual bool Activate();
	virtual bool UpdateScreen();

	virtual bool IsVSyncEnabled() const;
	virtual bool SetVSyncEnabled(bool pEnabled);

	virtual void OnResize(int pWidth, int pHeight);

	virtual void OnMinimize();
	virtual void OnMaximize(int pWidth, int pHeight);

	virtual bool InitScreen();
	virtual void SetFocus(bool pFocus);
};



}
