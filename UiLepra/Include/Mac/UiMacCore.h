
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <Mac/X.h>
#include <Mac/Xlib.h>
#include "../../../Lepra/Include/HashTable.h"
#include "../../../Lepra/Include/Thread.h"
#include "../UiCore.h"
#include "../UiLepra.h"



namespace UiLepra
{



class MacDisplayManager;



class MacCore
{
public:
	static void Init();
	static void Shutdown();

	// Takes care of all incoming window messages.
	// Must be called at least once "per frame".
	static void ProcessMessages();

	static void AddDisplayManager(MacDisplayManager* pDisplayManager);
	static void RemoveDisplayManager(MacDisplayManager* pDisplayManager);
	static MacDisplayManager* GetDisplayManager(Window pWindowHandle);

private:
	static Lock* mLock;
	typedef HashTable<Window, MacDisplayManager*, std::hash<Window> > WindowTable;
	static WindowTable mWindowTable;
};



}
