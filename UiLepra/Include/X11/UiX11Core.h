
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <X11/X.h>
#include <X11/Xlib.h>
#include "../../../Lepra/Include/HashTable.h"
#include "../../../Lepra/Include/Thread.h"
#include "../UiCore.h"
#include "../UiLepra.h"



namespace UiLepra
{



class X11DisplayManager;



class X11Core
{
public:
	static void Init();
	static void Shutdown();

	// Takes care of all incoming window messages.
	// Must be called at least once "per frame".
	static void ProcessMessages();

	static void AddDisplayManager(X11DisplayManager* pDisplayManager);
	static void RemoveDisplayManager(X11DisplayManager* pDisplayManager);
	static X11DisplayManager* GetDisplayManager(Window pWindowHandle);

private:
	static Lock* mLock;
	typedef HashTable<Window, X11DisplayManager*, std::hash<void*> > WindowTable;
	static WindowTable mWindowTable;
};



}
