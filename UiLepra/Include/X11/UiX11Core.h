
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <X11/X.h>
#include <X11/Xlib.h>
#include "../../../lepra/include/hashtable.h"
#include "../../../lepra/include/thread.h"
#include "../uicore.h"
#include "../uilepra.h"



namespace uilepra {



class X11DisplayManager;



class X11Core {
public:
	static void Init();
	static void Shutdown();

	// Takes care of all incoming window messages.
	// Must be called at least once "per frame".
	static void ProcessMessages();

	static void AddDisplayManager(X11DisplayManager* display_manager);
	static void RemoveDisplayManager(X11DisplayManager* display_manager);
	static X11DisplayManager* GetDisplayManager(Window window_handle);

private:
	static Lock* lock_;
	typedef HashTable<Window, X11DisplayManager*, std::hash<Window> > WindowTable;
	static WindowTable window_table_;
};



}
