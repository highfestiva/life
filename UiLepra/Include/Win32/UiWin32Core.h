/*
	Class:  Win32Core,
		Win32Observer
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	This class (Win32Core) exists only because of the stupid design of
	Windows	and its API. It is a central "place" (container or whatever)
	where commonly used data as the HWND, the HINSTANCE etc can be stored.
	It also provides the core of all Windows programming - the WndProc().

	Win32Observer is a virtual class meant to be derived from by classes
	that are interested in "Window Messages".
*/



#pragma once

#include "../../../lepra/include/hashtable.h"
#include "../../../lepra/include/lepraos.h"
#include "../../../lepra/include/thread.h"
#include "../uicore.h"
#include "../uilepra.h"



namespace uilepra {



class Win32DisplayManager;

class Win32Core {
public:
	static void Init();
	static void Shutdown();

	static HINSTANCE GetAppInstance();

	// Takes care of all incoming Window messages.
	// Must be called at least once "per frame".
	static void ProcessMessages();

	static void AddDisplayManager(Win32DisplayManager* display_manager);
	static void RemoveDisplayManager(Win32DisplayManager* display_manager);
	static Win32DisplayManager* GetDisplayManager(HWND window_handle);

private:
	static Lock* lock_;
	typedef HashTable<HWND, Win32DisplayManager*, LEPRA_VOIDP_HASHER> WindowTable;
	static WindowTable window_table_;
};



}
