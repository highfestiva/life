/*
	Class:  Win32Core, 
		Win32Observer
	Author: Jonas Bystr�m
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

#include "../../../Lepra/Include/HashTable.h"
#include "../../../Lepra/Include/Thread.h"
#include "../UiCore.h"
#include "../UiLepra.h"



namespace UiLepra
{



class Win32DisplayManager;

class Win32Core
{
public:
	static void Init();
	static void Shutdown();

	static HINSTANCE GetAppInstance();

	// Takes care of all incoming Window messages.
	// Must be called at least once "per frame".
	static void ProcessMessages();

	static void AddDisplayManager(Win32DisplayManager* pDisplayManager);
	static void RemoveDisplayManager(Win32DisplayManager* pDisplayManager);
	static Win32DisplayManager* GetDisplayManager(HWND pWindowHandle);

private:
	static Lock* mLock;
	typedef HashTable<HWND, Win32DisplayManager*, LEPRA_VOIDP_HASHER> WindowTable;
	static WindowTable mWindowTable;
};



}
