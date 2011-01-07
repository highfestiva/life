
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

//#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>

#include "../../../Lepra/Include/HashTable.h"
#include "../../../Lepra/Include/Thread.h"
#include "../UiCore.h"
#include "../UiLepra.h"


@interface UiMacApplication : NSApplication
{

}

- (void)run;
- (void)terminate:(id)sender;

@end

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
	static MacDisplayManager* GetDisplayManager(NSWindow* pWindowHandle);

private:
	static Lock* mLock;
	typedef HashTable<NSWindow*, MacDisplayManager*, LEPRA_VOIDP_HASHER> WindowTable;
	static WindowTable mWindowTable;

	static UiMacApplication* mApplication;

};



}
